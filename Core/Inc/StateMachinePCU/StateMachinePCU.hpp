#pragma once
#include "ST-LIB.hpp"
#include "Communication/Communication.hpp"
#include "Data/Data.hpp"
#include "Three_Phased_PWM/Three_Phased_PWM.hpp"
#include  "Sensors/Sensors.hpp"
#include "Control/SpaceVector.hpp"


class StateMachinePCU{
    private:
        StateMachine *stateMachine;
        StateMachine *operationalStateMachine;
        Data_struct *Data;
        Three_Phased_PWM *three_phased_pwm;
        Sensors *sensors;
        Communication *communication;
        SpaceVector *spaceVectorControl;
    public:
        static bool space_vector_on;
        StateMachinePCU(Data_struct *data, Three_Phased_PWM *three_phased,Sensors *sensors,SpaceVector *spVec);
        void start(Communication *comms);
        void add_states();
        void add_transitions();
        void add_exit_actions();
        void add_cyclic_actions();
        void update();

};