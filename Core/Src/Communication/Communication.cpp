#include "Communication/Communication.hpp"

bool Communication::received_enable_buffer = false;
bool Communication::received_disable_buffer = false;
bool Communication::received_pwm_order = false;
bool Communication::received_stop_pwm_order = false;
float Communication::duty_cycle_received = 0.0;
uint32_t Communication::frequency_received = 0;
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
Communication::Communication(Data_struct *data): Data(data){
    ControlStationSocket = new ServerSocket(Communication_Data::PCU_IP,Communication_Data::TCP_CLIENT);
    datagramSocket = new DatagramSocket(Communication_Data::PCU_IP,Communication_Data::UDP_PORT_PCU,Communication_Data::Backend,Communication_Data::UDP_PORT);
    Enable_Buffer_Order = new HeapOrder(Communication_Data::ENABLE_BUFFER_ORDER,&received_enable_buffer_callback);
    Disable_Buffer_Order = new HeapOrder(Communication_Data::DISABLE_BUFFER_ORDER,&received_disable_buffer_callback);
    Send_pwm_Order = new HeapOrder(Communication_Data::SEND_PWM_ORDER,&received_pwm_order_callback,&pwm_received,&frequency_received,&duty_cycle_received);
    Stop_pwm_Order = new HeapOrder(Communication_Data::STOP_PWM_ORDER,&stop_pwm_order_callback);
    Pwm_packet  = new HeapPacket(Communication_Data::PWM_PACKET,&Data->pwm_active,&Data->actual_frequency,&Data->actual_duty,&Data->buffer_enable);

}
void Communication::send_UDP_packets(){
    datagramSocket->send_packet(*Pwm_packet);
}

bool Communication::is_connected(){
    return ControlStationSocket->is_connected();
}