#include "StateMachinePCU/StateMachinePCU.hpp"


StateMachinePCU::StateMachinePCU(Data_struct *data, Three_Phased_PWM *three_phased):
Data(data),
three_phased_pwm(three_phased)
{
    stateMachine = new StateMachine();
    operationalStateMachine = new StateMachine();
    add_states();
    add_transitions();
    add_exit_actions();
}
void StateMachinePCU::start(Communication *comms){
    communication = comms;
    three_phased_pwm->start();

}

void StateMachinePCU::add_states(){
    stateMachine->add_state(State_PCU::Connecting);
    stateMachine->add_state(State_PCU::Operational);
    stateMachine->add_state(State_PCU::Fault);
    operationalStateMachine ->add_state(Operational_State_PCU::Idle);
    operationalStateMachine->add_state(Operational_State_PCU::Sending_PWM);
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
}
void StateMachinePCU::add_exit_actions(){
    stateMachine->add_exit_action([this](){
        three_phased_pwm->stop_all();
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
    if(Communication::received_pwm_order == true){
        Communication::received_pwm_order = false;
        switch (Communication::pwm_received)
        {
            case PWM_ACTIVE::NONE:
                three_phased_pwm->turn_off_active_pwm();
                return;
            case PWM_ACTIVE::U:
                three_phased_pwm->set_frequency_u(Communication::frequency_received);
                three_phased_pwm->set_duty_u(Communication::duty_cycle_received);
                three_phased_pwm->turn_on_u();
            return;
            case PWM_ACTIVE::V:
                three_phased_pwm->set_frequency_v(Communication::frequency_received);
                three_phased_pwm->set_duty_v(Communication::duty_cycle_received);
                three_phased_pwm->turn_on_v();
            return;
            case PWM_ACTIVE::W:
                three_phased_pwm->set_frequency_w(Communication::frequency_received);
                three_phased_pwm->set_duty_w(Communication::duty_cycle_received);
                three_phased_pwm->turn_on_w();
            return;
        }
    }
}
