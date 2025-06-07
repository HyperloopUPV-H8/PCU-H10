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
}
void Sensors::start(){
    speetec.turn_on();
}
void Sensors::read_speetec(){
    speetec.read();
    data->speed_km_h_encoder = 3.6*data->speed_encoder;
}
void Sensors::read_reeds(){
    reed1.read();
    reed2.read();
    reed3.read();
    reed4.read();
}

bool Sensors::reeds_braking(){
    #ifdef FALSIFY_BRAKES
        return false;
    #endif
    #ifndef FALSIFY_BRAKES
        data->reed1 == PinState::OFF || data->reed2 == PinState::OFF || data->reed3 == PinState::OFF || data->reed4 == PinState::OFF;
    #endif
}