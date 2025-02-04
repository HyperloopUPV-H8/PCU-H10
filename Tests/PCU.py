from vmcu.services.communications.Socket import Socket
from vmcu.shared_memory import SharedMemory
from vmcu.services.digital_out import DigitalOutService
from vmcu.pin.pinout import Pinout
from vmcu.services.DualPWM import DualPWM
from vmcu.services.communications.Packets import Packets
from enum import IntEnum


class PCU:
    class StateMachine(IntEnum):
        GENERAL = 1
        OPERATIONAL = 2

    class General_SM(IntEnum):
        CONNECTING = 0
        OPERATIONAL = 1
        FAULT = 2

    class Operational_SM(IntEnum):
        IDLE = 0
        PWM = 1

    class PWM(IntEnum):
        U = 0
        V = 1
        W = 2

    def __init__(self):
        self._shm = SharedMemory("gpio_PCU", "state_machine_PCU")
        self._client = Socket("localhost", 50400, "localhost", 50500)
        packet_definitions = {
            500: [],  # enable_buffer
            501: [],  # disable_buffer
            502: ["enum(NONE,U,V,W)", "uint32", "float32"],  # send_pwm
            503: [],  # stop_pwm
        }
        self.packets = Packets(packet_definitions)
        self._enable = DigitalOutService(self._shm, Pinout.PF4)
        self._pwms = {
            PCU.PWM.U: DualPWM(Pinout.PE9, Pinout.PE8, self._shm),
            PCU.PWM.V: DualPWM(Pinout.PE11, Pinout.PE10, self._shm),
            PCU.PWM.W: DualPWM(Pinout.PE13, Pinout.PE12, self._shm),
        }
        print("PCU initialized succesfully")

    def connect_gui(self):
        self._client.connect()

    def disconnect_gui(self):
        self._client.stop()

    def is_state(self, state_machine: StateMachine, state: int) -> bool:
        # print("State machine where are checking: ", state_machine)
        # print("MCU state: ", self._shm.get_state_machine_state(state_machine))
        # print("State it should be: ", state)
        # print("There are ", self._shm.get_state_machine_count(), "state machines")
        current_state = self._shm.get_state_machine_state(state_machine)
        if current_state < 0:
            print(
                "You are trying to get the state machine number",
                state_machine,
                "but there are",
                self._shm.get_state_machine_count(),
                "state machines in the PCU",
            )
        return current_state == state

    def check_pwm(self, pwm: PWM, freq: int, duty: float):
        selected_pwm = self._pwms[pwm]
        return (
            selected_pwm.get_is_on()
            and selected_pwm.get_duty_cycle() == duty
            and selected_pwm.get_frequency() == freq
        )

    def transmit(self, packet):
        self._client.transmit(packet)

    def get_enable(self) -> bool:
        return not self._enable.get_pin_state()
    
    def __del__(self):
        self._client.stop()
        
