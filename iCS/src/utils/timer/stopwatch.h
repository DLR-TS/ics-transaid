#ifndef STOPWATCH_H
#define STOPWATCH_H

#include <time.h>
#include <iostream>
#include <fstream>

class Stopwatch {

private:

    clock_t start_ticks, stop_ticks;
    std::ofstream myfile;

public:

    bool StartStopwatch(); 
    bool HaltStopwatch();
    bool Writeresult(std::string file);

};

#endif