from vmcu.services.communications.Socket import Socket
from vmcu.shared_memory import SharedMemory
from vmcu.services.digital_out import DigitalOutService
from vmcu.pin.pinout import Pinout
from vmcu.services.DualPWM import DualPWM
from vmcu.services.communications.Packets import Packets
from enum import Enum
import enum


class PCU:
    class StateMachine(Enum):
        GENERAL = 1
        OPERATIONAL = 2

    class General_SM(Enum):
        CONNECTING = 0
        OPERATIONAL = 1
        FAULT = 2

    class Operational_SM(Enum):
        IDLE = 0
        PWM = 1

    class PWM(Enum):
        U = 0
        V = 1
        W = 2

    def __init__(self):
        self._shm = SharedMemory("gpio_PCU", "state_machine_PCU")
        self._client = Socket("192.168.0.9", 50400, "192.168.0.5", 50500)
        packet_definitions = {
            500: [],  # enable_buffer
            501: [],  # disable_buffer
            502: [enum("NONE", "U", "V", "W"), "uint32", "float32"],  # send_pwm
            503: [],  # stop_pwm
        }
        self.packets = Packets(packet_definitions)
        self._enable = DigitalOutService(self.shm, Pinout.PF4)
        self._pwmU = DualPWM(Pinout.PE9, Pinout.PE8)
        self._pwmV = DualPWM(Pinout.PE11, Pinout.PE10)
        self._pwmW = DualPWM(Pinout.PE13, Pinout.PE12)
        self._pwms = {
            PCU.PWM.U: self._pwmU,
            PCU.PWM.V: self._pwmV,
            PCU.PWM.W: self._pwmW,
        }
        self._mcu = None

    def connect_gui(self):
        self._client.connect()

    def disconnect_gui(self):
        self._client.stop()

    def is_state(self, state_machine: StateMachine, state: int) -> bool:
        self._shm.get_state_machine_state(state_machine) == state

    def check_pwm(self, pwm: PWM, freq: int, duty: float):
        selected_pwm = self._pwms[pwm]
        return (
            selected_pwm.get_is_on()
            and selected_pwm.get_duty_cycle() == duty
            and selected_pwm.get_frequency() == freq
        )

    def transmit(self, packet):
        self._server.transmit(packet, self._mcu)

    def get_enable(self) -> bool:
        return not self._enable.get_pin_state
