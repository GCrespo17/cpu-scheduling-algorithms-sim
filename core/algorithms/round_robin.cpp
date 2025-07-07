#include "Process.hpp"
#include "round_robin.hpp"
#include <vector>
#include <thread>
#include <chrono>
#include <functional>
#include <queue>


extern void sortByArrivalTime(std::vector<Process>& processes);



void runRR(std::vector<Process>& processes, int quantum, std::function<void(const Process&)> onStateChange){
    sortByArrivalTime(processes);

    int currentTime=0;
    std::queue<int> readyQueue; //Cola de procesos en estado READY
    std::vector<int> remainingTime(processes.size()); //Tiempo restante de cada proceso
    std::vector<bool> inQueue(processes.size(), false); //Verifica si el proceso esta en la cola
    std::vector<bool> completed(processes.size(), false); //Verifica si el proceso ya termino
    int completedCount = 0;
    int processIndex = 0; //indice para agregar procesos que van llegando

    //Inicializamos tiempos restantes
    for(int i=0; i<processes.size(); i++){
        remainingTime[i]=processes[i].getBurstTime();
    }

    //Iniciamos el round robin

    while(completedCount<processes.size()){
        while(processIndex < processes.size() && processes[processIndex].getArrivalTime()<=currentTime){
            if(!completed[processIndex] && !inQueue[processIndex]){

                //Marcamos el proceso como READY
                processes[processIndex].setCurrentState(Process::READY);
                onStateChange(processes[processIndex]);

                readyQueue.push(processIndex);
                inQueue[processIndex]=true;
            }
            processIndex++;
        }

        //Si no hay procesos en la cola, avanzamos el tiempo
        if(readyQueue.empty()){
            currentTime++;
            continue;
        }

        //Tomamos el siguiente proceso de la cola
        int currentProcessIndex = readyQueue.front();
        readyQueue.pop();
        inQueue[currentProcessIndex] = false;

        Process& currentProcess = processes[currentProcessIndex];

        //Si es la primera vez que se ejecuta el proceso, establecemos
        //un start time
        if(currentProcess.getStartTime() == -1){
            currentProcess.setStartTime(currentTime);
        }

        //Marcamos el proceso actual como RUNNING
        currentProcess.setCurrentState(Process::RUNNING);
        onStateChange(currentProcess);

        //Calculamos el tiempo de ejecucion
        //El tiempo de ejecucion puede ser el menor entre quantum o tiempo restante
        int executeTime = std::min(quantum, remainingTime[currentProcessIndex]);
        
        //Simulamos la ejecucion mientras esta RUNNING
        std::this_thread::sleep_for(std::chrono::milliseconds(executeTime*1000));

        //Actualizamos el tiempo y tiempo restante
        currentTime += executeTime;
        remainingTime[currentProcessIndex] = remainingTime[currentProcessIndex] - executeTime;

        //Agregamos nuevos procesos que llegaron durante la ejecucion
        while(processIndex < processes.size() && processes[processIndex].getArrivalTime() <= currentTime){
            if(!completed[processIndex] && !inQueue[processIndex]){
                processes[processIndex].setCurrentState(Process::READY);
                onStateChange(processes[processIndex]);

                readyQueue.push(processIndex);
                inQueue[processIndex] = true;
            }
            processIndex++;
        }

        //Verificamos si el proceso termino
        if(remainingTime[currentProcessIndex] == 0){
            //El proceso fue completado
            currentProcess.setCompletionTime(currentTime);
            currentProcess.setTurnaroundTime(currentProcess.getCompletionTime() - currentProcess.getArrivalTime());
            currentProcess.setWaitingTime(currentProcess.getTurnaroundTime() - currentProcess.getBurstTime());
            currentProcess.setResponseTime(currentProcess.getStartTime() - currentProcess.getArrivalTime());
            currentProcess.setCurrentState(Process::TERMINATED);
            onStateChange(currentProcess);
            
            completed[currentProcessIndex] = true;
            completedCount++;
        }else{
            //Si el proceso no esta completado, lo registramos en la cola
            currentProcess.setCurrentState(Process::READY);
            onStateChange(currentProcess);

            readyQueue.push(currentProcessIndex);
            inQueue[currentProcessIndex] = true;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    //ROUND ROBIN COMPLETADO
}