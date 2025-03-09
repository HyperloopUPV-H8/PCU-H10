#include "Sensors/Sensors.hpp"

Sensors::Sensors(Data_struct *data):data(data),currentSensors(data){}

void Sensors::read(){
    currentSensors.read();
    sensor_voltage_A.read();
    sensor_voltage_B.read();
}
void Sensors::start(){
    speetec.start();
}
void Sensors::read_speetec(){
    speetec.read();
    data->speed_km_h_encoder = 3.6*data->speed_encoder;
}