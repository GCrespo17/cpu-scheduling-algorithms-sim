#ifndef PRIORITY_HPP
#define PRIORITY_HPP

#include "Process.hpp"
#include <vector>
#include <thread>
#include <chrono>
#include <functional>

//Funcion hecha en fcfs.hpp para ordenar por tiempo de llegada
extern void sortByArrivalTime(std::vector<Process>& processes);

//Priority forma no preemptiva
void runPriority(std::vector<Process>& processes, std::function<void(const Process& )>onStateChange){
    //Ordenamos por orden de llegada
    sortByArrivalTime(processes);
    int currentTime = 0;
    std::vector<bool> completed(processes.size(), false);
    int completedCount = 0;

    while(completedCount < processes.size()){
        //Encontramos los procesos que han llegado y no han sido completado
        std::vector<int> availableProcesses;
        for(int i=0; i<processes.size(); i++){
            if(!completed[i] && processes[i].getArrivalTime()<=currentTime){
                availableProcesses.push_back(i);
            }
        }

        //Si no hay procesos disponibles, avanzar el tiempo
        if(availableProcesses.empty()){
            currentTime++;
            continue;;
        }

        //De los procesos disponibles, encontrar el de mayor prioridad
        int highestPriorityIndex = availableProcesses[0];
        for(int i=1; i<availableProcesses.size(); i++){
            int currentIndex = availableProcesses[i];

            //Comparamos las prioridades
            if(processes[currentIndex].getPriority()<processes[highestPriorityIndex].getPriority()){
                highestPriorityIndex = currentIndex;
            }else if(processes[currentIndex].getPriority() == processes[highestPriorityIndex].getPriority()){
                //En caso de un empate de prioridades, usamos FCFS como criterio de desempate
                if(processes[currentIndex].getArrivalTime() < processes[highestPriorityIndex].getArrivalTime()){
                    highestPriorityIndex=currentIndex;
                }else if(processes[currentIndex].getArrivalTime() == processes[highestPriorityIndex].getArrivalTime()){
                    //Si tambien hay empate, utilizaremos SJF como criterio
                    if(processes[currentIndex].getBurstTime() < processes[highestPriorityIndex].getBurstTime()){
                        highestPriorityIndex = currentIndex;
                    }
                }
                //
            }
        }

        //Ejecutamos el proceso seleccionado
        Process& selectedProcess = processes[highestPriorityIndex];

        //Marcamos el proceso como READY
        selectedProcess.setCurrentState(Process::READY);
        onStateChange(selectedProcess);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        //Establecemos el tiempo de inicio y marcamos elproceso como RUNNING
        selectedProcess.setStartTime(currentTime);
        selectedProcess.setCurrentState(Process::RUNNING);
        onStateChange(selectedProcess);
        //Simulamos la ejecucion del proceso
        std::this_thread::sleep_for(std::chrono::milliseconds(selectedProcess.getBurstTime()*1000));

        //Completamos el proceso
        currentTime += selectedProcess.getBurstTime();

        //Calculamos las metricas
        selectedProcess.setCompletionTime(currentTime);
        selectedProcess.setTurnaroundTime(selectedProcess.getCompletionTime() - selectedProcess.getArrivalTime());
        selectedProcess.setWaitingTime(selectedProcess.getTurnaroundTime() - selectedProcess.getBurstTime());
        selectedProcess.setResponseTime(selectedProcess.getStartTime() - selectedProcess.getArrivalTime());
        
        //Marcamos el proceso como TERMINATED
        selectedProcess.setCurrentState(Process::TERMINATED);
        onStateChange(selectedProcess);

        //Completamos el proceso
        completed[highestPriorityIndex] = true;
        completedCount++;

        std::this_thread::sleep_for(std::chrono::milliseconds(500));

    }

}


#endif //PRIORITY_HPP