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
        vel_ref = 50;
        switch(curr_state)
        {
            case 0:
                if(time_window_flag)
                {
                    Data->next_state = 2;
                    vel_ref = vel_ref;
                }
                break;
        
            case 2:
                if(pos > 6)
                {
                    Data->next_state = 4;
                }
                vel_ref = vel_ref;
                break;
        
        //Velocidad Crucero
            case 4:
                if(pos > 48 - distance_brake  ||   vel > vel_ref)
                {        //En principio no debería hacer falta "vel > vel_ref" y el control debería mantenerse en crucero
                    Data->next_state = 7;
                }   
                vel_ref = vel_ref;
                break;
    
        //Realmente frenado mecánico
            case 7:                                                               //Estado inventado (como todos) que activa el frenado mecánico
                if(vel < 1)
                {
                    Data->next_state = 0;
                }
                vel_ref = 0;
                break;
        }
    }

    //Booster
    else if(run == 2)
    {
        switch(curr_state)
        {
            case 0:
                if(time_window_flag)
                {
                    Data->next_state = 1;
                    vel_ref = 0;
                }
                break;
            
            case 1:
                if(pos > 4.5)
                {
                    Data->next_state = 4;
                }
                vel_ref = vel_ref;
                break;
        
                //Velocidad Crucero
            case 4:
                if(pos > 48 - distance_brake  ||   vel > 30)
                {
                    Data->next_state = 5;
                }
                vel_ref = vel_ref;
                break;
    
            case 5:
                if(vel <= 0)
                {
                    Data->next_state = 0;
                }
                vel_ref = 0;
                break;
        }
    }

    //Tubo
    else if(run == 3)
    {
        vel_ref = 5;
            
        switch(curr_state)
        {
            case 0:
                if(time_window_flag)
                {
                    Data->next_state = 4;
                    vel_ref = vel_ref;
                }
                break;
    
            //Velocidad Crucero
            case 4:
                if(pos > 7)
                {     //Frena 3 metros antes de los 10
                    Data->next_state = 6;
                }
                vel_ref = vel_ref;
                break;
    
            case 6:
                if(pos < 3)
                {
                    Data->next_state = 0;
                }
                vel_ref = -vel_ref;
                break;
        }
    }

    //LIM
    else if(run == 4)
    {
        vel_ref = 35;
            
        switch(curr_state)
        {
            case 0:
                if(time_window_flag)
                {
                    Data->next_state = 2;
                    vel_ref = vel_ref;
                }
                break;
        
            case 2:
                if(vel > vel_ref)
                {
                    Data->next_state = 4;
                }
                vel_ref = vel_ref;
                break;
        
        //Velocidad Crucero
            case 4:
                if(pos > 48 - distance_brake  ||   vel > vel_ref)
                {
                    Data->next_state = 5;
                }
                vel_ref = vel_ref;
                break;
            case 5:
                if(vel <= 0)
                {
                    Data->next_state = 0;
                }
                vel_ref = 0;
                break;
    
        //Realmente frenado mecánico
            case 7:                                                               //Estado inventado (como todos) que activa el frenado mecánico
                if(vel < 1)
                {
                    Data->next_state = 0;
                }
                vel_ref = 0;
                break;
        }
    }

    //LIM (50 km/h)
    else if(run == 5)
    {
        vel_ref = 50;
    
        switch(curr_state)
        {
            case 0:
                if(time_window_flag)
                {
                    Data->next_state = 2;
                    vel_ref = vel_ref;
                }
                break;
            
            case 2:
                if(vel > vel_ref && pos > 48-distance_brake)
                {
                    Data->next_state = 5;
                }
                vel_ref = vel_ref;
                break;
    
            case 5:        
                if(vel <= 0)
                {            
                    Data->next_state = 0;
                }                                                                                                                              //end
                vel_ref = 0;
                break;
        }
    }

    //LIM + Booster (70 km/h)
    else if(run == 6)
    {
        vel_ref = 70;
            
        switch(curr_state)
        {
            case 0:
                if(time_window_flag)
                {
                    Data->next_state = 2;
                    vel_ref = vel_ref;
                }
        
            case 2:
                if(pos > 6)
                {
                    Data->next_state = 4;
                }
                vel_ref = vel_ref;
                break;
        
        //Velocidad Crucero
            case 4:
                if(pos > 100 - distance_brake_100  ||   vel > vel_ref)
                {        
                    Data->next_state = 7;
                }
                vel_ref = vel_ref;
                break;
    
        //Realmente frenado mecánico
            case 7:                                                               
                if(vel < 1)
                {
                    Data->next_state = 0;
                }
                vel_ref = 0;
                break;
        }
    }

    //LIM + Booster (100 km/h)
    else if(run == 7)
    {
        vel_ref = 100;
            
        switch(curr_state)
        {
            case 0:
                if(time_window_flag)
                {
                    Data->next_state = 2;
                    vel_ref = vel_ref;
                }
                break;
        
            case 2:
                if(pos > 6)
                {
                    Data->next_state = 4;
                }
                vel_ref = vel_ref;
                break;
        
        //Velocidad Crucero
            case 4:
                if(pos > 100 - distance_brake_100  ||   vel > vel_ref)
                {        
                    Data->next_state = 7;
                }
                vel_ref = vel_ref;
                break;
    
        //Realmente frenado mecánico
            case 7:                                                               
                if(vel < 1)
                {
                    Data->next_state = 0;
                }
                vel_ref = 0;
                break;
        }
    }

    //frenado Regenerativo
    else if(run == 8)
    {
        vel_ref = 30;
    
        switch(curr_state)
        {
         case 0:
            if(time_window_flag)
            {
                Data->next_state = 2;
            }
            vel_ref = 0;
            break;

        case 2:
            if(vel > vel_ref)
            {
                Data->next_state = 9;
            }
            else
            {
                vel_ref = vel_ref;
            }
            break;

        case 9:
            if(pos < 48-distance_brake)            //si la velocidad del vehículo supera a la deseada y antes de la distancia límite de frenar mecánicamente
            {
                vel_ref = 0;
            }

            if(pos > 48 - distance_brake)
            {
                Data->next_state = 7;
            }
            break;

         case 7:
            if(vel <= 0)
            {
                Data->next_state = 0;
            }
            vel_ref = 0;
            break;
        }
    }

    return vel_ref;

}

void RUNS::start(uint8_t run_id)
{
    run = run_id;

    (run != 2) ? Data->next_state = 0 : Data->next_state = 1;

    Time::set_timeout(1000, [&]()
    {
        time_window_flag = true;
        Time::set_timeout(1000, [&]()
        {
          time_window_flag = false;  
        });
    });
}