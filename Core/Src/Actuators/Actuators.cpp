#include "../../Inc/Actuators/Actuators.hpp"
using namespace std::chrono_literals;

Actuators::Actuators(Pin& u, Pin& u_negated, Pin& v, Pin& v_negated, Pin& w,
                     Pin& w_negated, Pin& enable, Pin& reset, Data_struct* data)
    : U_Dual(u, u_negated),
      V_Dual(v, v_negated),
      W_Dual(w, w_negated),
      Enable(enable),
      Reset(reset),
      data(data) {}
void Actuators::start() {
    U_Dual.set_frequency(initial_frequency);
    V_Dual.set_frequency(initial_frequency);
    W_Dual.set_frequency(initial_frequency);
    U_Dual.set_dead_time(dead_time_ns);
    V_Dual.set_dead_time(dead_time_ns);
    W_Dual.set_dead_time(dead_time_ns);
    U_Dual.set_duty_cycle(0.0);
    V_Dual.set_duty_cycle(0.0);
    W_Dual.set_duty_cycle(0.0);
    U_Dual.turn_on();
    V_Dual.turn_on();
    W_Dual.turn_on();

    Enable.turn_on();  // works Active low
    Enable_reset();    // has to be active to work

    data->pwm_active = PWM_ACTIVE::NONE;
    data->actual_duty = 0;
    data->actual_frequency = 0.0;
}
void Actuators::stop_all() {
    U_Dual.set_duty_cycle(0.0);
    V_Dual.set_duty_cycle(0.0);
    W_Dual.set_duty_cycle(0.0);
    disable();
    Disable_reset();
}
void Actuators::disable() {
    Enable.turn_on();
    data->buffer_enable = BUFFER_ENABLE::OFF;
}
void Actuators::enable() {
    Enable.turn_off();
    data->buffer_enable = BUFFER_ENABLE::ON;
}
void Actuators::set_frequency_u(uint32_t frequency) {
    U_Dual.set_frequency(frequency);
    data->actual_frequency = get_frequency_u();
}
void Actuators::set_frequency_v(uint32_t frequency) {
    V_Dual.set_frequency(frequency);
    data->actual_frequency = get_frequency_v();
}
void Actuators::set_frequency_w(uint32_t frequency) {
    W_Dual.set_frequency(frequency);
    data->actual_frequency = get_frequency_w();
}
void Actuators::set_duty_u(float duty_cycle) {
    if (duty_cycle < 0.0)
        duty_cycle = 0.0;
    else if (duty_cycle > 100.0)
        duty_cycle = 100.0;

    U_Dual.set_duty_cycle(duty_cycle);
    data->actual_duty = get_duty_u();
}
void Actuators::set_duty_v(float duty_cycle) {
    if (duty_cycle < 0.0)
        duty_cycle = 0.0;
    else if (duty_cycle > 100.0)
        duty_cycle = 100.0;

    V_Dual.set_duty_cycle(duty_cycle);
    data->actual_duty = get_duty_v();
}
void Actuators::set_duty_w(float duty_cycle) {
    if (duty_cycle < 0.0)
        duty_cycle = 0.0;
    else if (duty_cycle > 100.0)
        duty_cycle = 100.0;

    W_Dual.set_duty_cycle(duty_cycle);
    data->actual_duty = get_duty_w();
}
float Actuators::get_duty_u() { return U_Dual.get_duty_cycle(); }
float Actuators::get_duty_v() { return V_Dual.get_duty_cycle(); }
float Actuators::get_duty_w() { return W_Dual.get_duty_cycle(); }
uint32_t Actuators::get_frequency_u() { return U_Dual.get_frequency(); }
uint32_t Actuators::get_frequency_v() { return V_Dual.get_frequency(); }
uint32_t Actuators::get_frequency_w() { return W_Dual.get_frequency(); }
/**/
void Actuators::turn_on_u() {
    data->actual_duty = get_duty_u();
    data->actual_frequency = get_frequency_u();
    data->pwm_active = PWM_ACTIVE::U;
}
void Actuators::turn_on_w() {
    data->actual_duty = get_duty_w();
    data->actual_frequency = get_frequency_w();
    data->pwm_active = PWM_ACTIVE::W;
}
void Actuators::turn_on_v() {
    data->actual_duty = get_duty_v();
    data->actual_frequency = get_frequency_v();
    data->pwm_active = PWM_ACTIVE::V;
}
void Actuators::turn_off_u() {
    U_Dual.set_duty_cycle(0.0);
    data->actual_duty = 0.0;
    data->actual_frequency = 0;
    data->pwm_active = PWM_ACTIVE::NONE;
}
void Actuators::turn_off_v() {
    V_Dual.set_duty_cycle(0.0);
    data->actual_duty = 0.0;
    data->actual_frequency = 0;
    data->pwm_active = PWM_ACTIVE::NONE;
}
void Actuators::turn_off_w() {
    W_Dual.set_duty_cycle(0.0);
    data->actual_duty = 0.0;
    data->actual_frequency = 0;
    data->pwm_active = PWM_ACTIVE::NONE;
}
void Actuators::turn_off_active_pwm() {
    switch (data->pwm_active) {
        case PWM_ACTIVE::NONE:
            return;
        case PWM_ACTIVE::U:
            turn_off_u();
            return;
        case PWM_ACTIVE::V:
            turn_off_v();
            return;
        case PWM_ACTIVE::W:
            turn_off_w();
    }
}
void Actuators::Disable_reset() { Reset.turn_off(); }
void Actuators::Enable_reset() { Reset.turn_on(); }

void Actuators::set_three_frequencies(uint32_t frequency) {
    U_Dual.set_frequency(frequency);
    V_Dual.set_frequency(frequency);
    W_Dual.set_frequency(frequency);
    data->actual_frequency = get_frequency_u();
    data->actual_frequency = get_frequency_v();
    data->actual_frequency = get_frequency_w();
}