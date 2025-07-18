#include "Sensors/Sensors.hpp"

Sensors::Sensors(Data_struct *data): 
speetec( Pinout::ENCODER_B,
        Pinout::ENCODER_A,
        Sensors_data::encoder_counter_distance_m,
        Sensors_data::encoder_sample_time_s,
        &data->direction_encoder,
        &data->position_encoder,
        &data->speed_encoder,
        &data->acceleration_encoder),
data(data), 
currentSensors(data) {}

void Sensors::read(){
    currentSensors.read();
    sensor_voltage_A.read();
    sensor_voltage_B.read();
    read_reeds();
    fault_gd_inverter_a.read();
    fault_gd_inverter_b.read();
    ready_gd_inverter_a.read();
    ready_gd_inverter_b.read();
}
void Sensors::start(){
    speetec.turn_on();
}
void Sensors::start_emulated_speetec()
{
    emulation_start = Time::get_global_tick();
}
void Sensors::read_speetec(){
    if(data->emulated_speetec)
    {
        emulated_read();
    }
    else
    {
        speetec.read();
    }
    data->speed_km_h_encoder = 3.6*data->speed_encoder;
}
void Sensors::read_reeds(){
    reed1.read();
    reed2.read();
    data->reed2 = data->reed1;
    data->reed4 = data->reed3;
}

double Sensors::get_emulation_time() {
    return (Time::get_global_tick() - emulation_start) / 1000000000.0;
}

void Sensors::emulated_read()
{
    double time = get_emulation_time();
    switch(data->emulated_speetec)
    {
        case emulated_speetec_States::STOP:
        break;
        case emulated_speetec_States::POSITION:
            data->position_encoder = data->emulated_ref;

        break;
        case emulated_speetec_States::SPEED:
            data->speed_encoder = data->emulated_ref;
            data->position_encoder = time*data->emulated_ref;
        break;
        case emulated_speetec_States::ACCELERATION:
            data->acceleration_encoder = data->emulated_ref;
            data->speed_encoder = time*data->emulated_ref;
            data->position_encoder = time*(data->speed_encoder);
        break;
    }
}

bool Sensors::reeds_braking(){
    #ifdef FALSIFY_BRAKES
        return false;
    #endif
    #ifndef FALSIFY_BRAKES
        return (data->reed1 == PinState::ON || data->reed3 == PinState::ON);
    #endif
}
bool Sensors::check_gate_drivers() const {
    return data->fault_gd_inverter_a == PinState::ON &&
           data->fault_gd_inverter_b == PinState::ON &&
           data->ready_gd_inverter_a == PinState::ON &&
           data->ready_gd_inverter_b == PinState::ON;
}

static MovingAverage <10>sensor_check_window;
void Sensors::update_protections()
{
    sensor_speetec_protection_flag |= check_speetec_disconnection_protection();
    going_backwards |= is_going_backwards();
    sensor_braking_zone_flag |= check_braking_distance_protection();
}

bool Sensors::check_speetec_disconnection_protection()
{
    double position = sensor_check_window.compute(data->position_encoder);
    if(position < 0.1 && data->current_Peak > 6.0) return true;
    return false;
}

bool Sensors::is_going_backwards()
{
    static double prev_position_encoder = 0;
    bool ret = false;
    if((data->position_encoder - prev_position_encoder) < 0.0 && data->recovery_state_vcu == 0)
        ret = true;
    prev_position_encoder = data->position_encoder;
    return ret;
}

bool Sensors::check_braking_distance_protection()
{
    if(data->position_encoder >= BRAKING_DISTANCE) return true;
    return false;
}