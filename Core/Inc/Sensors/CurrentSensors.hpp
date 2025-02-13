#pragma once
#include "Data/Data.hpp"

#define HALL_CURRENT_SENSOR_SLOPE 96.945
#define HALL_CURRENT_SENSOR_OFFSET -157.30

class CurrentSensors{
    private: 
        Data_struct *data;
        LinearSensor<float> sensor_u_a{Pinout::CURRENT_SENSOR_U_A,HALL_CURRENT_SENSOR_SLOPE,HALL_CURRENT_SENSOR_OFFSET,&data->actual_current_sensor_u_a};
        LinearSensor<float> sensor_u_b{Pinout::CURRENT_SENSOR_U_B,HALL_CURRENT_SENSOR_SLOPE,HALL_CURRENT_SENSOR_OFFSET,&data->actual_current_sensor_u_b};
        LinearSensor<float> sensor_v_a{Pinout::CURRENT_SENSOR_V_A,HALL_CURRENT_SENSOR_SLOPE,HALL_CURRENT_SENSOR_OFFSET,&data->actual_current_sensor_v_a};
        LinearSensor<float> sensor_v_b{Pinout::CURRENT_SENSOR_V_B,HALL_CURRENT_SENSOR_SLOPE,HALL_CURRENT_SENSOR_OFFSET,&data->actual_current_sensor_v_b};
        LinearSensor<float> sensor_w_a{Pinout::CURRENT_SENSOR_W_A,HALL_CURRENT_SENSOR_SLOPE,HALL_CURRENT_SENSOR_OFFSET,&data->actual_current_sensor_w_a};
        LinearSensor<float> sensor_w_b{Pinout::CURRENT_SENSOR_W_B,HALL_CURRENT_SENSOR_SLOPE,HALL_CURRENT_SENSOR_OFFSET,&data->actual_current_sensor_w_b};
    public:
    CurrentSensors(Data_struct *data);
    void read();
    void zeroing();

};