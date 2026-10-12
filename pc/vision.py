import os
import time
import requests
import base64
import datetime
from dotenv import load_dotenv
from openai import OpenAI

# Load environment variables
load_dotenv()

ESP32_CAPTURE_URL = os.getenv("ESP32_CAPTURE_URL")
API_KEY = os.getenv("DEEPSEEK_API_KEY")
BASE_URL = os.getenv("DEEPSEEK_BASE_URL")
MODEL_NAME = os.getenv("MODEL_NAME")
DISCORD_WEBHOOK_URL = os.getenv("DISCORD_WEBHOOK_URL")

# Application Settings
POLL_INTERVAL = 10     # Delay between each camera capture (seconds)
ALERT_COOLDOWN = 300   # Minimum delay between Discord alerts (seconds), e.g., 5 minutes

# Global state for cooldown tracking
last_alert_time = 0

# Validate essential configs
if not API_KEY or not BASE_URL:
    raise ValueError("Missing DEEPSEEK_API_KEY or DEEPSEEK_BASE_URL in .env")

# Initialize DeepSeek client
client = OpenAI(
    api_key=API_KEY,
    base_url=BASE_URL
)

def send_discord_alert(status_message, image_bytes=None):
    """Send webhook alert with image to Discord"""
    if not DISCORD_WEBHOOK_URL:
        print("Error: DISCORD_WEBHOOK_URL not configured")
        return

    print("Dispatching Discord cleaning request...")
    
    now = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    alert_text = f"**[Cleaning Request]**\n**Time**: {now}\n**Status**: {status_message}\n**Action**: Please dispatch personnel to clear track obstacles."

    try:
        if image_bytes:
            files = {
                "file": ("evidence.jpg", image_bytes, "image/jpeg")
            }
            payload = {
                "content": alert_text
            }
            response = requests.post(DISCORD_WEBHOOK_URL, data=payload, files=files, timeout=15)
        else:
            payload = {
                "content": alert_text
            }
            response = requests.post(DISCORD_WEBHOOK_URL, json=payload, timeout=10)
            
        if response.status_code in [200, 204]:
            print("Discord alert sent successfully.")
        else:
            print(f"Failed to send alert. HTTP Status: {response.status_code}")
            
    except Exception as e:
        print(f"Discord Webhook error: {e}")

def get_image_from_esp32(url):
    """Fetch current frame from ESP32-CAM"""
    try:
        print(f"[{datetime.datetime.now().strftime('%H:%M:%S')}] Fetching image from robot...")
        response = requests.get(url, timeout=10)
        response.raise_for_status()
        return response.content
    except requests.RequestException as e:
        print(f"Failed to fetch image: {e}")
        return None

def encode_image_to_base64(image_bytes):
    """Convert image bytes to Base64"""
    return base64.b64encode(image_bytes).decode('utf-8')

def analyze_environment(base64_image):
    """Analyze image via DeepSeek vision model"""
    print("Requesting vision analysis...")
    
    try:
        response = client.chat.completions.create(
            model=MODEL_NAME,
            messages=[
                {
                    "role": "system",
                    "content": "You are a track inspection monitor. Analyze the image for cleanliness. If there are visible papers, leaves, stains, trash, or obstacles blocking the track, reply only with 'DIRTY'. If the track is clean and clear, reply only with 'CLEAN'. Do not provide explanations or punctuation."
                },
                {
                    "role": "user",
                    "content": [
                        {
                            "type": "text",
                            "text": "Analyze the status of this image."
                        },
                        {
                            "type": "image_url",
                            "image_url": {
                                "url": f"data:image/jpeg;base64,{base64_image}",
                                "detail": "low"
                            }
                        }
                    ]
                }
            ],
            stream=False,
            temperature=0.0
        )
        
        return response.choices[0].message.content.strip().upper()
        
    except Exception as e:
        print(f"Vision API error: {e}")
        return "ERROR"

def run_inspection():
    """Execute single inspection pipeline with cooldown logic"""
    global last_alert_time

    if not ESP32_CAPTURE_URL:
        print("Error: ESP32_CAPTURE_URL not configured")
        return

    image_data = get_image_from_esp32(ESP32_CAPTURE_URL)

    if image_data:
        base64_img = encode_image_to_base64(image_data)
        status = analyze_environment(base64_img)
        
        print(f"Detection result: {status}")
        
        if "DIRTY" in status:
            current_time = time.time()
            if current_time - last_alert_time >= ALERT_COOLDOWN:
                print("Environment dirty, triggering alert...")
                send_discord_alert("Trash detected", image_bytes=image_data)
                last_alert_time = current_time
            else:
                remaining = int(ALERT_COOLDOWN - (current_time - last_alert_time))
                print(f"Environment dirty, but alert is on cooldown. ({remaining}s remaining)")
        elif "CLEAN" in status:
            print("Environment clean, no action required.")
        else:
            print(f"Unexpected response format: {status}")

if __name__ == "__main__":
    print(f"Starting continuous inspection loop. Polling every {POLL_INTERVAL}s...")
    try:
        while True:
            run_inspection()
            time.sleep(POLL_INTERVAL)
    except KeyboardInterrupt:
        print("\nInspection loop terminated by user.")
