#pragma once
#include "ST-LIB.hpp"
namespace Pinout{
    static constexpr Pin& U_PWM = PE8;
    static constexpr Pin& U_PWM_NEGATED = PE9;
    static constexpr Pin& W_PWM = PE13;
    static constexpr Pin& W_PWM_NEGATED = PE12;
    static constexpr Pin& V_PWM = PE11;
    static constexpr Pin& V_PWM_NEGATED = PE10; 
    static constexpr Pin& ENABLE_BUFFER = PF4; 
    static constexpr Pin& Reset = PE15;
};

namespace Communication_Data{
    const IPV4 Backend = {"192.168.0.9"};
    const IPV4 PCU_IP = {"192.168.1.5"};
    static constexpr uint32_t UDP_PORT = 50400;
    static constexpr uint32_t TCP_SERVER = 50500;
    static constexpr uint32_t TCP_CLIENT = 50401;
    static constexpr uint32_t UDP_PORT_PCU = 50421;
    static constexpr uint16_t ENABLE_BUFFER_ORDER = 500;
    static constexpr uint16_t DISABLE_BUFFER_ORDER = 501;
    static constexpr uint16_t SEND_PWM_ORDER = 502;
    static constexpr uint16_t STOP_PWM_ORDER = 503;
    static constexpr uint16_t PWM_PACKET = 550;
};
enum class PWM_ACTIVE: uint8_t{
    NONE,
    U,
    V,
    W
}; 
enum class BUFFER_ENABLE : uint8_t{
    OFF,
    ON
};
struct Data_struct{
    PWM_ACTIVE pwm_active{};
    uint32_t actual_frequency{};
    float actual_duty{};
    BUFFER_ENABLE buffer_enable{};
};