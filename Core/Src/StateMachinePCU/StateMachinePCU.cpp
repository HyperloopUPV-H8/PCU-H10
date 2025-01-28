#include "StateMachinePCU/StateMachinePCU.hpp"


StateMachinePCU::StateMachinePCU(Data_struct *data, Three_Phased_PWM *three_phased):
Data(data),
three_phased_pwm(three_phased)
{
    stateMachine = new StateMachine();
    operationalStateMachine = new StateMachine();
    add_states();
    add_transitions();
    add_e
}
void StateMachinePCU::start(comms){
    comms = new Communication(Data);
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
        return comms->is_connected();
    });
    stateMachine->add_transition(State_PCU::Operational,State_PCU::Fault,[this](){
        return !comms->is_connected();
    });
    operationalStateMachine->add_transition(Operational_State_PCU::Idle,Operational_State_PCU::Sending_PWM,[this](){
        return Data->pwm_active != PWM_ACTIVE::NONE;
    });
    operationalStateMachine->add_transition(Operational_State_PCU::Sending_PWM,Operational_State_PCU::Idle,[this](){
        return Data->pwm_active == PWM_ACTIVE::NONE;
    });
}
void StateMachinePCU::add_exit_actions(){
    stateMachine->add_exit_action([this](){
        three_phased_pwm->stop_all();
    },State_PCU::Operational);
}
void StateMachinePCU::add_cyclic_actions(){
    stateMachine->add_low_precision_cyclic_action(&update,500ms,State_PCU::Operational);
}
void StateMachinePCU::update(){
    if(Communication::duty_cycle_received == true)
}
