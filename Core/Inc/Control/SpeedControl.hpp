#pragma once
#include "Data/Data.hpp"
#include "SpaceVector.hpp"
#include "CurrentControl.hpp"
class SpeedControl{
private:
    float current_ref{};
    PI<IntegratorType::Trapezoidal> speed_PI{Current_Control_Data::kp,Current_Control_Data::ki,Current_Control_Data::period};
    Data_struct *Data;
    SpaceVector *spaceVector;

public:
    SpeedControl(Data_struct *Data,SpaceVector *spaceVector,CurrentControl *currentControl);
    void set_current_ref(float cur_ref);
    double calculate_peak();
    float get_current_ref();
    void control_action();
};