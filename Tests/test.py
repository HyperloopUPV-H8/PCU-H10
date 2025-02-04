from PCU import PCU
from runner import runner
from vmcu.assertions import *


@runner.test()
def PCU_test():
    pcu = PCU()

    check(
        pcu.is_state,
        args=(PCU.StateMachine.GENERAL, PCU.General_SM.CONNECTING),
        msg="PCU should be in CONNECTNG state!",
    )

    # Now PCU is on Connecting state

    pcu.connect_gui()
    completes(
        wait_until_true(pcu.is_state),
        args=(PCU.StateMachine.GENERAL, PCU.General_SM.OPERATIONAL),
        before=seconds(1),
        msg="PCU should be in OPERATIONAL state!",
    )

    # Now PCU is on Operational state

    check(
        pcu.is_state,
        args=(PCU.StateMachine.OPERATIONAL, PCU.Operational_SM.IDLE),
        msg="PCU should be in OPERATIONAL-IDLE state!",
    )

    # Now PCU is on Operational - Idle state

    enable_order = pcu.packets.serialize_packet(500)
    pcu.transmit(enable_order)

    completes(
        wait_until_true(pcu.get_enable),
        before=seconds(1),
        msg="PCU enable should be enabled",
    )

    disable_order = pcu.packets.serialize_packet(501)
    pcu.transmit(disable_order)

    completes(
        wait_until_true(lambda: not pcu.get_enable()),
        before=seconds(1),
        msg="PCU enable should be disabled",
    )

    send_pwm_U_order = pcu.packets.serialize_packet(502, "U", 1000, 90)
    pcu.transmit(send_pwm_U_order)

    completes(
        wait_until_true(pcu.check_pwm),
        before=seconds(1),
        args=(PCU.PWM.U, 1000, 90),
        msg="PWM U is not at 1KHz and 90%% duty",
    )

    check(
        pcu.is_state,
        args=(PCU.StateMachine.OPERATIONAL, PCU.Operational_SM.PWM),
        msg="PCU should be in OPERATIONAL-PWM state!",
    )

    # Now PCU is on Operational - PWM state

    send_pwm_V_order = pcu.packets.serialize_packet(502, "V", 2000, 80)
    pcu.transmit(send_pwm_V_order)

    completes(
        wait_until_true(pcu.check_pwm),
        args=(PCU.PWM.V, 2000, 80),
        before=seconds(1),
        msg="PWM V is not at 2KHz and 80%% duty",
    )

    send_pwm_W_order = pcu.packets.serialize_packet(502, "W", 3000, 70)
    pcu.transmit(send_pwm_W_order)

    completes(
        wait_until_true(pcu.check_pwm),
        args=(PCU.PWM.W, 3000, 70),
        before=seconds(1),
        msg="PWM W is not at 3KHz and 70%% duty",
    )

    stop_pwm_order = pcu.packets.serialize_packet(503)
    pcu.transmit(stop_pwm_order)

    check(
        pcu.is_state,
        args=(PCU.StateMachine.OPERATIONAL, PCU.Operational_SM.IDLE),
        msg="PCU should be in OPERATIONAL-IDLE state!",
    )

    # TODO check pwms are powered off

    # Now PCU is on Operational - Idle state

    pcu.disconnect_gui()

    check(
        pcu.is_state,
        args=(PCU.StateMachine.GENERAL, PCU.General_SM.FAULT),
        msg="PCU should be in FAULT state!",
    )

    # Now PCU is on FAULT state

    check(lambda: not pcu.get_enable(), msg="PCU enable should be disabled")
    # # TODO check PWMs are off


runner.run()  # Runs the tests, do not delete!
