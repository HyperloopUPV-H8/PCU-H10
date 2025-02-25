#ifndef SIM_ON
#include "main.h"
#include "lwip.h"
#endif

#include "ST-LIB.hpp"
#include "Communication/Communication.hpp"
#include "Data/Data.hpp"
#include "Three_Phased_PWM/Three_Phased_PWM.hpp"
#include "StateMachinePCU/StateMachinePCU.hpp"

int main(void) {
#ifdef SIM_ON
    SharedMemory::start();
#endif
    Data_struct Data;
    Sensors sensors(&Data);
    Three_Phased_PWM three_phased_pwm(Pinout::U_PWM,Pinout::U_PWM_NEGATED,Pinout::V_PWM,Pinout::V_PWM_NEGATED,Pinout::W_PWM,Pinout::W_PWM_NEGATED,Pinout::ENABLE_BUFFER,Pinout::Reset,&Data);
    SpaceVector spaceVec(&three_phased_pwm, &Data);
    CurrentControl currentControl(&Data,&spaceVec);
    SpeedControl speedControl(&Data,&currentControl,&spaceVec);
    StateMachinePCU stateMachinePCU(&Data,&three_phased_pwm,&sensors,&spaceVec,&currentControl,&speedControl);
    STLIB::start("192.168.0.5");
    sensors.currentSensors.zeroing();
    Communication comms(&Data);
    stateMachinePCU.start(&comms);


    while (1) {
        STLIB::update();
        stateMachinePCU.update();
    }
}

void Error_Handler(void) {
    ErrorHandler("HAL error handler triggered");
    while (1) {
    }
}
