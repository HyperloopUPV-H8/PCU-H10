#pragma once
#include "Data/Data.hpp"
#include "Sensors/CurrentSensors.hpp"

using SPeetecSensor = EncoderSensor<Sensors_data::encoder_samples>;
class Sensors{
    private: 
        MovingAverage<100> filter_voltage_A;
        MovingAverage<100> filter_voltage_B;
        SPeetecSensor speetec;
        Data_struct *data;
        FilteredLinearSensor<float,100> sensor_voltage_A{Pinout::Batt_Voltage_A,Sensors_data::slope_voltage_sensor,Sensors_data::offset_voltage_sensor,&data->actual_voltage_battery_A,filter_voltage_A};
        FilteredLinearSensor<float,100> sensor_voltage_B{Pinout::Batt_Voltage_B,Sensors_data::slope_voltage_sensor,Sensors_data::offset_voltage_sensor,&data->actual_voltage_battery_B,filter_voltage_B};
        
    public:
    Sensors(Data_struct *data);
    CurrentSensors currentSensors;
    void read();
    void start();
    void read_speetec();
     
};