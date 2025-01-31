import enum
import sys, os

from Tests import PCU
from runner import runner
from vmcu.assertions import *

sys.path.append(os.path.join(os.path.dirname(__file__), "VirtualMCU", "src"))


@runner.test()
def PCU_test():
    pcu = PCU()

    check(pcu.is_state(PCU.StateMachine.GENERAL, PCU.General_SM.CONNECTING))

    # Now PCU is on Connecting state

    pcu.connect_gui()
    completes(
        wait_until_true(
            pcu.is_state(PCU.StateMachine.GENERAL, PCU.General_SM.OPERATIONAL)
        ),
        before=seconds(1),
    )

    # Now PCU is on Operational state

    check(pcu.is_state(PCU.StateMachine.OPERATIONAL, PCU.Operational_SM.IDLE))

    # Now PCU is on Operational - Idle state

    enable_order = pcu.packets.serialize_packet(500)
    pcu.transmit(enable_order)

    completes(
        wait_until_true(pcu.get_enable()),
        before=seconds(1),
    )

    disable_order = pcu.packets.serialize_packet(501)
    pcu.transmit(disable_order)

    completes(
        wait_until_true(not pcu.get_enable()),
        before=seconds(1),
    )

    send_pwm_U_order = pcu.packets.serialize_packet(502, "U", 1000, 90)
    pcu.transmit(send_pwm_U_order)

    completes(
        wait_until_true(pcu.check_pwm(PCU.PWM.U, 1000, 90)),
        before=seconds(1),
    )

    check(pcu.is_state(PCU.StateMachine.OPERATIONAL, PCU.Operational_SM.PWM))

    # Now PCU is on Operational - PWM state

    send_pwm_V_order = pcu.packets.serialize_packet(502, "V", 2000, 80)
    pcu.transmit(send_pwm_V_order)

    completes(
        wait_until_true(pcu.check_pwm(PCU.PWM.V, 2000, 80)),
        before=seconds(1),
    )

    send_pwm_W_order = pcu.packets.serialize_packet(502, "W", 3000, 70)
    pcu.transmit(send_pwm_W_order)

    completes(
        wait_until_true(pcu.check_pwm(PCU.PWM.W, 3000, 70)),
        before=seconds(1),
    )

    stop_pwm_order = pcu.packets.serialize_packet(504)
    pcu.transmit(stop_pwm_order)

    check(pcu.is_state(PCU.StateMachine.OPERATIONAL, PCU.Operational_SM.IDLE))

    # TODO check pwms are powered off

    # Now PCU is on Operational - Idle state

    pcu.disconnect_gui()

    check(pcu.is_state(PCU.StateMachine.GENERAL, PCU.General_SM.FAULT))

    # Now PCU is on FAULT state

    check(not pcu.get_enable())
    # TODO check PWMs are off


runner.run()  # Runs the tests, do not delete!
