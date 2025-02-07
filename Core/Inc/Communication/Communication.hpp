#pragma once
#include "ST-LIB.hpp"
#include "Data/Data.hpp"
class Communication{
    private:
        
        Data_struct *Data;
        ServerSocket *ControlStationSocket;
        DatagramSocket *datagramSocket;
        HeapOrder *Enable_Buffer_Order;
        HeapOrder *Disable_Buffer_Order;
        HeapOrder *Send_pwm_Order;
        HeapOrder *Stop_pwm_Order;
        HeapPacket *Pwm_packet;
    public:
        Communication(Data_struct *data);
        static bool received_enable_buffer;
        static bool received_disable_buffer;
        static bool received_pwm_order;
        static bool received_stop_pwm_order;
        static uint32_t frequency_received;
        static float duty_cycle_received;
        static PWM_ACTIVE pwm_received;
        void send_UDP_packets(); 
        bool is_connected();
};