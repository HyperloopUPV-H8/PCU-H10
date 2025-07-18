#pragma once 
#include "SpeedControl.hpp"
#include "Communication/Communication.hpp"


class RUNS
{

    private:
    inline static RunMode run{0};
    inline static float vel_ref{0.0};


    inline static Data_struct *Data{nullptr};

    inline static float distance_brake{24.0};
    
    public:
    static void init(Data_struct *d);
    static float update();
    static void start(uint8_t run_id);
};