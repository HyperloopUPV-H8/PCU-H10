#pragma once

#include "Data/Data.hpp"
class Reeds{
    private:
        DigitalInput reed1{Pinout::REED_1};  
        DigitalInput reed2{Pinout::REED_2};
        DigitalInput reed3{Pinout::REED_3};
        DigitalInput reed4{Pinout::REED_4};
    public:
        Reeds();
}   ;