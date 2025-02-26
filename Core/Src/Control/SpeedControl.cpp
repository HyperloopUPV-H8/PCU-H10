#include "Control/SpeedControl.hpp"
#define CURRENT_LIMIT 100

SpeedControl::SpeedControl(Data_struct *Data,CurrentControl *currentControl,SpaceVector *spaceVector):
    Data(Data),currentControl(currentControl),spaceVector(spaceVector){
        speed_PI.reset();
    }

void SpeedControl::set_reference_speed(float speed_ref){
    reference_speed = speed_ref;
}
float SpeedControl::get_reference_speed(){
    return reference_speed;
}
double SpeedControl::calculate_frequency_modulation(){
    return a*Data->speed + b;
}
void SpeedControl::control_action(){
    spaceVector->set_frequency_Modulation(calculate_frequency_modulation());
    double speed_error = reference_speed - Data->speed;
    Data->speed_error = speed_error;
    speed_PI.input(speed_error);
    speed_PI.execute();
    float actual_current_ref = (speed_PI.output_value > CURRENT_LIMIT || speed_PI.output_value < -CURRENT_LIMIT) ? CURRENT_LIMIT : speed_PI.output_value;
    Data->actual_current_ref = actual_current_ref;
    currentControl->set_current_ref(actual_current_ref);
}
