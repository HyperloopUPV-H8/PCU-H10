#pragma once
#include "Data/Data.hpp"


class CurrentSensors{
    private: 
  
        Data_struct *data;
        LinearSensor<float> sensor_u_a{Pinout::CURRENT_SENSOR_U_A,Sensors_data::slope_current_sensor,Sensors_data::offset_current_sensor,&data->actual_current_sensor_u_a};
        LinearSensor<float> sensor_u_b{Pinout::CURRENT_SENSOR_U_B,Sensors_data::slope_current_sensor,Sensors_data::offset_current_sensor,&data->actual_current_sensor_u_b};
        LinearSensor<float> sensor_v_a{Pinout::CURRENT_SENSOR_V_A,Sensors_data::slope_current_sensor_inverted,Sensors_data::offset_current_sensor_inverted,&data->actual_current_sensor_v_a};
        LinearSensor<float> sensor_v_b{Pinout::CURRENT_SENSOR_V_B,Sensors_data::slope_current_sensor_inverted,Sensors_data::offset_current_sensor_inverted,&data->actual_current_sensor_v_b};
        LinearSensor<float> sensor_w_a{Pinout::CURRENT_SENSOR_W_A,Sensors_data::slope_current_sensor,Sensors_data::offset_current_sensor,&data->actual_current_sensor_w_a};
        LinearSensor<float> sensor_w_b{Pinout::CURRENT_SENSOR_W_B,Sensors_data::slope_current_sensor,Sensors_data::offset_current_sensor,&data->actual_current_sensor_w_b};
    public:
    CurrentSensors(Data_struct *data);
    void read();
    void zeroing();

};