#include "Control/Runs.hpp"


void RUNS::init(Data_struct *d)
{
    Data = d;
}

float RUNS::update()
{
    curr_state = Data->next_state;

    double vel = Data->speed_km_h_encoder;
    double pos = Data->position_encoder;

    //LIM + BOOSTER
    if(run == 1)
    {

        if(pos > 48 - distance_brake  ||   vel > vel_ref)
        {        //En principio no debería hacer falta "vel > vel_ref" y el control debería mantenerse en crucero
            vel_ref = 0.0F;
        }
        else
        {
            vel_ref = 50.0F;
        }
    }

    //Booster
    else if(run == 2)
    {

        if(pos > 48 - distance_brake  ||   vel > 30)
        {
            vel_ref = 0.0F;
        }
        else
        {
            vel_ref = 25.0F;
        }
    }

    //LIM
    else if(run == 4)
    {
        if(pos > 48 - distance_brake  ||   vel > vel_ref)
        {
            vel_ref = 0.0F;
        }
        else
        {
            vel_ref = 35.0F;
        }
    }

    //LIM (50 km/h)
    else if(run == 5)
    {
        if(vel > vel_ref && pos > 48-distance_brake)
        {
            vel_ref = 0.0F;
        }
        else
        {
            vel_ref = 50.0F;
        }
    }

    return vel_ref;

}

void RUNS::start(uint8_t run_id)
{
    run = run_id;
}