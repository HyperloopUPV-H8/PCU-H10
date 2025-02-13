#include "Control/CurrentControl.hpp"
#define NANOSECOND 1000000000

CurrentControl::CurrentControl(Data_struct *Data,SpaceVector *spaceVector):
Data(Data),
spaceVector(spaceVector)
{
    current_PI.reset();
}
void CurrentControl::set_current_ref(float cur_ref){
    current_ref = cur_ref;
}
float CurrentControl::get_current_ref(){
    return current_ref;
}
double CurrentControl::calculate_peak(){
    double tiempo = static_cast<double>(Time::get_global_tick())/NANOSECOND;
    double maximo;
    static double antiguo_maximo = 0.0;
    static double t_maximo = 0.0;
    static double posible_nuevo_maximo = 0.0;
    static double posible_t_maximo = 0.0;

   double period = 1/(spaceVector->get_modulation_frequency());
   //Max current will be the max between the actual values and the value of the last measure
   double max_current = std::max({Data->actual_current_sensor_u_a, Data->actual_current_sensor_u_b,
                            Data->actual_current_sensor_v_a,Data->actual_current_sensor_v_b,
                            Data->actual_current_sensor_w_a,Data->actual_current_sensor_w_b}); 
                            
    if(max_current >= antiguo_maximo){
        maximo = max_current;
        antiguo_maximo = max_current;
        t_maximo = tiempo;
        posible_nuevo_maximo = 0.0;
        posible_t_maximo = tiempo;
    }else{
        maximo = antiguo_maximo;
        if(tiempo > (t_maximo + 0.1*period)){
            if(max_current > posible_nuevo_maximo){
                posible_nuevo_maximo = max_current;
                posible_t_maximo = tiempo;
            }
        }
        if(tiempo > (t_maximo + 0.6*period)){
            maximo = posible_nuevo_maximo;
            antiguo_maximo = posible_nuevo_maximo;
            posible_nuevo_maximo = 0;
            t_maximo = posible_t_maximo;
        }
    }
    return maximo;
   

}
void CurrentControl::control_action(){
    double current_peak = calculate_peak();
    double current_error = current_ref - current_peak;
    current_PI.input(current_error);
    current_PI.execute();
    spaceVector->set_target_voltage(current_PI.output_value);
}