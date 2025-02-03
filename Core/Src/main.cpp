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
    Three_Phased_PWM three_phased_pwm(Pinout::U_PWM,Pinout::U_PWM_NEGATED,Pinout::V_PWM,Pinout::V_PWM_NEGATED,Pinout::W_PWM,Pinout::W_PWM_NEGATED,Pinout::ENABLE_BUFFER,Pinout::Reset,Pinout::Batt_Voltage_A,Pinout::Batt_Voltage_B,Pinout::LED_COMMUTION,&Data);
    SpaceVector spaceVec(&three_phased_pwm);
    StateMachinePCU stateMachinePCU(&Data,&three_phased_pwm,&spaceVec);
    STLIB::start("192.168.0.5");
    Communication comms(&Data);
    stateMachinePCU.start(&comms);


    while (1) {
        STLIB::update();
        stateMachinePCU.update();
        stateMachinePCU.check
    }
}

void Error_Handler(void) {
    ErrorHandler("HAL error handler triggered");
    while (1) {
    }
}
