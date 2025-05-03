#pragma once
#include "ST-LIB.hpp"
//configuraciones
#define PPU_USING 2     // 0 PPU connector A, 1 PPU connector B , 2 Both PPU
#define COMMUNICATION_HVSCU 0   // 0 = No communication, 1 = send packet with voltages 
#define TEST_PWM 0 //0 = no test pwm, eliminate some orders, 1 = have every order for test single pwm
#define CHILL_KEEPALIVES  1  //0 = agressive keepalives, 1 = chill keepalives
#define MODE_CALCULATE_SIN 0 //0 = Normal sin, 1 = Look Up table with direct interpolation
#define ARMONIC_INJECTION 1 // 0 = NO armonic injection, 1 = Armonic injection active
namespace Pinout{
    static constexpr Pin& U_PWM = PE9;
    static constexpr Pin& U_PWM_NEGATED = PE8; 
    static constexpr Pin& V_PWM = PE11;
    static constexpr Pin& V_PWM_NEGATED = PE10; 
    static constexpr Pin& W_PWM = PE13;
    static constexpr Pin& W_PWM_NEGATED = PE12;

    static constexpr Pin& ENABLE_BUFFER = PF4; 
    static constexpr Pin& Batt_Voltage_A = PF3;
    static constexpr Pin& Batt_Voltage_B = PF5;
    static constexpr Pin& LED_COMMUTATION = PG6;
    static constexpr Pin& LED_FAULT = PG7;
    static constexpr Pin& LED_OPERATIONAL = PG8;
    //current sensors
    static constexpr Pin& CURRENT_SENSOR_U_A = PA0;
    static constexpr Pin& CURRENT_SENSOR_U_B = PA6;
    static constexpr Pin& CURRENT_SENSOR_V_A = PA4;
    static constexpr Pin& CURRENT_SENSOR_V_B = PB0;
    static constexpr Pin& CURRENT_SENSOR_W_A = PA5;
    static constexpr Pin& CURRENT_SENSOR_W_B = PB1;
    //Speetec
    static constexpr Pin& ENCODER_A = PF1;
    static constexpr Pin& ENCODER_B = PF0;
};
namespace Sensors_data{
    static constexpr float slope_current_sensor = 96.945;
    static constexpr float offset_current_sensor = 0.0;
    
    static constexpr float slope_voltage_sensor = 133.31627;
    static constexpr float offset_voltage_sensor = -9.24655;

    static constexpr uint32_t read_sensors_us = 200; 
    static constexpr double encoder_sample_time_s =  static_cast<double>(read_sensors_us)/1e6; // this has to be the same frequency that the read is done
    static constexpr double encoder_counter_distance_m = 0.0001;
    static constexpr size_t  encoder_samples = 250;
};

namespace Communication_Data{
    const IPV4 Backend = {"192.168.0.9"};
    const IPV4 PCU_IP = {"192.168.1.5"};
    static constexpr uint32_t UDP_PORT = 50400;
    static constexpr uint32_t TCP_SERVER = 50500;
    static constexpr uint32_t TCP_CLIENT = 50500;
    static constexpr uint32_t UDP_PORT_PCU = 50400;
    #if COMMUNICATION_HVSCU
    static constexpr uint32_t UDP_PORT_HVSCU_SEND = 50417;
    static constexpr uint32_t UDP_PORT_HVSCU = 50417;
    const IPV4 HVSCU_IP = {"192.168.1.7"};
    #endif
    //orders//
    static constexpr uint16_t ENABLE_BUFFER_ORDER = 500;
    static constexpr uint16_t DISABLE_BUFFER_ORDER = 501;
    static constexpr uint16_t SEND_PWM_ORDER = 502;
    static constexpr uint16_t STOP_PWM_ORDER = 503;
    static constexpr uint16_t ENABLE_RESET_ORDER = 504;
    static constexpr uint16_t DISABLE_RESET_ORDER = 505;
    static constexpr uint16_t START_SPACE_VECTOR_ORDER = 507;
    static constexpr uint16_t STOP_SPACE_VECTOR_ORDER = 508;
    static constexpr uint16_t CURRENT_REFERENCE_ORDER = 509;
    static constexpr uint16_t ZEROING_ORDER = 510;
    static constexpr uint16_t SPEED_REFERENCE_ORDER = 511;
    static constexpr uint16_t PRECHARGE_ORDER = 512;
    static constexpr uint16_t MAKE_COMPLETE_RUN_ORDER = 513;
    
    //packets//

    static constexpr uint16_t PWM_PACKET = 550;
    static constexpr uint16_t BATTERIES_PACKET = 551;
    static constexpr uint16_t CURRENT_SENSOR_PACKET = 552;
    static constexpr uint16_t STATE_MACHINE_PACKET = 553;
    static constexpr uint16_t ENCODER_PACKET = 554;
    static constexpr uint16_t CONTROL_SPEED_PACKET = 555;
    static constexpr uint16_t CONTROL_STATE_PACKET = 556;
};
namespace Current_Control_Data{
    static constexpr double kp_accelerate = 1.0;
    static constexpr double ki_accelerate = 12.0;
    static constexpr double kp_regenerate = 0.2;
    static constexpr double ki_regenerate = 6.0;
    static constexpr double period = 0.0002;
    static constexpr uint32_t microsecond_period = static_cast<uint32_t>(period * 1000000);
}
namespace Speed_Control_Data{
    static constexpr double kp_accelerate = 9.0;
    static constexpr double ki_accelerate = 1.0;
    static constexpr double kp_regenerate = 0.2;
    static constexpr double ki_regenerate = 1.5;
    static constexpr double period = 0.001;
    static constexpr uint32_t microsecond_period = static_cast<uint32_t>(period * 1000000);
}
enum class PWM_ACTIVE: uint8_t{
    NONE = 0,
    U = 1,
    V = 2,
    W = 3
}; 
enum class BUFFER_ENABLE : uint8_t{
    OFF = 0,
    ON = 1
};

enum State_PCU: uint8_t{
    Connecting,
    Operational,
    Fault
};
enum Operational_State_PCU: uint8_t{
    Idle,
    Sending_PWM,
    Accelerating
};
enum ControlStates{
    accelerate,
    regenerate
};
 using Direction = EncoderSensor<Sensors_data::encoder_samples>::Direction;
struct Data_struct{
    PWM_ACTIVE pwm_active{};
    uint32_t actual_frequency{};
    uint32_t modulation_frequency{};
    float actual_duty{};
    BUFFER_ENABLE buffer_enable{};
    
    //batteries
    float actual_voltage_battery_A{};
    float actual_voltage_battery_B{};
    //current sensor
    float actual_current_sensor_u_a{};
    float actual_current_sensor_u_b{};
    float actual_current_sensor_v_a{};
    float actual_current_sensor_v_b{};
    float actual_current_sensor_w_a{};
    float actual_current_sensor_w_b{};
    uint8_t state_pcu{}; // general state
    uint8_t operational_state_pcu{}; //operational state
    double current_error{};
    double current_Peak{};
    double target_voltage{};
    float time{};
    float imod{};
    //encoder
    double position_encoder{};
    Direction direction_encoder{};
    double speed_encoder{};
    double acceleration_encoder{};
    double speed_km_h_encoder{};
    //speed
    float target_speed{};
    double speed_error{};
    float actual_current_ref{};
    //control
    ControlStates currentState{ControlStates::accelerate};
    ControlStates speedState{ControlStates::accelerate};
    Direction Stablished_direction{Direction::FORWARD};
};