#include "Communication/Communication.hpp"

#if TEST_PWM
    bool Communication::received_enable_buffer = false;
    bool Communication::received_disable_buffer = false;
    bool Communication::received_pwm_order = false;
    bool Communication::received_stop_pwm_order = false;
    bool Communication::received_disable_reset = false;
    bool Communication::received_enable_reset = false;
#endif
bool Communication::received_activate_space_vector = false;
bool Communication::received_stop_space_vector = false;
bool Communication::received_Current_reference_order = false;
bool Communication::received_zeroing_order = false;
bool Communication::received_Speed_reference_order = false;
bool Communication::received_Precharge_order = false;
bool Communication::received_Complete_Run_order = false;


float Communication::duty_cycle_received{};
float Communication::ref_voltage_space_vector_received{};
float Communication::Vmax_control_received{};
float Communication::frequency_space_vector_received{};
float Communication::frequency_received{};

float Communication::current_reference_received{};
float Communication::speed_reference_received{};

PWM_ACTIVE Communication::pwm_received = PWM_ACTIVE::NONE;
#if TEST_PWM
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
#endif
void received_activate_space_vector_callback(){
    Communication::received_activate_space_vector = true;
}
void received_stop_space_vector_callback(){
    Communication::received_stop_space_vector = true;
}
void received_current_reference_callback(){
    Communication::received_Current_reference_order = true;
}
void received_zeroing_callback(){
    Communication::received_zeroing_order = true;
}
void received_speed_reference_callback(){
    Communication::received_Speed_reference_order = true;    
}
void received_Precharge_callback(){
    Communication::received_Precharge_order = true;
}
void received_Complete_Run_callback(){
    Communication::received_Complete_Run_order = true;
}

Communication::Communication(Data_struct *data): Data(data){
    #if CHILL_KEEPALIVES
        ControlStationSocket = new ServerSocket(Communication_Data::PCU_IP,Communication_Data::TCP_SERVER,1000,500,10);
    #else
        ControlStationSocket = new ServerSocket(Communication_Data::PCU_IP,Communication_Data::TCP_SERVER);
    #endif
    #if COMMUNICATION_HVSCU 
        HVSCU_datagramSocket = new DatagramSocket(Communication_Data::PCU_IP,Communication_Data::UDP_PORT_HVSCU,Communication_Data::HVSCU_IP,Communication_Data::UDP_PORT_HVSCU_SEND);
    #endif
    datagramSocket = new DatagramSocket(Communication_Data::PCU_IP,Communication_Data::UDP_PORT_PCU,Communication_Data::Backend,Communication_Data::UDP_PORT);
    #if TEST_PWM
        Enable_Buffer_Order = new HeapOrder(Communication_Data::ENABLE_BUFFER_ORDER,&received_enable_buffer_callback);
        Disable_Buffer_Order = new HeapOrder(Communication_Data::DISABLE_BUFFER_ORDER,&received_disable_buffer_callback);
        Send_pwm_Order = new HeapOrder(Communication_Data::SEND_PWM_ORDER,&received_pwm_order_callback,&pwm_received,&frequency_received,&duty_cycle_received);
        Stop_pwm_Order = new HeapOrder(Communication_Data::STOP_PWM_ORDER,&stop_pwm_order_callback);
        Disable_Reset = new HeapOrder(Communication_Data::DISABLE_RESET_ORDER,&disable_reset_callback);
        Enable_Reset = new HeapOrder(Communication_Data::ENABLE_RESET_ORDER,&enable_reset_callback);
    #endif
    Start_space_vector = new HeapOrder(Communication_Data::START_SPACE_VECTOR_ORDER,&received_activate_space_vector_callback,&frequency_space_vector_received,&frequency_received,&ref_voltage_space_vector_received,&Vmax_control_received,&Data->Stablished_direction);
    Stop_space_vector = new HeapOrder(Communication_Data::STOP_SPACE_VECTOR_ORDER,&received_stop_space_vector_callback);
    Current_reference_Order = new HeapOrder(Communication_Data::CURRENT_REFERENCE_ORDER,&received_current_reference_callback,&frequency_space_vector_received,&frequency_received,&current_reference_received,&Vmax_control_received,&Data->Stablished_direction);
    Speed_reference_Order = new HeapOrder(Communication_Data::SPEED_REFERENCE_ORDER,&received_speed_reference_callback,&speed_reference_received,&frequency_received,&Vmax_control_received,&Data->Stablished_direction);
    Complete_Run_order = new HeapOrder(Communication_Data::MAKE_COMPLETE_RUN_ORDER,&received_Complete_Run_callback,&speed_reference_received,&frequency_received,&Vmax_control_received,&Data->Stablished_direction);
    Zeroing_Order = new HeapOrder(Communication_Data::ZEROING_ORDER,&received_zeroing_callback);
    Precharge_Order = new HeapOrder(Communication_Data::PRECHARGE_ORDER,&received_Precharge_callback,&frequency_received,&Vmax_control_received);
    //packets
    Pwm_packet  = new HeapPacket(Communication_Data::PWM_PACKET,&Data->pwm_active,&Data->actual_frequency,&Data->modulation_frequency,&Data->actual_duty,&Data->buffer_enable);
    batteries_Packet = new HeapPacket(Communication_Data::BATTERIES_PACKET,&Data->actual_voltage_battery_A,&Data->actual_voltage_battery_B);
    Current_sensor_Packet = new HeapPacket(Communication_Data::CURRENT_SENSOR_PACKET,&Data->actual_current_sensor_u_a,&Data->actual_current_sensor_v_a,&Data->actual_current_sensor_w_a,&Data->actual_current_sensor_u_b,&Data->actual_current_sensor_v_b,&Data->actual_current_sensor_w_b,&Data->current_Peak,&Data->current_error,&Data->target_voltage,&Data->time,&Data->imod);
    StateMachine_Packet = new HeapPacket(Communication_Data::STATE_MACHINE_PACKET,&Data->state_pcu,&Data->operational_state_pcu);
    Encoder_Packet = new HeapPacket(Communication_Data::ENCODER_PACKET,&Data->position_encoder,&Data->direction_encoder,&Data->speed_encoder,&Data->speed_km_h_encoder,&Data->acceleration_encoder);
    Control_Speed_Packet = new HeapPacket(Communication_Data::CONTROL_SPEED_PACKET,&Data->target_speed,&Data->speed_error,&Data->actual_current_ref);
    ControlState_Packet = new HeapPacket(Communication_Data::CONTROL_STATE_PACKET,&Data->Stablished_direction,&Data->currentState,&Data->speedState);
}
void Communication::send_UDP_packets(){
    datagramSocket->send_packet(*Pwm_packet);
    datagramSocket->send_packet(*batteries_Packet);
    datagramSocket->send_packet(*Current_sensor_Packet);
    datagramSocket->send_packet(*StateMachine_Packet);
    datagramSocket->send_packet(*Encoder_Packet);
    datagramSocket->send_packet(*Control_Speed_Packet);
    datagramSocket->send_packet(*ControlState_Packet);
    #if COMMUNICATION_HVSCU 
        HVSCU_datagramSocket->send_packet(*batteries_Packet);
    #endif
}

bool Communication::is_connected(){
    return ControlStationSocket->is_connected();
}
