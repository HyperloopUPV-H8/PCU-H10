#pragma once
#include "Data/Data.hpp"
#include "Sensors/CurrentSensors.hpp"

using SPeetecSensor = EncoderSensor<Sensors_data::encoder_samples>;
class Sensors{
    private: 
        SPeetecSensor speetec;
        Data_struct *data;
        LinearSensor<float> sensor_voltage_A{Pinout::Batt_Voltage_A,Sensors_data::slope_voltage_sensor,Sensors_data::offset_voltage_sensor,&data->actual_voltage_battery_A};
        LinearSensor<float> sensor_voltage_B{Pinout::Batt_Voltage_B,Sensors_data::slope_voltage_sensor,Sensors_data::offset_voltage_sensor,&data->actual_voltage_battery_B};
        
    public:
    Sensors(Data_struct *data);
    CurrentSensors currentSensors;
    void read();
    void start();
    void read_speetec();
     
};