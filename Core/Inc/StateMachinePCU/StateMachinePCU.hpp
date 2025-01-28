#include "ST-LIB.hpp"
#include "Communication/Communication.hpp"
#include "Three_Phased_PWM/Three_Phased_PWM.hpp"

enum State_PCU{
    Connecting,
    Operational,
    Fault
};
enum Operational_State_PCU{
    Idle,
    Sending_PWM
};
class StateMachinePCU{
    private:
        StateMachine *stateMachine;
        StateMachine *operationalStateMachine;
        Three_Phased_PWM *three_phased_pwm;
        Data_struct *Data;
        Communication *comms;
    public:
        StateMachinePCU(Data_struct *data, Three_Phased_PWM *three_phased);
        void start();
        void add_states();
        void add_transitions();
        void add_exit_actions();
        void add_cyclic_actions();
        void update();

};