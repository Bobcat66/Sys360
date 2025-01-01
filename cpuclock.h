#ifndef CPUCLOCK_H
#define CPUCLOCK_H

#include <chrono>
#include <atomic>
#include <thread>

//Warning: take this clock's accuracy with a grain of salt
class cpu_clock {
    public:
    void start();   //Starts the clock
    void stop();    //Stops the clock
    void reset();   //Resets the clock
    void setInterval(unsigned long newInterval); //Sets interval in microseconds
    unsigned long gettime();  //Returns the clock's current time
    private:
    std::thread clockthread;
    std::atomic_ulong time;
    std::atomic_ulong intervalMicro = 1; //The clock's interval, in microseconds
    std::atomic_bool running;
    void run();
};

#endif