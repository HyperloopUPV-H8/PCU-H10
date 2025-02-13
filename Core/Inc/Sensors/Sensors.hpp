#pragma once
#include "Data/Data.hpp"
#include "Sensors/CurrentSensors.hpp"
class Sensors{
    private: 
        Data_struct *data;
    public:
    Sensors(Data_struct *data);
    CurrentSensors currentSensors;
    void read();
     
};