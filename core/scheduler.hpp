#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include "Process.hpp"
#include "Fcfs.hpp"
#include "sjf.hpp"
#include "round_robin.hpp"
#include "priority.hpp"
#include <string>


enum pType{
    FCFS,
    SJF,
    RR,
    PRIORITY
};


void schedule(std::vector<Process>& processes, std::string algorithm, int quantum, std::function<void(const Process&)> onStateChange){

    pType algorithmType=FCFS;

    if(algorithm == "FCFS"){
        algorithmType == FCFS;
    }else if(algorithm == "SJF"){
        algorithmType = SJF;
    }else if(algorithm == "RR"){
        algorithmType = RR;
    }else if(algorithm == "PRIORITY"){
        algorithmType = PRIORITY;
    }

    switch(algorithmType){
        case FCFS:
            runFCFS(processes, onStateChange);
            break;
        case SJF:
            runSJF(processes, onStateChange);
            break;
        case RR:
            runRR(processes, quantum, onStateChange);
            break;
        case PRIORITY:
            runPriority(processes, onStateChange);
            break;
        default:
            std::cerr<< "Tipo de algoritmo no soportado"<<std::endl;
            break;
    }
}



#endif //SCHEDULER_HPP