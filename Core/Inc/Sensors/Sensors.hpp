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
        DigitalSensor reed2{Pinout::REED_2,&data->reed3};
        FilteredLinearSensor<float,50> sensor_voltage_A{Pinout::Batt_Voltage_A,Sensors_data::slope_voltage_sensor,Sensors_data::offset_voltage_sensor,&data->actual_voltage_battery_A,filter_voltage_A};
        FilteredLinearSensor<float,50> sensor_voltage_B{Pinout::Batt_Voltage_B,Sensors_data::slope_voltage_sensor,Sensors_data::offset_voltage_sensor,&data->actual_voltage_battery_B,filter_voltage_B};
        DigitalSensor fault_gd_inverter_a{Pinout::FAULT_GD_INVERTER_A, &data->fault_gd_inverter_a};
        DigitalSensor fault_gd_inverter_b{Pinout::FAULT_GD_INVERTER_B, &data->fault_gd_inverter_b};
        DigitalSensor ready_gd_inverter_a{Pinout::READY_GD_INVERTER_A, &data->ready_gd_inverter_a};
        DigitalSensor ready_gd_inverter_b{Pinout::READY_GD_INVERTER_B, &data->ready_gd_inverter_b};
        uint64_t emulation_start{};
    public:
    Sensors(Data_struct *data);
    CurrentSensors currentSensors;
    void read();
    void start();
    void start_emulated_speetec();
    void read_speetec();
    void read_reeds();
    bool reeds_braking();
    bool check_gate_drivers()const;

    private:
    void emulated_read();
    double get_emulation_time();
     
};