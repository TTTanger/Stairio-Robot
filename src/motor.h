// src/motor.h
#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>

// 电机引脚宏定义
#define MOTOR_DIR1 13
#define MOTOR_DIR2 14
#define MOTOR_PWM 12

// 限位开关/红外传感器引脚宏定义
#define LIMIT_PIN 15

// 定义 ESP32 PWM 硬件参数
const int freq = 1000;         // 1kHz 频率，适合直流减速电机
const int pwmChannel = 4;     // 避开摄像头和闪光灯冲突   
const int pwmResolution = 8;   // 8位分辨率 (占空比0-255)

// 中断标志位 (使用 extern 声明，表示该变量在别处定义)
extern volatile bool emergency_stop;

// 函数声明
void motor_init();
void car_forward(int speed);
void car_backward(int speed);
void car_stop();

#endif // MOTOR_H