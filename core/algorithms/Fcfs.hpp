#ifndef FCFS_HPP
#define FCFS_HPP

#include "Process.hpp"
#include <vector>
#include <thread>
#include <chrono>
#include <functional>
#include <iostream> // Para depuración, si es necesario

// Implementación manual de Bubble Sort para ordenar por tiempo de llegada
void sortByArrivalTime(std::vector<Process>& processes) {
    int n = processes.size();
    for (int i = 0; i < n - 1; ++i) {
        for (int j = 0; j < n - i - 1; ++j) {
            if (processes[j].getArrivalTime() > processes[j+1].getArrivalTime()) {
                // Intercambiar procesos
                Process temp = processes[j];
                processes[j] = processes[j+1];
                processes[j+1] = temp;
            }
        }
    }
}


void runFCFS(std::vector<Process>& processes, std::function<void(const Process&)> onStateChange) {
    // Primero, ordenar procesos por tiempo de llegada
    sortByArrivalTime(processes);

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

#endif //FCFS_HPP