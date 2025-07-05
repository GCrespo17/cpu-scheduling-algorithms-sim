#ifndef SJF_HPP
#define SJF_HPP

#include "Process.hpp"
#include <vector>
#include <thread>
#include <chrono>
#include <functional>


// Implementación para ordenar segun hora de llegada que ya esta en FCFS
extern void sortByArrivalTime(std::vector<Process>& processes);


void runSJF(std::vector<Process>& processes, std::function<void(const Process&)> onStateChange) {
    // Primero, ordenar procesos por tiempo de rafaga
    sortByArrivalTime(processes);

    int currentTime = 0;
    //Se declara un arreglo de verdadero y falso, tomando como parametros el numero
    //de procesos y todos inician como falso
    std::vector<bool> completedProcesses(processes.size(), false); 

    int completedCount = 0; //Contador de procesos terminados

    // ========== EJEMPLO CON 3 PROCESOS ==========
    // Si tienes 3 procesos: P1, P2, P3
    // completed = [false, false, false]
    //              ↑       ↑       ↑
    //             P1      P2      P3
    //           (índice 0)(índice 1)(índice 2)

    while(completedCount<processes.size()){
        //Buscamos los procesos disponibles que aun no han terminado
        std::vector<int> availableProcesses;
        
        for(int i=0; i<processes.size(); i++){
            //Verificamos si el proceso no esta completado y ya llego
            if(!completedProcesses[i] && processes[i].getArrivalTime() <= currentTime){
                availableProcesses.push_back(i);
            }
        }

        //Si no hay procesos disponibles, se avanza el tiempo
        if(availableProcesses.empty()){
            currentTime++;
            continue;
        }

        //Encontrar el proceso con menor burst time de los disponibles
        int shortestJobIndex = availableProcesses[0];
        for(int i=1; i<availableProcesses.size(); i++){
            int currentIndex = availableProcesses[i];
            if(processes[currentIndex].getBurstTime()<processes[shortestJobIndex].getBurstTime()){
                shortestJobIndex = currentIndex;
            }
        }

        //Ejecutamos el processo seleccionado
        Process& selectedProcess = processes[shortestJobIndex];

        //El proceso esta READY y notificamos
        selectedProcess.setCurrentState(Process::READY);
        onStateChange(selectedProcess);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        //Establecer tiempo de inicio y marcar como RUNNING
        selectedProcess.setStartTime(currentTime);
        selectedProcess.setCurrentState(Process::RUNNING);
        onStateChange(selectedProcess);
        //Simulamos la ejecucion del proceso
        std::this_thread::sleep_for(std::chrono::milliseconds(selectedProcess.getBurstTime()*1000));

        //Completar el proceso
        currentTime += selectedProcess.getBurstTime();

        //Calcular las metricas
        selectedProcess.setCompletionTime(currentTime);
        selectedProcess.setTurnaroundTime(selectedProcess.getCompletionTime()-selectedProcess.getArrivalTime());
        selectedProcess.setWaitingTime(selectedProcess.getTurnaroundTime()-selectedProcess.getBurstTime());
        selectedProcess.setResponseTime(selectedProcess.getStartTime()-selectedProcess.getArrivalTime());

        //Terminar el proceso TERMINATED
        selectedProcess.setCurrentState(Process::TERMINATED);
        onStateChange(selectedProcess);

        //Completar el proceso
        completedProcesses[shortestJobIndex] = true;
        completedCount++;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    
}


#endif //SJF_HPP