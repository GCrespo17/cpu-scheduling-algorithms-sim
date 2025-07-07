#ifndef FCFS_HPP
#define FCFS_HPP

#include "Process.hpp"
#include <vector>
#include <functional>

// Declaración de la función para ordenar procesos por tiempo de llegada
void sortByArrivalTime(std::vector<Process>& processes);

// Declaración de la función para ejecutar el algoritmo FCFS (First Come First Served)
void runFCFS(std::vector<Process>& processes, std::function<void(const Process&)> onStateChange);

#endif // FCFS_HPP