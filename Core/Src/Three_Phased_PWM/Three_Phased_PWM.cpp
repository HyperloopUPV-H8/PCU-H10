#include "../../Inc/Three_Phased_PWM/Three_Phased_PWM.hpp"
using namespace std::chrono_literals;

Three_Phased_PWM::Three_Phased_PWM(Pin& u,Pin& u_negated,Pin& v,Pin& v_negated,Pin& w,Pin& w_negated,Pin& enable,Pin& reset,Pin& batt_con_A,Pin& batt_con_B,Pin& Led_Commutat,Data_struct *data):
U_Dual(u,u_negated),
V_Dual(v,v_negated),
W_Dual(w,w_negated),
Enable(enable),
Reset(reset),
data(data),
Led_Commutation(Led_Commutat)
{ 
    battery_connector_A_id =ADC::inscribe(batt_con_A);
    battery_connector_B_id = ADC::inscribe(batt_con_B);
    
}
void Three_Phased_PWM::start(){
    U_Dual.set_frequency(initial_frequency);
    V_Dual.set_frequency(initial_frequency);
    W_Dual.set_frequency(initial_frequency);
    U_Dual.set_dead_time(500ns);
    V_Dual.set_dead_time(500ns);
    W_Dual.set_dead_time(500ns);
    U_Dual.set_duty_cycle(0.0);
    V_Dual.set_duty_cycle(0.0);
    W_Dual.set_duty_cycle(0.0);
    U_Dual.turn_on();
    V_Dual.turn_on();
    W_Dual.turn_on();

    Enable.turn_on(); //works Active low
    Enable_reset(); // has to be active to work

    //start adc
    ADC::start();
    ADC::turn_on(battery_connector_A_id);
    ADC::turn_on(battery_connector_B_id);
    data->pwm_active = PWM_ACTIVE::NONE;
    data->actual_duty = 0;
    data->actual_frequency = 0.0;
}
void Three_Phased_PWM::stop_all(){
    U_Dual.set_duty_cycle(0.0);
    V_Dual.set_duty_cycle(0.0);
    W_Dual.set_duty_cycle(0.0);
    disable();
    Disable_reset();
    
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
    data->actual_frequency = get_frequency_u();
}
void Three_Phased_PWM::set_frequency_v(uint32_t frequency){
    V_Dual.set_frequency(frequency);
    data->actual_frequency = get_frequency_v();
}
void Three_Phased_PWM::set_frequency_w(uint32_t frequency){
    W_Dual.set_frequency(frequency);
    data->actual_frequency = get_frequency_w();
}
void Three_Phased_PWM::set_duty_u(float duty_cycle){
    if(duty_cycle < 0.0) duty_cycle = 0.0;
    else if(duty_cycle > 100.0) duty_cycle = 100.0;

    U_Dual.set_duty_cycle(duty_cycle);
    data->actual_duty = get_duty_u();
}
void Three_Phased_PWM::set_duty_v(float duty_cycle){
    if(duty_cycle < 0.0) duty_cycle = 0.0;
    else if(duty_cycle > 100.0) duty_cycle = 100.0;
    
    V_Dual.set_duty_cycle(duty_cycle);
    data->actual_duty = get_duty_v();
}
void Three_Phased_PWM::set_duty_w(float duty_cycle){
    if(duty_cycle < 0.0) duty_cycle = 0.0;
    else if(duty_cycle > 100.0) duty_cycle = 100.0;
    
    W_Dual.set_duty_cycle(duty_cycle);
    data->actual_duty = get_duty_w();
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
/**/
void Three_Phased_PWM::turn_on_u(){
    data->actual_duty = get_duty_u();
    data->actual_frequency = get_frequency_u();
    data->pwm_active = PWM_ACTIVE::U;
}
void Three_Phased_PWM::turn_on_w(){
    data->actual_duty = get_duty_w();
    data->actual_frequency = get_frequency_w();
    data->pwm_active = PWM_ACTIVE::W;
}
void Three_Phased_PWM::turn_on_v(){
    data->actual_duty = get_duty_v();
    data->actual_frequency = get_frequency_v();
    data->pwm_active = PWM_ACTIVE::V;
}
void Three_Phased_PWM::turn_off_u(){
    U_Dual.set_duty_cycle(0.0); 
    data->actual_duty = 0.0;
    data->actual_frequency = 0;
    data->pwm_active = PWM_ACTIVE::NONE;
}
void Three_Phased_PWM::turn_off_v(){
    V_Dual.set_duty_cycle(0.0);
    data->actual_duty = 0.0;
    data->actual_frequency = 0;
    data->pwm_active = PWM_ACTIVE::NONE;
}
void Three_Phased_PWM::turn_off_w(){
    W_Dual.set_duty_cycle(0.0);
    data->actual_duty = 0.0;
    data->actual_frequency = 0;
    data->pwm_active = PWM_ACTIVE::NONE;
}
void Three_Phased_PWM::turn_off_active_pwm(){
    switch(data->pwm_active){
        case PWM_ACTIVE::NONE :
            return;
        case PWM_ACTIVE::U : 
            turn_off_u();
            return;
        case PWM_ACTIVE::V :
            turn_off_v();
            return;
        case PWM_ACTIVE::W :
            turn_off_w();
    }
}
void Three_Phased_PWM::Disable_reset(){
    Reset.turn_off();
}
void Three_Phased_PWM::Enable_reset(){
    Reset.turn_on();
}
void Three_Phased_PWM::read_ADC(){
    switch (data->connector_Batteries)
    {
    case Battery_Connector::A :
        data->actual_voltage_batteries = ADC::get_value(battery_connector_A_id);
        break;
    case Battery_Connector::B:
        data->actual_voltage_batteries = ADC::get_value(battery_connector_B_id);
    }
}
