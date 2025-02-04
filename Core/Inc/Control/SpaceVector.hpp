#pragma once
#include "Three_Phased_PWM/Three_Phased_PWM.hpp"
class SpaceVector{
    private:
        Three_Phased_PWM *three_pwm;
        float modulation;
        uint32_t Modulation_frequency;
        static constexpr float VMAX = 163.0;
        static constexpr float IMAX = 1.1547;
    public:
        static constexpr uint32_t Period = 200; //this Period is in microseconds
        SpaceVector(Three_Phased_PWM *three_pwm_class);
        void set_target_voltage(float V_ref);
        void set_frequency_Modulation(uint32_t freq);
        void calculate_duties();

};