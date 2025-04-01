#include "Control/SpaceVector.hpp"


SpaceVector::SpaceVector(Three_Phased_PWM *three_pwm_class, Data_struct *data)
    : three_pwm(three_pwm_class), data(data) {}

void SpaceVector::set_target_voltage(float V_ref) {
    if (V_ref < 0) V_ref = 0;
    Imodulation = V_ref * 2.0 / VMAX;
    if (Imodulation >= IMAX) Imodulation = IMAX;
    data->imod = Imodulation;
}

void SpaceVector::set_frequency_Modulation(uint32_t freq) {
    Modulation_frequency = freq;
    data->modulation_frequency = Modulation_frequency;
}

void SpaceVector::calculate_duties() {
#if MODE_CALCULATE_SIN == 0
    float sin_u_prueba = Imodulation * sin(2.0f * M_PI * Modulation_frequency * time);
    float sin_v_prueba = Imodulation *
                  sin(2.0f * M_PI * Modulation_frequency * time + 2 * M_PI / 3);
    float sin_w_prueba = Imodulation *
                  sin(2.0f * M_PI * Modulation_frequency * time - 2 * M_PI / 3);
#endif
#if MODE_CALCULATE_SIN == 1
    float sin_u = calculate_sin_phase(phase::U);
    float sin_v = calculate_sin_phase(phase::V);
    float sin_w = calculate_sin_phase(phase::W);
#endif
    float offset =  (std::max({sin_u, sin_v, sin_w}) + std::min({sin_u, sin_v, sin_w})) / 2;

    sin_u -= offset;
    sin_v -= offset;
    sin_w -= offset;

    if (data->Stablished_direction == Direction::Forward) {
        three_pwm->set_duty_u((sin_u / 2.0 + 0.5) * 100.0);
        three_pwm->set_duty_v((sin_v / 2.0 + 0.5) * 100.0);
    } else {
        three_pwm->set_duty_u((sin_v / 2.0 + 0.5) * 100.0);
        three_pwm->set_duty_v((sin_u / 2.0 + 0.5) * 100.0);
    }
    three_pwm->set_duty_w((sin_w / 2.0 + 0.5) * 100.0);
    time += Period / 1000000.0;
    data->time = time;
}

uint32_t SpaceVector::get_modulation_frequency() {
    return Modulation_frequency;
}

float SpaceVector::get_actual_time() { return time; }
void SpaceVector::set_VMAX(float Vmax) { VMAX = Vmax; }

#if MODE_CALCULATE_SIN == 1
float SpaceVector::calculate_sin_look_up_table(float angle) {
    // control range between 0 and 360
    while (angle < 0) {
        angle += 2.0f * M_PI;
    }
    while(angle > (2.0f*M_PI)){
        angle = angle - (2.0f*M_PI);
    }
    if (angle <= (M_PI / 2.0f)) {
        int idx = (angle * NUMBER_POINTS) / (M_PI / 2.0f);
        float interpolation =
            static_cast<float>((angle * NUMBER_POINTS) / (M_PI / 2.0f)) - idx;
        return (look_up_table_sin[idx] * (1.0f - interpolation) + look_up_table_sin[idx + 1] * (interpolation)) ;
    } else if (angle <= (M_PI)) {
        angle = M_PI - angle;
        int idx = (angle * NUMBER_POINTS) / (M_PI / 2.0f);
        float interpolation =
            static_cast<float>((angle * NUMBER_POINTS) / (M_PI / 2.0f)) - idx;
        return (look_up_table_sin[idx] * (1.0f - interpolation) +
                look_up_table_sin[idx - 1] * (interpolation));
    } else if (angle <= (M_PI * 3.0f) / 2.0f) {
        angle = angle - M_PI;
        int idx = (angle * NUMBER_POINTS) / (M_PI / 2.0f);
        float interpolation =
            static_cast<float>((angle * NUMBER_POINTS) / (M_PI / 2.0f)) - idx;
        return -(look_up_table_sin[idx] * (1.0f - interpolation) +
                 look_up_table_sin[idx + 1] * (interpolation));
    } else {
        angle = 2.0 * M_PI - angle;
        int idx = (angle * NUMBER_POINTS) / (M_PI / 2.0f);
        float interpolation =
            static_cast<float>((angle * NUMBER_POINTS) / (M_PI / 2.0f)) - idx;
        return -(look_up_table_sin[idx] * (1.0f - interpolation) +
                 look_up_table_sin[idx - 1] * (interpolation));
    }
}
float SpaceVector::calculate_sin_phase(phase p) {
    float angle = 0.0f;
    if (p == phase::U) {
        angle = (2.0f * M_PI * Modulation_frequency * time);
    }
    else if (p == phase::V) {
        angle = (2.0f * M_PI * Modulation_frequency * time + 2 * M_PI / 3);
    }
    else if (p == phase::W) {
        angle = (2.0f * M_PI * Modulation_frequency * time - 2 * M_PI / 3);
    }
    return calculate_sin_look_up_table(angle);
}
#endif