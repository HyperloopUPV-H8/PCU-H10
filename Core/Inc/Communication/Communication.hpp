#pragma once
#include "Data/Data.hpp"
class Communication{
    private:
        
        Data_struct *Data;
        ServerSocket ControlStationSocket;
        DatagramSocket *datagramSocket;
        HeapOrder *Enable_Buffer_Order;
        HeapOrder *Disable_Buffer_Order;
        HeapOrder *Send_pwm_Order;
        HeapOrder *Stop_pwm_Order;
        HeapOrder *Disable_Reset;
        HeapOrder *Enable_Reset;
        HeapOrder *Choose_Batteries_type;
        HeapOrder *Start_space_vector;
        HeapOrder *Stop_space_vector;
        HeapOrder *Current_reference_Order;
        HeapPacket *Pwm_packet;
        HeapPacket *batteries_Packet;
        HeapPacket *Current_sensor_Packet;
        HeapPacket *StateMachine_Packet;
    public:
        Communication(Data_struct *data);
        static bool received_enable_buffer;
        static bool received_disable_buffer;
        static bool received_pwm_order;
        static bool received_stop_pwm_order;
        static bool received_disable_reset;
        static bool received_enable_reset;
        static bool received_activate_space_vector;
        static bool received_stop_space_vector;
        static bool received_choose_batteries_type;
        static bool received_Current_reference_order;
        static float frequency_received;
        static float duty_cycle_received;
        static float frequency_space_vector_received;
        static float ref_voltage_space_vector_received;
        static float current_reference_received;
        static float Vmax_control_received;
        static PWM_ACTIVE pwm_received;
        static Battery_Connector connector_received;
        void send_UDP_packets(); 
        bool is_connected();
       

};