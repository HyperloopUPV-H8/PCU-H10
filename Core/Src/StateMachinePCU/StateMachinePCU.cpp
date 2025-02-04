#include "StateMachinePCU/StateMachinePCU.hpp"


bool StateMachinePCU::space_vector_on = false;
StateMachinePCU::StateMachinePCU(Data_struct *data, Three_Phased_PWM *three_phased,SpaceVector *spVec):
Data(data),
three_phased_pwm(three_phased),
spaceVectorControl(spVec)
{
    stateMachine = new StateMachine(State_PCU::Connecting);
    operationalStateMachine = new StateMachine(Operational_State_PCU::Idle);
    add_states();
    add_transitions();
    add_exit_actions();
}
void StateMachinePCU::start(Communication *comms){
    communication = comms;
    three_phased_pwm->start();
    Time::register_low_precision_alarm(100,[this](){
        //read ADC
        three_phased_pwm->read_ADC();
        communication->send_UDP_packets();
    });
    operationalStateMachine->add_mid_precision_cyclic_action([this](){
        spaceVectorControl->calculate_duties();
    },us(spaceVectorControl->Period),Operational_State_PCU::Accelerating);
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
void StateMachinePCU::add_exit_actions(){
    operationalStateMachine->add_enter_action([this](){
        three_phased_pwm->Led_Commutation.turn_on();
    },State_PCU::Operational);

    stateMachine->add_exit_action([this](){
        three_phased_pwm->stop_all();
        three_phased_pwm->Led_fault.turn_on();
        three_phased_pwm->Led_Commutation.turn_off();
    },State_PCU::Operational);
}
void StateMachinePCU::update(){
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
    if(Communication::received_choose_batteries_type == true){
        Communication::received_choose_batteries_type = false;
        Data->connector_Batteries = Communication::connector_received;
    }
    if(Communication::received_activate_space_vector == true){
        Communication::received_activate_space_vector = false;
        StateMachinePCU::space_vector_on = true;
        three_phased_pwm->set_three_frequencies(Communication::frequency_received);
        spaceVectorControl->set_frequency_Modulation(Communication::frequency_space_vector_received);
        spaceVectorControl->set_target_voltage(Communication::ref_voltage_space_vector_received);

    }else if(Communication::received_stop_space_vector == true){
        Communication::received_stop_space_vector = false;
        StateMachinePCU::space_vector_on = false;
    }
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
    stateMachine->check_transitions();
}
