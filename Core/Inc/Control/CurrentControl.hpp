#pragma once
#include "Data/Data.hpp"
#include "SpaceVector.hpp"

#define NANOSECOND 1000000000
class Max_Peak{
    private:
        double antiguo_maximo = 0.0;
        double t_maximo = 0.0;
        double posible_nuevo_maximo = 0.0;
        double posible_t_maximo = 0.0;
        float *current_sensor;
        static double modulation_frequency; 
    public:
        static void set_modulation_freq(double modulation_freq){
            modulation_frequency = modulation_freq;
        }
        Max_Peak(float &curr_sensor): current_sensor(&curr_sensor){}
        
        double calculate_Max_Peak(){
            double tiempo = static_cast<double>(Time::get_global_tick())/NANOSECOND;
            double maximo;

            double period = 1.0 / modulation_frequency;

            double max_current = std::abs(*current_sensor);
                                    
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
};
class CurrentControl{
private:
    float current_ref;
    PI<IntegratorType::Trapezoidal> current_PI{Current_Control_Data::kp_accelerate,Current_Control_Data::ki_accelerate,Current_Control_Data::period};
    PI<IntegratorType::Trapezoidal> current_regenerate_PI{Current_Control_Data::kp_regenerate,Current_Control_Data::ki_regenerate,Current_Control_Data::period};
    Data_struct *Data;
    SpaceVector *spaceVector;
    #if PPU_USING != 1
        Max_Peak current_u_a{Data->actual_current_sensor_u_a};
        Max_Peak current_v_a{Data->actual_current_sensor_v_a};
        Max_Peak current_w_a{Data->actual_current_sensor_w_a};
    #endif
    #if PPU_USING != 0
        Max_Peak current_u_b{Data->actual_current_sensor_u_b};
        Max_Peak current_v_b{Data->actual_current_sensor_v_b};
        Max_Peak current_W_b{Data->actual_current_sensor_w_b};
    #endif
    ControlStates currentControlState{ControlStates::accelerate};
    bool should_be_running{false};

public:
    CurrentControl(Data_struct *Data,SpaceVector *spaceVector);
    void set_current_ref(float cur_ref);
    double calculate_peak();
    float get_current_ref();
    void control_action();
    void start();
    void stop();
    bool is_running();
};