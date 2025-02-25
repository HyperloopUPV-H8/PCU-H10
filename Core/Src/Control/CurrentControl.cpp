#include "Control/CurrentControl.hpp"

double Max_Peak::modulation_frequency = 0.0;

CurrentControl::CurrentControl(Data_struct *Data,SpaceVector *spaceVector):
Data(Data),
spaceVector(spaceVector)
{
    current_PI.reset();
    current_ref = 0.0; 
}
void CurrentControl::set_current_ref(float cur_ref){
    current_ref = cur_ref;
}

float CurrentControl::get_current_ref(){
    return current_ref;
}
double CurrentControl::calculate_peak(){
    Max_Peak::set_modulation_freq(spaceVector->get_modulation_frequency());
    #if PPU_USING != 1
        double Peak_u_a = current_u_a.calculate_Max_Peak();
        double Peak_v_a = current_v_a.calculate_Max_Peak();
        double Peak_w_a = current_w_a.calculate_Max_Peak();
        double average_a = (Peak_u_a + Peak_v_a + Peak_w_a)/3.0;
    #endif
    #if PPU_USING != 0
        double Peak_u_b = current_u_b.calculate_Max_Peak();
        double Peak_v_b = current_v_b.calculate_Max_Peak();
        double Peak_w_b = current_W_b.calculate_Max_Peak();
        double average_b = (Peak_u_b + Peak_v_b + Peak_w_b)/3.0;
    #endif
    #if PPU_USING == 0
        return average_a;
    #elif PPU_USING == 1
        return average_b;
    #elif PPU_USING == 2
        return (average_a + average_b)/2.0;
    #endif
}
void CurrentControl::control_action(){
    if (!should_be_running) return;

    double current_peak = calculate_peak();
    double current_error = current_ref - current_peak;
    Data->current_Peak = current_peak;
    Data->current_error = current_error;
    current_PI.input(current_error);
    current_PI.execute();
    Data->target_voltage = current_PI.output_value;
    
    if(current_PI.output_value > spaceVector->VMAX){
        Data->target_voltage = spaceVector->VMAX;
    }
    spaceVector->set_target_voltage(Data->target_voltage);
}

void CurrentControl::start() {
    should_be_running = true;
}

void CurrentControl::stop() {
    should_be_running = false;
}