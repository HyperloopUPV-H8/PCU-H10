#include "Control/SpeedControl.hpp"
#define CURRENT_LIMIT 100
#define REGENERATIVE_SPEED_REF 0.0
bool saturating_current = false;
static double output{0.0};
static double exp_follower(double reference, double error_factor = 0.5){
    double error{reference - output};
    output += error * error_factor;
    return output;
}
SpeedControl::SpeedControl(Data_struct *Data,CurrentControl *currentControl,SpaceVector *spaceVector):
    Data(Data),currentControl(currentControl),spaceVector(spaceVector){
        speed_PI.reset();
        regenerate_PI.reset();
    }

void SpeedControl::set_reference_speed(float speed_ref){
    reference_speed = speed_ref;
    Data->target_speed = reference_speed;
    output = 0.0;
}
float SpeedControl::get_reference_speed(){
    return reference_speed;
}
double SpeedControl::calculate_frequency_modulation(){
    return (Data->speedState == ControlStates::accelerate) ? exp_follower(a*Data->speed_km_h_encoder + b) : exp_follower(a*Data->speed_km_h_encoder + b - Data->speed_km_h_encoder/1.2);
}
void SpeedControl::control_action(){
    if(!currentControl->is_running())  return;
    spaceVector->set_frequency_Modulation(calculate_frequency_modulation());
    double speed_error = reference_speed - Data->speed_km_h_encoder;
    Data->speed_error = speed_error;
    float actual_current_ref;
    if(Data->speedState == ControlStates::accelerate){
        speed_PI.input(speed_error);
        speed_PI.execute(); 
        actual_current_ref = speed_PI.output_value;
    }else{
        regenerate_PI.input(speed_error);
        regenerate_PI.execute();
        actual_current_ref = regenerate_PI.output_value;
    }
    //actual_current_ref = (actual_current_ref > CURRENT_LIMIT || actual_current_ref < -CURRENT_LIMIT) ? CURRENT_LIMIT : actual_current_ref;
    if(actual_current_ref > CURRENT_LIMIT)
        actual_current_ref = CURRENT_LIMIT;
    if(actual_current_ref < 0){
        actual_current_ref = 0;
        saturating_current = true;
    }else{
        saturating_current = false;
    }
        

    Data->actual_current_ref = actual_current_ref;
    currentControl->set_current_ref(actual_current_ref,saturating_current);
    //if we are in regenerate and we arrive to the max speed we change the reference speed to zero
    if(Data->speedState == ControlStates::regenerate && reference_speed > 0.1 &&  Data->speed_km_h_encoder > reference_speed - 0.5){
        set_reference_speed(REGENERATIVE_SPEED_REF);
    }
}
ControlStates SpeedControl::get_controlState(){
    return Data->speedState;
}
void SpeedControl::change_mode(ControlStates state){
    Data->speedState = state;
}
void SpeedControl::reset_PI(){
    speed_PI.reset();
    regenerate_PI.reset();
    currentControl->reset_PI();
}