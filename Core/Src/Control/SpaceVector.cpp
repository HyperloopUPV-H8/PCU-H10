#include "Control/SpaceVector.hpp"
SpaceVector::SpaceVector(Three_Phased_PWM *three_pwm_class): three_pwm(three_pwm_class){}

void SpaceVector::set_target_voltage(float V_ref){
    modulation = V_ref/VMAX * IMAX;
}

void SpaceVector::set_frequency(uint32_t freq){
    frequency = freq;
}

void SpaceVector::calculate_duties(){
    static uint64_t time = 0;
    float sin_u = modulation* sin(2* M_PI * frequency * time);
    float sin_v = modulation * sin(2*M_PI * frequency * time + 2*M_PI/3);
    float sin_w = modulation * sin(2*M_PI * frequency * time  - 2*M_PI/3);
    
    float offset = std::max({sin_u,sin_v,sin_w}) + std::min({sin_u,sin_v,sin_w})/2;

    sin_u -= offset;
    sin_v -= offset;
    sin_w -= offset;

    three_pwm->set_duty_u((sin_u/2.0 + 0.5) * 100.0);
    three_pwm->set_duty_v((sin_u/2.0 + 0.5) * 100.0);
    three_pwm->set_duty_w((sin_u/2.0 + 0.5) * 100.0);
}