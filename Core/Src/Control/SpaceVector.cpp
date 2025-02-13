#include "Control/SpaceVector.hpp"
SpaceVector::SpaceVector(Three_Phased_PWM *three_pwm_class): three_pwm(three_pwm_class){}

void SpaceVector::set_target_voltage(float V_ref){
    Imodulation = V_ref/VMAX * IMAX;
}

void SpaceVector::set_frequency_Modulation(uint32_t freq){
    Modulation_frequency = freq;
}

void SpaceVector::calculate_duties(){
    float sin_u = Imodulation * sin(2* M_PI * Modulation_frequency * time);
    float sin_v = Imodulation * sin(2*M_PI * Modulation_frequency * time + 2*M_PI/3);
    float sin_w = Imodulation * sin(2*M_PI * Modulation_frequency * time  - 2*M_PI/3);
    
    float offset = (std::max({sin_u,sin_v,sin_w}) + std::min({sin_u,sin_v,sin_w}))/2;

    sin_u -= offset;
    sin_v -= offset;
    sin_w -= offset;

    three_pwm->set_duty_u((sin_u/2.0 + 0.5) * 100.0);
    three_pwm->set_duty_v((sin_v/2.0 + 0.5) * 100.0);
    three_pwm->set_duty_w((sin_w/2.0 + 0.5) * 100.0);
    time+=Period/1000000.0;
}

uint32_t SpaceVector::get_modulation_frequency(){
    return Modulation_frequency;
}

float SpaceVector::get_actual_time(){
    return time;
}