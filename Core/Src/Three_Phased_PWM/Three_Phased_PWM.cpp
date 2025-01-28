#include "../../Inc/Three_Phased_PWM/Three_Phased_PWM.hpp"

Three_Phased_PWM::Three_Phased_PWM(Pin& u,Pin& u_negated,Pin& v,Pin& v_negated,Pin& w,Pin& w_negated,Pin& enable,Pin& reset,Data_struct *data):
U_Dual(u,u_negated),
V_Dual(v,v_negated),
W_Dual(w,w_negated),
Enable(enable),
Reset(reset),
data(data)
{}
void Three_Phased_PWM::start(){
    U_Dual.set_dead_time(dead_time_ns);
    V_Dual.set_dead_time(dead_time_ns);
    W_Dual.set_dead_time(dead_time_ns);
    set_frequency_u(initial_frequency);
    set_frequency_v(initial_frequency);
    set_frequency_w(initial_frequency);
    Enable.turn_on(); //works Active low
    Reset.turn_on(); // has to be active to work
    data->pwm_active = PWM_ACTIVE::NONE;
    data->actual_duty = 0;
    data->actual_frequency = 0.0;  
}
void Three_Phased_PWM::stop_all(){
    turn_off_u();
    turn_off_v();
    turn_off_w();
    Enable.turn_on();
    Reset.turn_off();
}
void Three_Phased_PWM::disable(){
    Enable.turn_on();
    data->buffer_enable = BUFFER_ENABLE::OFF;
}
void Three_Phased_PWM::enable(){
    Enable.turn_off();
    data->buffer_enable = BUFFER_ENABLE::ON;

}
void Three_Phased_PWM::set_frequency_u(uint32_t frequency){
    U_Dual.set_frequency(frequency);
}
void Three_Phased_PWM::set_frequency_v(uint32_t frequency){
    V_Dual.set_frequency(frequency);
}
void Three_Phased_PWM::set_frequency_w(uint32_t frequency){
    W_Dual.set_frequency(frequency);
}
void Three_Phased_PWM::set_duty_u(float duty_cycle){
    if(duty_cycle < 0.0) duty_cycle = 0.0;
    else if(duty_cycle > 100.0) duty_cycle = 100.0;

    U_Dual.set_duty_cycle(duty_cycle);
}
void Three_Phased_PWM::set_duty_v(float duty_cycle){
    if(duty_cycle < 0.0) duty_cycle = 0.0;
    else if(duty_cycle > 100.0) duty_cycle = 100.0;
    
    V_Dual.set_duty_cycle(duty_cycle);
}
void Three_Phased_PWM::set_duty_w(float duty_cycle){
    if(duty_cycle < 0.0) duty_cycle = 0.0;
    else if(duty_cycle > 100.0) duty_cycle = 100.0;
    
    W_Dual.set_duty_cycle(duty_cycle);
}
float Three_Phased_PWM::Three_Phased_PWM::get_duty_u(){
    return U_Dual.get_duty_cycle();
}
float Three_Phased_PWM::get_duty_v(){
    return V_Dual.get_duty_cycle();
}
float Three_Phased_PWM::get_duty_w(){
    return W_Dual.get_duty_cycle();
}
uint32_t Three_Phased_PWM::get_frequency_u(){
    return U_Dual.get_frequency();
}
uint32_t Three_Phased_PWM::get_frequency_v(){
    return V_Dual.get_frequency();
}
uint32_t Three_Phased_PWM::get_frequency_w(){
    return W_Dual.get_frequency();
}

void Three_Phased_PWM::turn_on_u(){
    if(data->pwm_active == PWM_ACTIVE::U) return;

    turn_off_other_pwm();
    U_Dual.turn_on();
    data->actual_duty = get_duty_u();
    data->actual_frequency = get_frequency_u();
    data->pwm_active = PWM_ACTIVE::U;
}
void Three_Phased_PWM::turn_on_w(){
    if(data->pwm_active == PWM_ACTIVE::W) return;

    turn_off_other_pwm();
    W_Dual.turn_on();
    data->actual_duty = get_duty_w();
    data->actual_frequency = get_frequency_w();
    data->pwm_active = PWM_ACTIVE::W;
}
void Three_Phased_PWM::turn_on_v(){
    if(data->pwm_active == PWM_ACTIVE::V) return;

    turn_off_other_pwm();
    V_Dual.turn_on();
    data->actual_duty = get_duty_v();
    data->actual_frequency = get_frequency_v();
    data->pwm_active = PWM_ACTIVE::V;
}
void Three_Phased_PWM::turn_off_u(){
    U_Dual.turn_off();
    data->actual_duty = 0.0;
    data->actual_frequency = 0;
    data->pwm_active = PWM_ACTIVE::NONE;
}
void Three_Phased_PWM::turn_off_v(){
    V_Dual.turn_off();
    data->actual_duty = 0.0;
    data->actual_frequency = 0;
    data->pwm_active = PWM_ACTIVE::NONE;
}
void Three_Phased_PWM::turn_off_w(){
    W_Dual.turn_off();
    data->actual_duty = 0.0;
    data->actual_frequency = 0;
    data->pwm_active = PWM_ACTIVE::NONE;
}
void Three_Phased_PWM::turn_off_other_pwm(){
    switch (data->pwm_active)
    {
    case PWM_ACTIVE::NONE:
        return;
    case PWM_ACTIVE::U:
        U_Dual.turn_off();
        return;
    case PWM_ACTIVE::V:
        V_Dual.turn_off();
        return;
    case PWM_ACTIVE::W:
        W_Dual.turn_off();
        return;
    }
}
void Three_Phased_PWM::reset(){
    Reset.turn_off();
}