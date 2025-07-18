#pragma once 
#include "SpeedControl.hpp"
#include "Communication/Communication.hpp"


class RUNS
{

    private:
    inline static RunMode run{0};
    inline static float vel_ref{0.0};


    inline static Data_struct *Data{nullptr};
    
    public:
    static void init(Data_struct *d);
    static float update();
    static void start(uint8_t run_id);
};