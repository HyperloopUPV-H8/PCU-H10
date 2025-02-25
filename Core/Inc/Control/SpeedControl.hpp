#pragma once
#include "Data/Data.hpp"
#include "SpaceVector.hpp"
#include "CurrentControl.hpp"
class SpeedControl{
private:
    float a = 1.3;
    float b = 8.4;
    float reference_speed{};
    PI<IntegratorType::Trapezoidal> speed_PI{Speed_Control_Data::kp,Speed_Control_Data::ki,Speed_Control_Data::period};
    Data_struct *Data;
    CurrentControl *currentControl;
    SpaceVector *spaceVector;
    double calculate_frequency_modulation();
public:
    float current_ref{};
    SpeedControl(Data_struct *Data,CurrentControl *currentControl,SpaceVector *spaceVector);
    void set_reference_speed(float speed_ref);
    float get_reference_speed();
    void control_action();
};