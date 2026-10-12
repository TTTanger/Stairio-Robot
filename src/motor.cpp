// src/motor.cpp
#include "motor.h"

// 必须在全局声明一次，对应 motor.h 中的 volatile bool
volatile bool emergency_stop = false;

// ----------------------------------------------------
// 行程开关硬件中断服务函数 (ISR)
// 注意：必须使用 IRAM_ATTR 放入内存以保证响应速度
// ----------------------------------------------------
void IRAM_ATTR onLimitSwitchTriggered() {
    // 瞬间将 PWM 占空比写为 0，切断动力
    ledcWrite(pwmChannel, 0);
    emergency_stop = true;
}

// 初始化电机引脚与传感器引脚
void motor_init() {
    // 1. 初始化电机控制引脚
    pinMode(MOTOR_DIR1, OUTPUT);
    pinMode(MOTOR_DIR2, OUTPUT);
  
    // 2. 初始化 PWM 通道
    ledcSetup(pwmChannel, freq, pwmResolution);
    ledcAttachPin(MOTOR_PWM, pwmChannel);

    // 3. 初始化传感器引脚
    // 行程开关使用内部上拉，常态为 HIGH
    pinMode(LIMIT_PIN, INPUT_PULLUP);
    
    // 4. 绑定物理防撞中断 (当电平从 HIGH 变为 LOW 时触发 FALLING)
    attachInterrupt(digitalPinToInterrupt(LIMIT_PIN), onLimitSwitchTriggered, FALLING);

    // 初始状态置为停止
    car_stop();
}

void car_forward(int speed) {
    // 如果已经触发急停，直接阻断前进指令
    if (emergency_stop) {
        return; 
    }
    digitalWrite(MOTOR_DIR1, HIGH);
    digitalWrite(MOTOR_DIR2, LOW);
    ledcWrite(pwmChannel, speed);
}

void car_backward(int speed) {
    // 只有执行后退动作，才能解除急停锁定状态
    emergency_stop = false;
    digitalWrite(MOTOR_DIR1, LOW);
    digitalWrite(MOTOR_DIR2, HIGH);
    ledcWrite(pwmChannel, speed);
}

void car_stop() {
    digitalWrite(MOTOR_DIR1, LOW);
    digitalWrite(MOTOR_DIR2, LOW);
    ledcWrite(pwmChannel, 0);
}