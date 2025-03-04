#include "Control/SpeedControl.hpp"
#define CURRENT_LIMIT 100
#define REGENERATIVE_SPEED_REF 0.0
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
    if(!currentControl->is_running())  return;
    spaceVector->set_frequency_Modulation(calculate_frequency_modulation());
    double speed_error = reference_speed - Data->speed;
    Data->speed_error = speed_error;
    float actual_current_ref;
    if(controlState == ControlStates::accelerate){
        speed_PI.input(speed_error);
        speed_PI.execute(); 
        actual_current_ref = speed_PI.output_value;
    }else{
        regenerate_PI.input(speed_error);
        regenerate_PI.execute();
        actual_current_ref = regenerate_PI.output_value;
    }
    actual_current_ref = (actual_current_ref > CURRENT_LIMIT || actual_current_ref < -CURRENT_LIMIT) ? CURRENT_LIMIT : actual_current_ref;
    Data->actual_current_ref = actual_current_ref;
    currentControl->set_current_ref(actual_current_ref);
    //if we are in regenerate and we arrive to the max speed we change the reference speed to zero
    if(controlState == ControlStates::regenerate && reference_speed > 0.1 &&  Data->speed > reference_speed){
        set_reference_speed(0.0);
    }
}
ControlStates SpeedControl::get_controlState(){
    return controlState;
}
void SpeedControl::change_mode(ControlStates state){
    controlState = state;
}