#pragma once
#include "ST-LIB.hpp"
#include "Communication/Communication.hpp"
#include "Data/Data.hpp"
#include "Actuators/Actuators.hpp"
#include  "Sensors/Sensors.hpp"
#include "Control/SpaceVector.hpp"
#include "Control/CurrentControl.hpp"
#include "Control/SpeedControl.hpp"
class StateMachinePCU{
    private:
        StateMachine *stateMachine;
        StateMachine *operationalStateMachine;
        Data_struct *data;
        Actuators *actuators;
        Sensors *sensors;
        Communication *communication;
        SpaceVector *spaceVectorControl;
        CurrentControl *currentControl;
        SpeedControl *speedControl;
        bool execute_current_control_flag{false};
        bool execute_space_vector_control_flag{false};
        bool send_udp_data_flag{false};
        bool execute_speed_control_flag{false};
    public:
        static bool space_vector_on;
        static bool speed_control_active;
        StateMachinePCU(Data_struct *data, Actuators *actuators,Sensors *sensors,SpaceVector *spVec,CurrentControl *current_Control,SpeedControl *speedControl);
        void start(Communication *comms);
        void add_states();
        void add_transitions();
        void add_enter_actions();
        void add_exit_actions();
        void add_cyclic_actions();
        void update();
        void Motor_Stop();
};