#include "stopwatch.h"

using namespace std;

bool
Stopwatch::StartStopwatch()
{
    start_ticks = clock();
    return true;
}

bool
Stopwatch::HaltStopwatch()
{
    stop_ticks = clock();
    return true;
}

bool
Stopwatch::Writeresult(string file)
{
    myfile.open(file.c_str());
    double d = (clock() - start_ticks);
    myfile << d /CLOCKS_PER_TICKS;
    myfile.close();


    return true;
}