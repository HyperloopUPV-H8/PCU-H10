#pragma once
#include "ST-LIB.hpp"
#include "Communication/Communication.hpp"
#include "Data/Data.hpp"
#include "Three_Phased_PWM/Three_Phased_PWM.hpp"
#include  "Sensors/Sensors.hpp"
#include "Control/SpaceVector.hpp"
#include "Control/CurrentControl.hpp"
#include "Control/SpeedControl.hpp"
class StateMachinePCU{
    private:
        StateMachine *stateMachine;
        StateMachine *operationalStateMachine;
        Data_struct *Data;
        Three_Phased_PWM *three_phased_pwm;
        Sensors *sensors;
        Communication *communication;
        SpaceVector *spaceVectorControl;
        CurrentControl *currentControl;
        SpeedControl *speedControl;
    public:
        static bool space_vector_on;
        static bool speed_control;
        StateMachinePCU(Data_struct *data, Three_Phased_PWM *three_phased,Sensors *sensors,SpaceVector *spVec,CurrentControl *current_Control,SpeedControl *speedControl);
        void start(Communication *comms);
        void add_states();
        void add_transitions();
        void add_enter_actions();
        void add_exit_actions();
        void add_cyclic_actions();
        void update();

};