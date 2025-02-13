#pragma once
#include "ST-LIB.hpp"
namespace Pinout{
    static constexpr Pin& U_PWM = PE9;
    static constexpr Pin& U_PWM_NEGATED = PE8;
    static constexpr Pin& W_PWM = PE13;
    static constexpr Pin& W_PWM_NEGATED = PE12;
    static constexpr Pin& V_PWM = PE11;
    static constexpr Pin& V_PWM_NEGATED = PE10; 
    static constexpr Pin& ENABLE_BUFFER = PF4; 
    static constexpr Pin& Reset = PE15;
    static constexpr Pin& Batt_Voltage_A = PF11;
    static constexpr Pin& Batt_Voltage_B = PF12;
    static constexpr Pin& LED_COMMUTION = PG6;
    static constexpr Pin& LED_FAULT = PG7;
    //current sensors
    static constexpr Pin& CURRENT_SENSOR_U_A = PA0;
    static constexpr Pin& CURRENT_SENSOR_U_B = PB0;
    static constexpr Pin& CURRENT_SENSOR_V_A = PA4;
    static constexpr Pin& CURRENT_SENSOR_V_B = PA6;
    static constexpr Pin& CURRENT_SENSOR_W_A =PA5;
    static constexpr Pin& CURRENT_SENSOR_W_B = PB1;
};
namespace Current_Sensors{
    static constexpr float slope_u_a = 1;
    static constexpr float offset_u_a = 0;
    static constexpr float slope_v_a = 1;
    static constexpr float offset_v_a = 0;
    static constexpr float slope_w_a = 1;
    static constexpr float offset_w_a = 0;
    static constexpr float slope_u_b = 1;
    static constexpr float offset_u_b = 0;
    static constexpr float slope_v_b = 1;
    static constexpr float offset_v_b = 0;
    static constexpr float slope_w_b = 1;
    static constexpr float offset_w_b = 0;
};

namespace Communication_Data{
    const IPV4 Backend = {"192.168.0.9"};
    const IPV4 PCU_IP = {"192.168.0.5"};
    static constexpr uint32_t UDP_PORT = 50400;
    static constexpr uint32_t TCP_SERVER = 8000;
    static constexpr uint32_t TCP_CLIENT = 8000;
    static constexpr uint32_t UDP_PORT_PCU = 50400;
    static constexpr uint16_t ENABLE_BUFFER_ORDER = 500;
    static constexpr uint16_t DISABLE_BUFFER_ORDER = 501;
    static constexpr uint16_t SEND_PWM_ORDER = 502;
    static constexpr uint16_t STOP_PWM_ORDER = 503;
    static constexpr uint16_t ENABLE_RESET_ORDER = 504;
    static constexpr uint16_t DISABLE_RESET_ORDER = 505;
    static constexpr uint16_t BATTERIES_TYPE_ORDER = 506;
    static constexpr uint16_t START_SPACE_VECTOR_ORDER = 507;
    static constexpr uint16_t STOP_SPACE_VECTOR_ORDER = 508;
    static constexpr uint16_t CURRENT_REFERENCE_ORDER = 509;
    static constexpr uint16_t PWM_PACKET = 550;
    static constexpr uint16_t BATTERIES_PACKET = 551;
    static constexpr uint16_t CURRENT_SENSOR_PACKET = 552;
    static constexpr uint16_t STATE_MACHINE_PACKET = 553;
};
namespace Current_Control_Data{
    static constexpr double kp = 0.2;
    static constexpr double ki = 6.0;
    static constexpr double period = 0.0002;
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
enum class Battery_Connector : uint8_t{
    A = 0,
    B = 1
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
struct Data_struct{
    PWM_ACTIVE pwm_active{};
    uint32_t actual_frequency{};
    float actual_duty{};
    BUFFER_ENABLE buffer_enable{};
    //batteries
    float actual_voltage_batteries{};
    Battery_Connector connector_Batteries = Battery_Connector::A;
    //current sensor
    float actual_current_sensor_u_a{};
    float actual_current_sensor_u_b{};
    float actual_current_sensor_v_a{};
    float actual_current_sensor_v_b{};
    float actual_current_sensor_w_a{};
    float actual_current_sensor_w_b{};
    uint8_t state_pcu{}; // general state
    uint8_t operational_state_pcu{}; //operational state
};