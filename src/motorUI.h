// src/motor_ui.h
#ifndef MOTOR_UI_H
#define MOTOR_UI_H

static const char motor_html[] = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>Stairio 控制台</title>
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
    <h2>Stairio 底盘测试</h2>
    
    <!-- 动态状态条 -->
    <div id="status_banner">状态: 正常 (Normal)</div>
    
    <div class="slider-container">
        <label for="speed">PWM 占空比: <span id="speed_val">150</span></label><br><br>
        <input type="range" id="speed" min="0" max="255" value="150" oninput="document.getElementById('speed_val').innerText=this.value">
    </div>

    <div class="btn-group">
        <button class="btn btn-fw" onclick="sendCmd('forward')">前 进</button>
        <button class="btn btn-stop" onclick="sendCmd('stop')">停 止</button>
        <button class="btn btn-bw" onclick="sendCmd('backward')">后 退</button>
        <button class="btn btn-clear" onclick="sendCmd('clear_estop')">解除急停</button>
    </div>

    <script>
        // 发送控制指令
        function sendCmd(action) {
            var speed = document.getElementById('speed').value;
            fetch('/motor?action=' + action + '&speed=' + speed);
            // 发送指令后立即强制拉取一次最新状态，提高UI响应速度
            setTimeout(fetchStatus, 100);
        }

        // 获取并更新急停状态
        function fetchStatus() {
            fetch('/motor?action=status')
                .then(response => response.json())
                .then(data => {
                    var banner = document.getElementById('status_banner');
                    if (data.emergency_stop) {
                        banner.style.backgroundColor = "#E53935"; // 红色预警
                        banner.innerText = "状态: 急停触发! (STOP)";
                    } else {
                        banner.style.backgroundColor = "#4CAF50"; // 绿色正常
                        banner.innerText = "状态: 正常 (Normal)";
                    }
                })
                .catch(err => console.log("Status fetch error"));
        }

        // 每 1000 毫秒 (1秒) 后台静默轮询一次状态
        setInterval(fetchStatus, 1000);
    </script>
</body>
</html>
)rawliteral";

#endif // MOTOR_UI_H