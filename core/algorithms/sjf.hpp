#ifndef SJF_HPP
#define SJF_HPP

#include "Process.hpp"
#include <vector>
#include <thread>
#include <chrono>
#include <functional>


// Implementación manual de Bubble Sort para ordenar por tiempo de rafaga
void sortByBurstTime(std::vector<Process>& processes) {
    int n = processes.size();
    for (int i = 0; i < n - 1; ++i) {
        for (int j = 0; j < n - i - 1; ++j) {
            if (processes[j].getBurstTime() > processes[j+1].getBurstTime()) {
                // Intercambiar procesos
                Process temp = processes[j];
                processes[j] = processes[j+1];
                processes[j+1] = temp;
            }
        }
    }
}


void runSJF(std::vector<Process>& processes, std::function<void(const Process&)> onStateChange) {
    // Primero, ordenar procesos por tiempo de rafaga
    sortByBurstTime(processes);

    int currentTime = 0;

    for (int i = 0; i < processes.size(); ++i) {
        Process& p = processes[i]; // Usar referencia para modificar el proceso original

        // Si la CPU está inactiva hasta que llegue este proceso, avanzar el tiempo
        if (currentTime < p.getArrivalTime()) {
            currentTime = p.getArrivalTime();
        }
        // PROCESO ESTA LISTO
        p.setCurrentState(Process::READY);
        onStateChange(p);
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); //Simula


        p.setStartTime(currentTime);
        p.setCurrentState(Process::State::RUNNING);
        onStateChange(p);
        std::this_thread::sleep_for(std::chrono::milliseconds(p.getBurstTime()*1000)); //Simula
        // Ejecutar el proceso
        currentTime += p.getBurstTime();

        p.setCompletionTime(currentTime);
        p.setTurnaroundTime(p.getCompletionTime() - p.getArrivalTime());
        p.setWaitingTime(p.getTurnaroundTime() - p.getBurstTime());
        p.setResponseTime(p.getStartTime() - p.getArrivalTime());
        p.setCurrentState(Process::State::TERMINATED);
        onStateChange(p);
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); //Simula
    }
}


#endif //SJF_HPP