#pragma once
#include "ST-LIB.hpp"
#include "Data/Data.hpp"
static constexpr uint32_t initial_frequency = 10000;
static constexpr std::chrono::nanoseconds dead_time_ns(300);
class Three_Phased_PWM{
    private:
        DualPWM U_Dual;
        DualPWM V_Dual;
        DualPWM W_Dual;
        DigitalOutput Enable; // the logic is negated
        DigitalOutput Reset;
        Data_struct *data;
        uint8_t battery_connector_A_id;
        uint8_t battery_connector_B_id;
    public:
    DigitalOutput Led_Commutation;
        Three_Phased_PWM(Pin& u,Pin& u_negated,Pin& v,Pin& v_negated,Pin& w,Pin& w_negated,Pin& enable,Pin& reset,Pin& batt_con_A,Pin& batt_con_B,Pin& Led_Comm,Data_struct *data);
        void start();
        void stop_all();
        void enable();
        void disable();
        void set_frequency_u(uint32_t frequency);
        void set_frequency_v(uint32_t frequency);
        void set_frequency_w(uint32_t frequency);
        void set_duty_u(float duty_cycle);
        void set_duty_v(float duty_cycle);
        void set_duty_w(float duty_cycle);
        float get_duty_u();
        float get_duty_v();
        float get_duty_w();
        void set_three_frequencies(uint32_t frequency);
        uint32_t get_frequency_u();
        uint32_t get_frequency_v();
        uint32_t get_frequency_w();
        void turn_on_u();
        void turn_on_w();
        void turn_on_v();
        void turn_off_u();
        void turn_off_v();
        void turn_off_w();
        void turn_off_active_pwm();
        void Disable_reset();
        void Enable_reset();
        void read_ADC();
};