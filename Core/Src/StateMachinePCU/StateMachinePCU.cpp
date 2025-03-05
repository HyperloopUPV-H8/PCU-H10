#include "StateMachinePCU/StateMachinePCU.hpp"
#define MODULATION_FREQUENCY_DEFAULT 10

bool StateMachinePCU::space_vector_on = false;
bool StateMachinePCU::speed_control = false;
StateMachinePCU::StateMachinePCU(Data_struct *data, Three_Phased_PWM *three_phased,Sensors *sensors,SpaceVector *spVec,CurrentControl *currentControl,SpeedControl *speedControl):
Data(data),
three_phased_pwm(three_phased),
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
    three_phased_pwm->start();
    sensors->start();
    add_cyclic_actions();
    
}

void StateMachinePCU::add_states(){
    stateMachine->add_state(State_PCU::Operational);
    stateMachine->add_state(State_PCU::Fault);
    operationalStateMachine->add_state(Operational_State_PCU::Sending_PWM);
    operationalStateMachine->add_state(Operational_State_PCU::Accelerating);
    stateMachine->add_state_machine(*operationalStateMachine,State_PCU::Operational);

}
void StateMachinePCU::add_transitions(){

    stateMachine->add_transition(State_PCU::Connecting,State_PCU::Operational,[this](){
        return communication->is_connected();
    });
    stateMachine->add_transition(State_PCU::Operational,State_PCU::Fault,[this](){
        return !communication->is_connected();
    });
    operationalStateMachine->add_transition(Operational_State_PCU::Idle,Operational_State_PCU::Sending_PWM,[this](){
        return (Data->pwm_active != PWM_ACTIVE::NONE && Data->buffer_enable == BUFFER_ENABLE::ON);
    });
    operationalStateMachine->add_transition(Operational_State_PCU::Sending_PWM,Operational_State_PCU::Idle,[this](){
        return (Data->pwm_active == PWM_ACTIVE::NONE || Data->buffer_enable == BUFFER_ENABLE::OFF);
    });
     operationalStateMachine->add_transition(Operational_State_PCU::Idle,Operational_State_PCU::Accelerating,[this](){
        return StateMachinePCU::space_vector_on == true;
    });
    operationalStateMachine->add_transition(Operational_State_PCU::Accelerating,Operational_State_PCU::Idle,[this](){
        return StateMachinePCU::space_vector_on == false;
    });
}
void StateMachinePCU::add_cyclic_actions(){

    Time::register_low_precision_alarm(100,[this](){
        Data->state_pcu = stateMachine->current_state;
        Data->operational_state_pcu = operationalStateMachine->current_state;
    });
    Time::register_mid_precision_alarm(1000, [this](){
        send_upd_data_flag = true;
    });
    Time::register_mid_precision_alarm(200,[this](){
        sensors->read();
    });
    operationalStateMachine->add_mid_precision_cyclic_action(
        [this](){
            execute_5khz_accelerating_flag = true;
        },us(Current_Control_Data::microsecond_period),Operational_State_PCU::Accelerating);

    operationalStateMachine->add_mid_precision_cyclic_action([this](){
        if(speed_control){
            speedControl->control_action();
        }
    },us(Speed_Control_Data::microsecond_period),Operational_State_PCU::Accelerating);
}
void StateMachinePCU::add_enter_actions(){
    operationalStateMachine->add_enter_action([this](){
        three_phased_pwm->Led_Commutation.turn_on();
        three_phased_pwm->enable();
        three_phased_pwm->Enable_reset();
    },Operational_State_PCU::Accelerating);

    operationalStateMachine->add_enter_action([this](){
        sensors->currentSensors.zeroing();
    },State_PCU::Operational);

}
void StateMachinePCU::add_exit_actions(){
    stateMachine->add_exit_action([this](){
        three_phased_pwm->stop_all();
        three_phased_pwm->Led_fault.turn_on();
        three_phased_pwm->Led_Commutation.turn_off();
    },State_PCU::Operational);
    
    operationalStateMachine->add_exit_action([this](){
        three_phased_pwm->Led_Commutation.turn_off();
        three_phased_pwm->stop_all();
    },Operational_State_PCU::Accelerating);
}
void StateMachinePCU::update(){
    #if TEST_PWM
        if(Communication::received_disable_buffer == true){
            Communication::received_disable_buffer = false;
            three_phased_pwm->disable();
        }
        if(Communication::received_enable_buffer == true){
            Communication::received_enable_buffer = false;
            three_phased_pwm->enable();
        }
        if(Communication::received_stop_pwm_order == true){
            Communication::received_stop_pwm_order = false;
            three_phased_pwm->stop_all();
        }
        if(Communication::received_disable_reset == true){
            Communication::received_disable_reset = false;
            three_phased_pwm->Disable_reset();
        }
        else if(Communication::received_enable_reset == true){
            Communication::received_enable_reset = false;
            three_phased_pwm->Enable_reset();
        }
    #endif

    if(Communication::received_activate_space_vector == true){
        Communication::received_activate_space_vector = false;
        StateMachinePCU::space_vector_on = true;
        three_phased_pwm->set_three_frequencies(Communication::frequency_received);
        spaceVectorControl->set_frequency_Modulation(Communication::frequency_space_vector_received);
        spaceVectorControl->set_VMAX(Communication::Vmax_control_received);
        spaceVectorControl->set_target_voltage(Communication::ref_voltage_space_vector_received);
        currentControl->stop();
    }
    else if(Communication::received_stop_space_vector == true){
        Communication::received_stop_space_vector = false;
        StateMachinePCU::space_vector_on = false;
        StateMachinePCU::speed_control = false;
        currentControl->stop();
        speedControl->reset_PI();
    }
    if(Communication::received_Precharge_order == true){
        Communication::received_Precharge_order = false;
        StateMachinePCU::space_vector_on = true;
        three_phased_pwm->set_three_frequencies(Communication::frequency_received);
        spaceVectorControl->set_frequency_Modulation(MODULATION_FREQUENCY_DEFAULT);
        spaceVectorControl->set_VMAX(Communication::Vmax_control_received);
        spaceVectorControl->set_target_voltage(0); //in precharge the target_voltage must to be 0
        currentControl->stop();
    }
    if(Communication::received_Current_reference_order == true){
        currentControl->change_mode(ControlStates::accelerate);
        Communication::received_Current_reference_order = false;
        speed_control = false;
        spaceVectorControl->set_VMAX(Communication::Vmax_control_received);
        currentControl->set_current_ref(Communication::current_reference_received);
        three_phased_pwm->set_three_frequencies(Communication::frequency_received);
        spaceVectorControl->set_frequency_Modulation(Communication::frequency_space_vector_received);
        StateMachinePCU::space_vector_on = true;
        currentControl->start();
    }
    if(Communication::received_Speed_reference_order == true){
        currentControl->change_mode(ControlStates::accelerate);
        speedControl->change_mode(ControlStates::accelerate);
        Communication::received_Speed_reference_order = false;
        speedControl->change_mode(ControlStates::accelerate);
        currentControl->change_mode(ControlStates::accelerate);
        StateMachinePCU::space_vector_on = true;
        StateMachinePCU::speed_control = true;
        currentControl->start();
        speedControl->set_reference_speed(Communication::speed_reference_received);
        speedControl->set_reference_speed(Communication::speed_reference_received);
        three_phased_pwm->set_three_frequencies(Communication::frequency_received);
    }
    if(Communication::received_Complete_Run_order == true){
        Communication::received_Complete_Run_order = false;
        speedControl->change_mode(ControlStates::regenerate);
        currentControl->change_mode(ControlStates::regenerate);
        StateMachinePCU::space_vector_on = true;
        StateMachinePCU::speed_control = true;
        currentControl->start();
        speedControl->set_reference_speed(Communication::speed_reference_received);
        speedControl->set_reference_speed(Communication::speed_reference_received);
        three_phased_pwm->set_three_frequencies(Communication::frequency_received);
    }
    if(Communication::received_zeroing_order == true){
        Communication::received_zeroing_order = false;
        sensors->currentSensors.zeroing();
    }
    #if TEST_PWM
        if(Communication::received_pwm_order == true){
            Communication::received_pwm_order = false;
            three_phased_pwm->turn_off_active_pwm();
            switch (Communication::pwm_received)
            {
                case PWM_ACTIVE::NONE:
                    break;
                case PWM_ACTIVE::U:
                    three_phased_pwm->set_frequency_u(static_cast<uint32_t>(Communication::frequency_received));
                    three_phased_pwm->set_duty_u(Communication::duty_cycle_received);
                    three_phased_pwm->turn_on_u();
                    break;
                case PWM_ACTIVE::V:
                    three_phased_pwm->set_frequency_v(static_cast<uint32_t>(Communication::frequency_received));
                    three_phased_pwm->set_duty_v(Communication::duty_cycle_received);
                    three_phased_pwm->turn_on_v();
                    break;
                case PWM_ACTIVE::W:
                    three_phased_pwm->set_frequency_w(static_cast<uint32_t>(Communication::frequency_received));
                    three_phased_pwm->set_duty_w(Communication::duty_cycle_received);
                    three_phased_pwm->turn_on_w();  
                    break;
            }
        }
    #endif
    if(execute_5khz_accelerating_flag){
        execute_5khz_accelerating_flag = false;
        spaceVectorControl->calculate_duties();
        currentControl->control_action();
    }
    if(send_upd_data_flag){
        send_upd_data_flag = false;
        communication->send_UDP_packets();
    }
    stateMachine->check_transitions();
}
