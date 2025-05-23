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
    set_frequency_u(initial_frequency);
    set_frequency_v(initial_frequency);
    set_frequency_w(initial_frequency);
    U_Dual.set_dead_time(dead_time_ns);
    V_Dual.set_dead_time(dead_time_ns);
    W_Dual.set_dead_time(dead_time_ns);
    set_duty_u(0.0);
    set_duty_v(0.0);
    set_duty_w(0.0);
    U_Dual.turn_on();
    V_Dual.turn_on();
    W_Dual.turn_on();

    Enable.turn_on();  // works Active low
    Enable_reset();    // has to be active to work
    data->actual_frequency = 0.0;
}
void Actuators::stop_all() {
    set_duty_u(0.0);
    set_duty_v(0.0);
    set_duty_w(0.0);
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
    data->actual_duty_u = duty_cycle;
}
void Actuators::set_duty_v(float duty_cycle) {
    if (duty_cycle < 0.0)
        duty_cycle = 0.0;
    else if (duty_cycle > 100.0)
        duty_cycle = 100.0;

    V_Dual.set_duty_cycle(duty_cycle);
    data->actual_duty_v = duty_cycle;
}
void Actuators::set_duty_w(float duty_cycle) {
    if (duty_cycle < 0.0)
        duty_cycle = 0.0;
    else if (duty_cycle > 100.0)
        duty_cycle = 100.0;

    W_Dual.set_duty_cycle(duty_cycle);
    data->actual_duty_w = duty_cycle;
}
float Actuators::get_duty_u() { return U_Dual.get_duty_cycle(); }
float Actuators::get_duty_v() { return V_Dual.get_duty_cycle(); }
float Actuators::get_duty_w() { return W_Dual.get_duty_cycle(); }
uint32_t Actuators::get_frequency_u() { return U_Dual.get_frequency(); }
uint32_t Actuators::get_frequency_v() { return V_Dual.get_frequency(); }
uint32_t Actuators::get_frequency_w() { return W_Dual.get_frequency(); }

void Actuators::Disable_reset() { Reset.turn_off(); }
void Actuators::Enable_reset() { Reset.turn_on(); }

void Actuators::set_three_frequencies(uint32_t frequency) {
    U_Dual.set_frequency(frequency);
    V_Dual.set_frequency(frequency);
    W_Dual.set_frequency(frequency);
    data->actual_frequency = frequency;
}