#ifndef SIM_ON
#include "main.h"
#include "lwip.h"
#endif

#include "ST-LIB.hpp"
#include "Communication/Communication.hpp"
#include "Data/Data.hpp"
#include "Actuators/Actuators.hpp"
#include "StateMachinePCU/StateMachinePCU.hpp"

int main(void) {
#ifdef SIM_ON
    SharedMemory::start();
#endif
    Data_struct Data;
    Sensors sensors(&Data);
    Actuators actuators(Pinout::U_PWM,Pinout::U_PWM_NEGATED,Pinout::V_PWM,Pinout::V_PWM_NEGATED,Pinout::W_PWM,Pinout::W_PWM_NEGATED,Pinout::ENABLE_BUFFER,&Data);
    SpaceVector spaceVec(&actuators, &Data);
    CurrentControl currentControl(&Data,&spaceVec);
    SpeedControl speedControl(&Data,&currentControl,&spaceVec);
    StateMachinePCU stateMachinePCU(&Data,&actuators,&sensors,&spaceVec,&currentControl,&speedControl);
    STLIB::start("192.168.1.5");
    sensors.currentSensors.zeroing();
    Communication comms(&Data);
    stateMachinePCU.start(&comms);


    while (1) {
        STLIB::update();
        stateMachinePCU.update();
        ProtectionManager::check_protections();
    }
}

void Error_Handler(void) {
    ErrorHandler("HAL error handler triggered");
    while (1) {
    }
}
