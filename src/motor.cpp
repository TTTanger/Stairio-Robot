// src/motor.cpp
#include "motor.h"

volatile bool emergency_stop = false;

void IRAM_ATTR onLimitSwitchTriggered() {
    ledcWrite(pwmChannel, 0);
    emergency_stop = true;
}

void motor_init() {
    pinMode(MOTOR_DIR1, OUTPUT);
    pinMode(MOTOR_DIR2, OUTPUT);
  
    ledcSetup(pwmChannel, freq, pwmResolution);
    ledcAttachPin(MOTOR_PWM, pwmChannel);

    pinMode(LIMIT_PIN, INPUT_PULLUP);
    
    attachInterrupt(digitalPinToInterrupt(LIMIT_PIN), onLimitSwitchTriggered, FALLING);

    car_stop();
}

void car_forward(int speed) {
    if (emergency_stop) {
        return; 
    }
    digitalWrite(MOTOR_DIR1, HIGH);
    digitalWrite(MOTOR_DIR2, LOW);
    ledcWrite(pwmChannel, speed);
}

void car_backward(int speed) {
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