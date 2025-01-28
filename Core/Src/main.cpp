#ifndef SIM_ON
#include "main.h"
#include "lwip.h"
#endif

#include "ST-LIB.hpp"
#include "Communication/Communication.hpp"
#include "Data/Data.hpp"
#include "Three_Phased_PWM/Three_Phased_PWM.hpp"

int main(void) {
#ifdef SIM_ON
    SharedMemory::start();
#endif
    
    STLIB::start();


    while (1) {
        STLIB::update();
    }
}

void Error_Handler(void) {
    ErrorHandler("HAL error handler triggered");
    while (1) {
    }
}
