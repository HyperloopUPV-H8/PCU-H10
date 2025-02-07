#pragma once
#include "Data/Data.hpp"
class CurrentSensors{
    private: 
        Data_struct *data;
        LinearSensor<float> sensor_u_a{Pinout::CURRENT_SENSOR_U_A,Current_Sensors::slope_u_a,Current_Sensors::offset_u_a,&data->actual_current_sensor_u_a};
        LinearSensor<float> sensor_u_b{Pinout::CURRENT_SENSOR_U_B,Current_Sensors::slope_u_b,Current_Sensors::offset_u_b,&data->actual_current_sensor_u_b};
        LinearSensor<float> sensor_v_a{Pinout::CURRENT_SENSOR_V_A,Current_Sensors::slope_v_a,Current_Sensors::offset_v_a,&data->actual_current_sensor_v_a};
        LinearSensor<float> sensor_v_b{Pinout::CURRENT_SENSOR_V_B,Current_Sensors::slope_v_b,Current_Sensors::offset_v_b,&data->actual_current_sensor_v_b};
        LinearSensor<float> sensor_w_a{Pinout::CURRENT_SENSOR_W_A,Current_Sensors::slope_w_a,Current_Sensors::offset_w_a,&data->actual_current_sensor_w_a};
        LinearSensor<float> sensor_w_b{Pinout::CURRENT_SENSOR_W_B,Current_Sensors::slope_w_b,Current_Sensors::offset_w_b,&data->actual_current_sensor_w_b};
        void zeroing();
    public:
    CurrentSensors(Data_struct *data);
    void read();
};