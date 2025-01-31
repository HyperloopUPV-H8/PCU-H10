#include "Communication/Communication.hpp"

bool Communication::received_enable_buffer = false;
bool Communication::received_disable_buffer = false;
bool Communication::received_pwm_order = false;
bool Communication::received_stop_pwm_order = false;
bool Communication::received_choose_batteries_type = false;
bool Communication::received_disable_reset = false;
bool Communication::received_enable_reset = false;
float Communication::frequency_received = 0;
float Communication::duty_cycle_received = 0.0;
Battery_Connector Communication::connector_received = Battery_Connector::A;
PWM_ACTIVE Communication::pwm_received = PWM_ACTIVE::NONE;

void received_enable_buffer_callback(){
    Communication::received_enable_buffer = true;
}
void received_disable_buffer_callback(){
    Communication::received_disable_buffer = true;
}
void received_pwm_order_callback(){
    Communication::received_pwm_order = true;
}
void stop_pwm_order_callback(){
    Communication::received_stop_pwm_order = true;
}
void disable_reset_callback(){
    Communication::received_disable_reset = true;
}
void enable_reset_callback(){
    Communication::received_enable_reset = true;
}
void received_choose_batteries_type_callback(){
    Communication::received_choose_batteries_type = true;
}
Communication::Communication(Data_struct *data): Data(data){
    ControlStationSocket = new ServerSocket(Communication_Data::PCU_IP,Communication_Data::TCP_SERVER);
    datagramSocket = new DatagramSocket(Communication_Data::PCU_IP,Communication_Data::UDP_PORT_PCU,Communication_Data::Backend,Communication_Data::UDP_PORT);
    Enable_Buffer_Order = new HeapOrder(Communication_Data::ENABLE_BUFFER_ORDER,&received_enable_buffer_callback);
    Disable_Buffer_Order = new HeapOrder(Communication_Data::DISABLE_BUFFER_ORDER,&received_disable_buffer_callback);
    Send_pwm_Order = new HeapOrder(Communication_Data::SEND_PWM_ORDER,&received_pwm_order_callback,&pwm_received,&frequency_received,&duty_cycle_received);
    Stop_pwm_Order = new HeapOrder(Communication_Data::STOP_PWM_ORDER,&stop_pwm_order_callback);
    Disable_Reset = new HeapOrder(Communication_Data::DISABLE_RESET_ORDER,&disable_reset_callback);
    Enable_Reset = new HeapOrder(Communication_Data::ENABLE_RESET_ORDER,&enable_reset_callback);
    Choose_Batteries_type = new HeapOrder(Communication_Data::BATTERIES_TYPE_ORDER,&received_choose_batteries_type_callback,&connector_received);
    Pwm_packet  = new HeapPacket(Communication_Data::PWM_PACKET,&Data->pwm_active,&Data->actual_frequency,&Data->actual_duty,&Data->buffer_enable);
    batteries_Packet = new HeapPacket(Communication_Data::BATTERIES_PACKET,&Data->actual_voltage_batteries,&Data->connector_Batteries);
}
void Communication::send_UDP_packets(){
    datagramSocket->send_packet(*Pwm_packet);
    datagramSocket->send_packet(*batteries_Packet);
}

bool Communication::is_connected(){
    return ControlStationSocket->is_connected();
}