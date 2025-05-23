#pragma once
#include "Data/Data.hpp"
class Communication{
    private:
        
        Data_struct *Data;
        ServerSocket *ControlStationSocket;
        DatagramSocket *datagramSocket;
        #if COMMUNICATION_HVSCU 
        DatagramSocket *HVSCU_datagramSocket;
        #endif
        //orders//
        #if TEST_PWM
            HeapOrder *Enable_Buffer_Order;
            HeapOrder *Disable_Buffer_Order;
            HeapOrder *Send_pwm_Order;
            HeapOrder *Stop_pwm_Order;
            HeapOrder *Disable_Reset;
            HeapOrder *Enable_Reset;
        #endif   
        HeapOrder *Start_space_vector;
        HeapOrder *Stop_space_vector;
        HeapOrder *Current_reference_Order;
        HeapOrder *Speed_reference_Order;
        HeapOrder *Zeroing_Order;
        HeapOrder *Precharge_Order;
        HeapOrder *Complete_Run_order;
        HeapOrder *Start_regenerative_now_order;
        //packets//
        HeapPacket *Pwm_packet;
        HeapPacket *Current_sensor_Packet;
        HeapPacket *StateMachine_Packet;
        HeapPacket *Encoder_Packet;
        HeapPacket *Control_Speed_Packet;
        HeapPacket *ControlState_Packet;
    public:
        #if COMMUNICATION_HVSCU 
        DatagramSocket *HVSCU_datagramSocket;
        #endif
        HeapPacket *batteries_Packet;
        Communication(Data_struct *data);
        #if TEST_PWM
            static bool received_enable_buffer;
            static bool received_disable_buffer;
            static bool received_pwm_order;
            static bool received_stop_pwm_order;
            static bool received_disable_reset;
            static bool received_enable_reset;
        #endif
        static bool received_activate_space_vector;
        static bool received_stop_space_vector;
        static bool received_Current_reference_order;
        static bool received_Speed_reference_order;
        static bool received_zeroing_order;
        static bool received_Precharge_order;
        static bool received_Complete_Run_order;
        static bool received_start_regenerative_now_order;

        static float frequency_received;
        static float duty_cycle_received;
        static float frequency_space_vector_received;
        static float ref_voltage_space_vector_received;
        static float current_reference_received;
        static float Vmax_control_received;
        static float speed_reference_received;
        static PWM_ACTIVE pwm_received;
        
        void send_UDP_packets(); 
        bool is_connected();
       

};