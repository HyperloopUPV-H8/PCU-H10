#include "Control/Runs.hpp"


void RUNS::init(Data_struct *d)
{
    Data = d;
}

float RUNS::update()
{

    double vel = Data->speed_km_h_encoder;
    double pos = Data->position_encoder;
    if(Data->state_run == RunState::BRAKING)
    {
        if(vel <= 0.1)
        {
            Communication::received_stop_motor = true;
        }
        return 0.0;
    }

    vel_ref = 35.0F;

    if(pos > BRAKING_DISTANCE)
    {
        Communication::received_motor_brake_order = true;
        Data->state_run = RunState::BRAKING;
        vel_ref = 0.0;
    }
    if(vel > vel_ref)
    {
        vel_ref = 0.0;
    }

    return vel_ref;

}

void RUNS::start(uint8_t run_id)
{
    run = (RunMode)run_id;
    Data->state_run = RunState::MOVING;
}