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
}
void Sensors::start(){
    speetec.turn_on();
}
void Sensors::read_speetec(){
    speetec.read();
    data->speed_km_h_encoder = 3.6*data->speed_encoder;
}