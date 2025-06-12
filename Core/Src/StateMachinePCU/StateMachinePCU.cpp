#include "StateMachinePCU/StateMachinePCU.hpp"
#define MODULATION_FREQUENCY_DEFAULT 10
#define Protecction_Voltage 420.0
StateMachinePCU::StateMachinePCU(Data_struct *data, Actuators *actuators,Sensors *sensors,SpaceVector *spVec,CurrentControl *currentControl,SpeedControl *speedControl):
data(data),
actuators(actuators),
sensors(sensors),
spaceVectorControl(spVec),
currentControl(currentControl),
speedControl(speedControl)
{
    stateMachine = new StateMachine(State_PCU::Connecting);
    operationalStateMachine = new StateMachine(Operational_State_PCU::Idle);
    add_states();
    add_enter_actions();
    add_transitions();
    add_exit_actions();
}
void StateMachinePCU::start(Communication *comms){
    communication = comms;
    actuators->start();
    sensors->start();
    add_cyclic_actions();
    
}

void StateMachinePCU::add_states(){
    stateMachine->add_state(State_PCU::Operational);
    stateMachine->add_state(State_PCU::Fault);
    operationalStateMachine->add_state(Operational_State_PCU::Braked);
    operationalStateMachine->add_state(Operational_State_PCU::Accelerating);
    operationalStateMachine->add_state(Operational_State_PCU::Regenerative);
    stateMachine->add_state_machine(*operationalStateMachine,State_PCU::Operational);

    //protections
    ProtectionManager::link_state_machine(*stateMachine,State_PCU::Fault);
    add_protection(&data->actual_voltage_battery_A,Boundary<float,ABOVE>(Protecction_Voltage));
    add_protection(&data->actual_voltage_battery_B,Boundary<float,ABOVE>(Protecction_Voltage));
}
void StateMachinePCU::add_transitions(){
//transitions generals
    stateMachine->add_transition(State_PCU::Connecting,State_PCU::Operational,[this](){
        return communication->is_connected();
    });
    stateMachine->add_transition(State_PCU::Connecting,State_PCU::Fault,[this](){
        return data->actual_voltage_battery_A > Protecction_Voltage || data->actual_voltage_battery_B >Protecction_Voltage;
    });
    stateMachine->add_transition(State_PCU::Operational,State_PCU::Fault,[this](){
        return !communication->is_connected() || data->actual_voltage_battery_A > Protecction_Voltage || data->actual_voltage_battery_B >Protecction_Voltage;
    });

    //Braked
    operationalStateMachine->add_transition(Operational_State_PCU::Idle,Operational_State_PCU::Braked,[this](){
        return sensors->reeds_braking();
    });
    operationalStateMachine->add_transition(Operational_State_PCU::Accelerating,Operational_State_PCU::Braked,[this](){
        return sensors->reeds_braking();
    });
    operationalStateMachine->add_transition(Operational_State_PCU::Regenerative,Operational_State_PCU::Braked,[this](){
        return sensors->reeds_braking();
    });
    //Unbraked
    operationalStateMachine->add_transition(Operational_State_PCU::Braked,Operational_State_PCU::Idle,[this](){
        return !sensors->reeds_braking();
    });
    //Pass to regenerative mode
    operationalStateMachine->add_transition(Operational_State_PCU::Accelerating,Operational_State_PCU::Regenerative,[this](){
        return data->speedState == ControlStates::regenerate;
    });
    operationalStateMachine->add_transition(Operational_State_PCU::Regenerative,Operational_State_PCU::Accelerating,[this](){
        return data->speedState == ControlStates::accelerate;
    });
    //Pass to idle if the space vector is off

    operationalStateMachine->add_transition(Operational_State_PCU::Idle,Operational_State_PCU::Accelerating,[this](){
        return data->space_vector_active == true;
    });
    operationalStateMachine->add_transition(Operational_State_PCU::Accelerating,Operational_State_PCU::Idle,[this](){
        return data->space_vector_active == false;
    });
    operationalStateMachine->add_transition(Operational_State_PCU::Regenerative,Operational_State_PCU::Idle,[this](){
        return data->space_vector_active == false || currentControl->running == false;
    });
}

void StateMachinePCU::add_cyclic_actions(){
    Time::register_low_precision_alarm(100,[this](){
        data->state_pcu = stateMachine->current_state;
        data->operational_state_pcu = operationalStateMachine->current_state;
    });
    Time::register_mid_precision_alarm(1000, [this](){
        send_udp_data_flag = true;
    });
    Time::register_mid_precision_alarm(Sensors_data::read_sensors_us,[this](){
        sensors->read();
        sensors->read_speetec();
    });
    //current control
    operationalStateMachine->add_mid_precision_cyclic_action(
        [this](){ 
            execute_space_vector_control_flag = true;
            if(currentControl->running){
                execute_current_control_flag = true;
                
            }
        },us(Current_Control_Data::microsecond_period),Operational_State_PCU::Accelerating);
    
    operationalStateMachine->add_mid_precision_cyclic_action(
        [this](){
          execute_current_control_flag = true;
          execute_space_vector_control_flag = true;
          execute_speed_control_flag = true;
        },us(Current_Control_Data::microsecond_period),Operational_State_PCU::Regenerative);
    //speed control 
    operationalStateMachine->add_mid_precision_cyclic_action(
        [this](){ 
            if(speedControl->running){
                execute_speed_control_flag = true;
            }
        },us(Current_Control_Data::microsecond_period),Operational_State_PCU::Accelerating);

    operationalStateMachine->add_mid_precision_cyclic_action([this](){
        execute_speed_control_flag = true;
    },us(Speed_Control_Data::microsecond_period),Operational_State_PCU::Regenerative);

  
}

void StateMachinePCU::add_enter_actions(){
    operationalStateMachine->add_enter_action([this](){
        actuators->Led_Commutation.turn_on();
        actuators->enable();
        actuators->Enable_reset();
    },Operational_State_PCU::Accelerating);
    
    operationalStateMachine->add_enter_action([this](){
       Motor_Stop(); //just for safety reasons

    },Operational_State_PCU::Idle);

    stateMachine->add_enter_action([this](){
        sensors->currentSensors.zeroing();
        actuators->Led_Operational.turn_on();
    },State_PCU::Operational);

    stateMachine->add_enter_action([this]() {
           Motor_Stop();
           actuators->Led_fault.turn_on();
    },State_PCU::Fault);
    
    operationalStateMachine->add_enter_action([this](){
        Motor_Stop();
    },Operational_State_PCU::Braked);
}
void StateMachinePCU::add_exit_actions(){
    //exit from operational
    stateMachine->add_exit_action([this](){
        Motor_Stop();
        actuators->Led_fault.turn_on();
        actuators->Led_Commutation.turn_off();
        actuators->Led_Operational.turn_off();
    },State_PCU::Operational);
}
void StateMachinePCU::update(){
    #if TEST_PWM
        if(Communication::received_disable_buffer == true){
            Communication::received_disable_buffer = false;
            actuators->disable();
        }
        if(Communication::received_enable_buffer == true){
            Communication::received_enable_buffer = false;
            actuators->enable();
        }
        if(Communication::received_stop_pwm_order == true){
            Communication::received_stop_pwm_order = false;
            actuators->stop_all();
        }
        if(Communication::received_disable_reset == true){
            Communication::received_disable_reset = false;
            actuators->Disable_reset();
        }
        else if(Communication::received_enable_reset == true){
            Communication::received_enable_reset = false;
            actuators->Enable_reset();
        }
    #endif

    if(Communication::received_activate_space_vector == true){
        Communication::received_activate_space_vector = false;
        data->space_vector_active = true;
        actuators->set_three_frequencies(Communication::frequency_received);
        spaceVectorControl->set_frequency_Modulation(Communication::frequency_space_vector_received);
        spaceVectorControl->set_VMAX(Communication::Vmax_control_received);
        spaceVectorControl->set_target_voltage(Communication::ref_voltage_space_vector_received);
        currentControl->stop();
        speedControl->stop();
    }
    else if(Communication::received_stop_motor == true){
        Communication::received_stop_motor = false;
        Motor_Stop();
    }
    if(Communication::received_Precharge_order == true){
        Communication::received_Precharge_order = false;
        actuators->set_three_frequencies(Communication::frequency_received);
        spaceVectorControl->set_frequency_Modulation(MODULATION_FREQUENCY_DEFAULT);
        spaceVectorControl->set_VMAX(Communication::Vmax_control_received);
        spaceVectorControl->set_target_voltage(0); //in precharge the target_voltage must to be 0
        //actions
        currentControl->stop();
        data->space_vector_active = true;
    }
    if(Communication::received_Current_reference_order == true){
        Communication::received_Current_reference_order = false;

        speedControl->reset_PI();
        spaceVectorControl->set_VMAX(Communication::Vmax_control_received);
        currentControl->set_current_ref(Communication::current_reference_received);
        actuators->set_three_frequencies(Communication::frequency_received);
        spaceVectorControl->set_frequency_Modulation(Communication::frequency_space_vector_received);

        data->space_vector_active = true;
        speedControl->stop();
        currentControl->start();
    }
    if(Communication::received_Speed_reference_order == true){ 
        Communication::received_Speed_reference_order = false;
        speedControl->reset_PI();
        speedControl->set_reference_speed(Communication::speed_reference_received);
        actuators->set_three_frequencies(Communication::frequency_received);
        spaceVectorControl->set_VMAX(Communication::Vmax_control_received);
        //actions
        if(data->speedState != ControlStates::regenerate){
            speedControl->change_mode(ControlStates::accelerate); 
        }
        data->space_vector_active = true;
        currentControl->start();
        speedControl->start();   
    }
    if(Communication::received_motor_brake_order == true){
        Communication::received_motor_brake_order = false;
        speedControl->reset_PI();
        //hardcoded almost everything 
        speedControl->set_reference_speed(0);
        data->Stablished_direction = Direction::BACKWARDS;
        actuators->set_three_frequencies(10000);
        spaceVectorControl->set_VMAX(Communication::Vmax_control_received);
        //ACTIONS
        data->space_vector_active = true;
        currentControl->start();
        speedControl->start();

    }
    if(Communication::received_start_regenerative_now_order == true){
        Communication::received_start_regenerative_now_order = false;
        if(!speedControl->running) return;
    
        speedControl->change_mode(ControlStates::regenerate);
    }
    if(Communication::received_Complete_Run_order == true){
        Communication::received_Complete_Run_order = false;

        speedControl->set_reference_speed(Communication::speed_reference_received);
        actuators->set_three_frequencies(Communication::frequency_received);
        spaceVectorControl->set_VMAX(Communication::Vmax_control_received);
        //actions
        /*This should be changed in a future*/
        speedControl->change_mode(ControlStates::regenerate);
        data->space_vector_active = true;
        currentControl->start();
        speedControl->start();
    }
    if(Communication::received_zeroing_order == true){
        Communication::received_zeroing_order = false;
        sensors->currentSensors.zeroing();
    }
    #if TEST_PWM
        if(Communication::received_pwm_order == true){
            Communication::received_pwm_order = false;
            actuators->turn_off_active_pwm();
            switch (Communication::pwm_received)
            {
                case PWM_ACTIVE::NONE:
                    break;
                case PWM_ACTIVE::U:
                    actuators->set_frequency_u(static_cast<uint32_t>(Communication::frequency_received));
                    actuators->set_duty_u(Communication::duty_cycle_received);
                    actuators->turn_on_u();
                    break;
                case PWM_ACTIVE::V:
                    actuators->set_frequency_v(static_cast<uint32_t>(Communication::frequency_received));
                    actuators->set_duty_v(Communication::duty_cycle_received);
                    actuators->turn_on_v();
                    break;
                case PWM_ACTIVE::W:
                    actuators->set_frequency_w(static_cast<uint32_t>(Communication::frequency_received));
                    actuators->set_duty_w(Communication::duty_cycle_received);
                    actuators->turn_on_w();  
                    break;
            }
        }
    #endif
    if(execute_space_vector_control_flag){
        execute_space_vector_control_flag = false;
        spaceVectorControl->calculate_duties();
    }
    if(execute_current_control_flag){
        execute_current_control_flag = false;
        currentControl->control_action();
    }
    if(execute_speed_control_flag){
        execute_speed_control_flag = false;
        speedControl->control_action();
    }
    if(send_udp_data_flag){
        send_udp_data_flag = false;
        communication->send_UDP_packets();
    }
    stateMachine->check_transitions();
    operationalStateMachine->check_transitions();
}

void StateMachinePCU::Motor_Stop(){ //This may be rebundance but Safety Reasons
    actuators->stop_all();
    data->space_vector_active = false;
    execute_current_control_flag = false;
    execute_space_vector_control_flag = false;
    execute_speed_control_flag = false;
    currentControl->stop();
    speedControl->stop();
}