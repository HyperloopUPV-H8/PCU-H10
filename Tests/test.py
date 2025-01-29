import enum
import sys, os

from vmcu.services.communications.Packets import Packets

sys.path.append(os.path.join(os.path.dirname(__file__), "VirtualMCU", "src"))

from runner import runner
from vmcu.shared_memory import SharedMemory
from vmcu.services.digital_out import DigitalOutService
from vmcu.pin.pinout import Pinout
from vmcu.assertions import *
from vmcu.services.communications.ServerSocket import Server
from enum import Enum
from vmcu.services.DualPWM import DualPWM

# @runner.test()
# def led_toggle():
#     TOGGLE_PERIOD = milliseconds(100 * 2)
#     ALLOWED_SLACK = milliseconds(5)

#     shm = SharedMemory("gpio__blinking_led", "state_machine__blinking_led")
#     led = DigitalOutService(shm, Pinout.PA1)

#     def led_turns_on():
#         nonlocal led
#         return led.get_pin_state() is DigitalOut.State.High

#     def led_turns_off():
#         nonlocal led
#         return led.get_pin_state() is DigitalOut.State.Low

#     #sync with board
#     completes(
#         wait_until_true(led_turns_on),
#         before=(TOGGLE_PERIOD / 2) + ALLOWED_SLACK,
#         msg="Sync fails"
#     )

#     for i in range(150):
#         completes(
#             wait_until_true(led_turns_off),
#             before=(TOGGLE_PERIOD / 2) + ALLOWED_SLACK,
#             after=(TOGGLE_PERIOD / 2) - ALLOWED_SLACK,
#             msg="turns off"
#         )
#         completes(
#             wait_until_true(led_turns_on),
#             before=(TOGGLE_PERIOD / 2) + ALLOWED_SLACK,
#             after=(TOGGLE_PERIOD / 2) - ALLOWED_SLACK,
#             msg="turns on"
#         )
#         print("toggle", i)


class StateMachine(Enum):
    GENERAL = 0
    OPERATIONAL = 1


class General_SM(Enum):
    CONNECTING = 0
    OPERATIONAL = 1
    FAULT = 2


class Operational_SM(Enum):
    IDLE = 0
    PWM = 1


def is_state(sm: StateMachine, state: int, shm: SharedMemory) -> int:
    return shm.get_state_machine_state(sm) == state

def check_pwm(pwm: DualPWM, freq: int, duty: float) -> bool:
    return pwm.get_is_on() and pwm.get_duty_cycle() == duty and pwm.get_frequency() == freq


@runner.test()
def PCU_test():
    shm = SharedMemory("gpio_PCU", "state_machine_PCU")
    server = Server("localhost", 50400)
    mcu = server.get_connected_clients()[0]
    packet_definitions = {
        500: [],  # enable_buffer
        501: [],  # disable_buffer
        502: [enum("NONE", "U", "V", "W"), "uint32", "float32"],  # send_pwm
        503: [],  # stop_pwm
    }
    packets = Packets(packet_definitions)
    enable = DigitalOutService(shm, Pinout.PF4)
    pwmU = DualPWM(Pinout.PE9, Pinout.PE8)
    pwmV = DualPWM(Pinout.PE11, Pinout.PE10)
    pwmW = DualPWM(Pinout.PE13, Pinout.PE12)

    check(is_state(StateMachine.GENERAL, General_SM.CONNECTING, shm))

    # Now PCU is on Connecting state

    server.start()
    completes(
        wait_until_true(is_state(StateMachine.GENERAL, General_SM.OPERATIONAL, shm)),
        before=seconds(1),
    )

    # Now PCU is on Operational state

    check(is_state(StateMachine.OPERATIONAL, Operational_SM.IDLE, shm))

    # Now PCU is on Operational - Idle state

    enable_order = packets.serialize_packet(500)
    server.transmit(enable_order, mcu)

    completes(
        wait_until_true(enable.state),
        before=seconds(1),
    )

    disable_order = packets.serialize_packet(501)
    server.transmit(disable_order, mcu)

    completes(
        wait_until_true(not enable.state),
        before=seconds(1),
    )

    send_pwm_U_order = packets.serialize_packet(502, "U", 1000, 90)
    server.transmit(send_pwm_U_order, mcu)

    completes(
        wait_until_true(check_pwm(pwmU, 1000, 90)),
        before=seconds(1),
    )

    check(is_state(StateMachine.OPERATIONAL, Operational_SM.PWM, shm))

    # Now PCU is on Operational - PWM state

    send_pwm_V_order = packets.serialize_packet(502, "U", 2000, 80)
    server.transmit(send_pwm_V_order, mcu)

    completes(
        wait_until_true(check_pwm(pwmV, 2000, 80)),
        before=seconds(1),
    )

    send_pwm_W_order = packets.serialize_packet(502, "U", 3000, 70)
    server.transmit(send_pwm_W_order, mcu)

    completes(
        wait_until_true(check_pwm(pwmW, 3000, 70)),
        before=seconds(1),
    )

    stop_pwm_order = packets.serialize_packet(504)
    server.transmit(stop_pwm_order, mcu)

    check(is_state(StateMachine.OPERATIONAL, Operational_SM.IDLE, shm))

    # Now PCU is on Operational - Idle state

    server.stop()
    
    check(is_state(StateMachine.GENERAL, General_SM.FAULT, shm))

    # Now PCU is on FAULT state
    check(not enable.state)
    #TODO check PWMs are off
    




runner.run()  # Runs the tests, do not delete!
