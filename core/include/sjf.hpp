#ifndef SJF_HPP
#define SJF_HPP

#include "Process.hpp"
#include <vector>
#include <functional>
#include "Fcfs.hpp" // Para la declaración de sortByArrivalTime

void runSJF(std::vector<Process>& processes, std::function<void(const Process&)> onStateChange);

#endif // SJF_HPP