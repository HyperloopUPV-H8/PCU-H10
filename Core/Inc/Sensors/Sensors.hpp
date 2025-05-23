#pragma once
#include "Data/Data.hpp"
#include "Sensors/CurrentSensors.hpp"

using SPeetecSensor = EncoderSensor<Sensors_data::encoder_samples>;
class Sensors{
    private: 
        MovingAverage<50> filter_voltage_A;
        MovingAverage<50> filter_voltage_B;
        SPeetecSensor speetec;
        Data_struct *data;
        DigitalSensor reed1{Pinout::REED_1,&data->reed1};  
        DigitalSensor reed2{Pinout::REED_2,&data->reed2};
        DigitalSensor reed3{Pinout::REED_3,&data->reed3};
        DigitalSensor reed4{Pinout::REED_4,&data->reed4};
        FilteredLinearSensor<float,50> sensor_voltage_A{Pinout::Batt_Voltage_A,Sensors_data::slope_voltage_sensor,Sensors_data::offset_voltage_sensor,&data->actual_voltage_battery_A,filter_voltage_A};
        FilteredLinearSensor<float,50> sensor_voltage_B{Pinout::Batt_Voltage_B,Sensors_data::slope_voltage_sensor,Sensors_data::offset_voltage_sensor,&data->actual_voltage_battery_B,filter_voltage_B};
        
    public:
    Sensors(Data_struct *data);
    CurrentSensors currentSensors;
    void read();
    void start();
    void read_speetec();
    void read_reeds();
     
};