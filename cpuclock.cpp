#include "cpuclock.h"
#include <thread>
void cpu_clock::reset(){
    time = 0;
}

void cpu_clock::start(){
    running = true;
    std::thread clockthread(cpu_clock::run,this);
}

void cpu_clock::stop(){
    running = false;
}

void cpu_clock::setInterval(unsigned long newInterval){
    intervalMicro = newInterval;
}

unsigned long cpu_clock::gettime(){
    return time.load();
}

void cpu_clock::run(){
    while (running){
        std::this_thread::sleep_for(std::chrono::microseconds(intervalMicro));
        time += intervalMicro;
    }
}