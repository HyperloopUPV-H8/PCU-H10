#pragma once
#include "Data/Data.hpp"
#include "Sensors/CurrentSensors.hpp"
class Sensors{
    private: 
        Data_struct *data;
        CurrentSensors currentSensors;
    public:
    Sensors(Data_struct *data);
    void read();
     
};