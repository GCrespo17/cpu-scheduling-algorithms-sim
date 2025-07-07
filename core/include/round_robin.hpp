#ifndef ROUND_ROBIN_HPP
#define ROUND_ROBIN_HPP

#include "Process.hpp"
#include <vector>
#include <functional>
#include "Fcfs.hpp" 

void runRR(std::vector<Process>& processes, int quantum, std::function<void(const Process&)> onStateChange);

#endif //ROUND_ROBIN_HPP