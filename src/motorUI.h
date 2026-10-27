#ifndef MOTOR_UI_H
#define MOTOR_UI_H

static const char motor_html[] = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>Stairio Control</title>
    <style>
        body { font-family: Arial, sans-serif; text-align: center; margin-top: 20px; background-color: #222; color: #fff; }
        #status_banner { margin: 15px auto; padding: 12px; width: 80%; max-width: 300px; font-size: 18px; font-weight: bold; border-radius: 8px; background-color: #4CAF50; transition: background-color 0.3s; }
        .slider-container { margin: 20px 0; }
        input[type=range] { width: 70%; max-width: 300px; }
        .btn-group { display: flex; flex-direction: column; align-items: center; gap: 15px; }
        .btn { width: 120px; height: 50px; font-size: 18px; border-radius: 8px; border: none; cursor: pointer; color: #fff; font-weight: bold; }
        .btn-fw { background-color: #4CAF50; }
        .btn-stop { background-color: #f44336; }
        .btn-bw { background-color: #2196F3; }
        .btn-clear { background-color: #FF9800; margin-top: 15px; }
        .btn:active { opacity: 0.7; }
    </style>
</head>
<body>
    <h2>Stairio Test</h2>
    <div id="status_banner">State: (Normal)</div>

    <div class="slider-container">
        <label for="speed">PWM duty cyle: <span id="speed_val">150</span></label><br><br>
        <input type="range" id="speed" min="0" max="255" value="150" oninput="document.getElementById('speed_val').innerText=this.value">
    </div>

    <div class="btn-group">
        <button class="btn btn-fw" onclick="sendCmd('forward')">Forward</button>
        <button class="btn btn-stop" onclick="sendCmd('stop')">Stop</button>
        <button class="btn btn-bw" onclick="sendCmd('backward')">Backward</button>
        <button class="btn btn-clear" onclick="sendCmd('clear_estop')">Disable Emergency</button>
    </div>

    <script>
        function sendCmd(action) {
            var speed = document.getElementById('speed').value;
            fetch('/motor?action=' + action + '&speed=' + speed);
            setTimeout(fetchStatus, 100);
        }

        function fetchStatus() {
            fetch('/motor?action=status')
                .then(response => response.json())
                .then(data => {
                    var banner = document.getElementById('status_banner');
                    if (data.emergency_stop) {
                        banner.style.backgroundColor = "#E53935"; 
                        banner.innerText = "State: Emergency Stop Enabled!";
                    } else {
                        banner.style.backgroundColor = "#4CAF50"; 
                        banner.innerText = "State: Normal";
                    }
                })
                .catch(err => console.log("Status fetch error"));
        }
        setInterval(fetchStatus, 1000);
    </script>
</body>
</html>
)rawliteral";

#endif