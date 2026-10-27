// src/motor.h
#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>

#define MOTOR_DIR1 13
#define MOTOR_DIR2 14
#define MOTOR_PWM 12

#define LIMIT_PIN 15

const int freq = 1000;
const int pwmChannel = 4;
const int pwmResolution = 8;

extern volatile bool emergency_stop;

void motor_init();
void car_forward(int speed);
void car_backward(int speed);
void car_stop();

#endif
