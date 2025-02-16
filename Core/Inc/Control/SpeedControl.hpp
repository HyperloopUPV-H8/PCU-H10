#pragma once
#include "Data/Data.hpp"
#include "SpaceVector.hpp"
#include "CurrentControl.hpp"
class SpeedControl{
private:
    float current_speed{};
    PI<IntegratorType::Trapezoidal> speed_PI{Current_Control_Data::kp,Current_Control_Data::ki,Current_Control_Data::period};
    Data_struct *Data;
    CurrentControl *currentControl;
    bool enable_speed_control = true;
public:
    float current_ref{};
    SpeedControl(Data_struct *Data,CurrentControl *currentControl);
    void set_current_speed(float speed_ref);
    double calculate_frequency();
    float get_current_speed();
    void control_action();
    void enable();
    void disable();
};