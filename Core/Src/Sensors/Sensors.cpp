#include "Sensors/Sensors.hpp"

Sensors::Sensors(Data_struct *data):data(data),currentSensors(data){}

void Sensors::read(){
    currentSensors.read();
}
