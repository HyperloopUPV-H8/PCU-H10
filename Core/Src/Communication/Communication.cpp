#include "Communication/Communication.hpp"

bool Communication::received_enable_buffer = false;
bool Communication::received_disable_buffer = false;
bool Communication::received_pwm_order = false;
bool Communication::received_stop_pwm_order = false;
bool Communication::received_disable_reset = false;
bool Communication::received_enable_reset = false;
bool Communication::received_activate_space_vector = false;
bool Communication::received_stop_space_vector = false;
bool Communication::received_Current_reference_order = false;

float Communication::duty_cycle_received{};
float Communication::ref_voltage_space_vector_received{};
float Communication::Vmax_control_received{};
float Communication::frequency_space_vector_received{};
float Communication::frequency_received{};

float Communication::current_reference_received{};

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

void received_activate_space_vector_callback(){
    Communication::received_activate_space_vector = true;
}
void received_stop_space_vector_callback(){
    Communication::received_stop_space_vector = true;
}
void received_current_reference_callback(){
    Communication::received_Current_reference_order = true;
}
Communication::Communication(Data_struct *data): Data(data),ControlStationSocket(Communication_Data::PCU_IP,Communication_Data::TCP_SERVER){
    datagramSocket = new DatagramSocket(Communication_Data::PCU_IP,Communication_Data::UDP_PORT_PCU,Communication_Data::Backend,Communication_Data::UDP_PORT);
    Enable_Buffer_Order = new HeapOrder(Communication_Data::ENABLE_BUFFER_ORDER,&received_enable_buffer_callback);
    Disable_Buffer_Order = new HeapOrder(Communication_Data::DISABLE_BUFFER_ORDER,&received_disable_buffer_callback);
    Send_pwm_Order = new HeapOrder(Communication_Data::SEND_PWM_ORDER,&received_pwm_order_callback,&pwm_received,&frequency_received,&duty_cycle_received);
    Stop_pwm_Order = new HeapOrder(Communication_Data::STOP_PWM_ORDER,&stop_pwm_order_callback);
    Disable_Reset = new HeapOrder(Communication_Data::DISABLE_RESET_ORDER,&disable_reset_callback);
    Enable_Reset = new HeapOrder(Communication_Data::ENABLE_RESET_ORDER,&enable_reset_callback);

    Start_space_vector = new HeapOrder(Communication_Data::START_SPACE_VECTOR_ORDER,&received_activate_space_vector_callback,&frequency_space_vector_received,&frequency_received,&ref_voltage_space_vector_received,&Vmax_control_received);
    Stop_space_vector = new HeapOrder(Communication_Data::STOP_SPACE_VECTOR_ORDER,&received_stop_space_vector_callback);
    Current_reference_Order = new HeapOrder(Communication_Data::CURRENT_REFERENCE_ORDER,&received_current_reference_callback,&frequency_space_vector_received,&frequency_received,&current_reference_received,&Vmax_control_received);
    Pwm_packet  = new HeapPacket(Communication_Data::PWM_PACKET,&Data->pwm_active,&Data->actual_frequency,&Data->actual_duty,&Data->buffer_enable);
    batteries_Packet = new HeapPacket(Communication_Data::BATTERIES_PACKET,&Data->actual_voltage_battery_A,&Data->actual_voltage_battery_B);
    Current_sensor_Packet = new HeapPacket(Communication_Data::CURRENT_SENSOR_PACKET,&Data->actual_current_sensor_u_a,&Data->actual_current_sensor_v_b,&Data->actual_current_sensor_w_a,&Data->actual_current_sensor_u_b,&Data->actual_current_sensor_v_b,&Data->actual_current_sensor_w_b,&Data->current_Peak,&Data->error_PI,&Data->target_voltage,&Data->time);
    StateMachine_Packet = new HeapPacket(Communication_Data::STATE_MACHINE_PACKET,&Data->state_pcu,&Data->operational_state_pcu);
    
}
void Communication::send_UDP_packets(){
    datagramSocket->send_packet(*Pwm_packet);
    datagramSocket->send_packet(*batteries_Packet);
    datagramSocket->send_packet(*Current_sensor_Packet);
    datagramSocket->send_packet(*StateMachine_Packet);
}

bool Communication::is_connected(){
    return ControlStationSocket.is_connected();
}