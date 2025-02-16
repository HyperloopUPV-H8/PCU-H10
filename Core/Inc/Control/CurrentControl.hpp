#pragma once
#include "Data/Data.hpp"
#include "SpaceVector.hpp"
class CurrentControl{
private:
    float *current_ref;
    PI<IntegratorType::Trapezoidal> current_PI{Current_Control_Data::kp,Current_Control_Data::ki,Current_Control_Data::period};
    Data_struct *Data;
    SpaceVector *spaceVector;

public:
    CurrentControl(Data_struct *Data,SpaceVector *spaceVector);
    void set_current_ref(float& cur_ref);
    double calculate_peak();
    float get_current_ref();
    void control_action();
};