#ifndef PROCESS_H
#define PROCESS_H

#include <iostream> 
class Process {
private:
    int id;
    int arrivalTime;
    int burstTime;
    int priority;

public:
    enum State {
        NEW,        // Recién creado, no ha llegado al sistema
        READY,      // Listo para ser ejecutado, esperando en la cola
        RUNNING,    // Actualmente en ejecución en la CPU
        TERMINATED  // Ha completado su ejecución
    };

private:
    State currentState; // Variable para almacenar el estado actual del proceso

    // Métricas de rendimiento
    int startTime;        // Tiempo en que el proceso comienza a ejecutarse por primera vez
    int completionTime;   // Tiempo de finalización
    int turnaroundTime;   // Tiempo de retorno (Completion Time - Arrival Time)
    int waitingTime;      // Tiempo de espera (Turnaround Time - Burst Time)
    int responseTime;     // Tiempo de respuesta (First Start Time - Arrival Time)

public:
    // Constructor de la clase
    Process(int idVal = 0, int arrivalTimeVal = 0, int burstTimeVal = 0, int priorityVal = 0)
        : id(idVal),
          arrivalTime(arrivalTimeVal),
          burstTime(burstTimeVal), 
          priority(priorityVal),
          currentState(NEW),      
          startTime(-1),           
          completionTime(0),
          turnaroundTime(0),
          waitingTime(0),
          responseTime(0)
    {}

    // Getters
    int getId() const {
        return id;
    }
    int getArrivalTime() const {
        return arrivalTime;
    }
    int getBurstTime() const {
        return burstTime;
    }
    int getPriority() const {
        return priority;
    }
    State getCurrentState() const {
        return currentState;
    }
    int getStartTime() const {
        return startTime;
    }
    int getCompletionTime() const {
        return completionTime;
    }
    int getTurnaroundTime() const {
        return turnaroundTime;
    }
    int getWaitingTime() const {
        return waitingTime;
    }
    int getResponseTime() const {
        return responseTime;
    }


    // Setters 
    void setId(int newId) {
        id = newId;
    }
    void setArrivalTime(int newArrival) {
        arrivalTime = newArrival;
    }
    void setBurstTime(int newBurstTime) {
        burstTime = newBurstTime; 
    }
    void setPriority(int newPriority) {
        priority = newPriority;
    }
    void setCurrentState(State newState) {
        currentState = newState;
    }
    void setStartTime(int newStartTime) {
        startTime = newStartTime;
    }
    void setCompletionTime(int newCompletionTime) {
        completionTime = newCompletionTime;
    }
    void setTurnaroundTime(int newTurnaroundTime) {
        turnaroundTime = newTurnaroundTime;
    }
    void setWaitingTime(int newWaitingTime) {
        waitingTime = newWaitingTime;
    }
    void setResponseTime(int newResponseTime) {
        responseTime = newResponseTime;
    }

    // Método para imprimir la información del proceso (útil para depuración)
    void printProcessInfo() const {
        std::cout << "Process ID: " << id
                  << ", Arrival: " << arrivalTime
                  << ", Burst: " << burstTime
                  << ", Priority: " << priority
                  << ", State: ";
        // Imprimir el nombre del estado según el enum
        switch (currentState) {
            case NEW: 
                std::cout << "NEW"; 
                break;
            case READY: 
                std::cout << "READY"; 
                break;
            case RUNNING: 
                std::cout << "RUNNING"; 
                break;
            case TERMINATED: 
                std::cout << "TERMINATED"; 
                break;
            default: 
                std::cout << "UNKNOWN"; 
                break;
        }
        std::cout << ", Start Time: " << startTime
                  << ", Completion Time: " << completionTime
                  << ", Turnaround Time: " << turnaroundTime
                  << ", Waiting Time: " << waitingTime
                  << ", Response Time: " << responseTime << std::endl;
    }
};

#ifndef PROCESS_H
#define PROCESS_H