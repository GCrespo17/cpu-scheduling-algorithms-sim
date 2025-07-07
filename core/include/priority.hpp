#ifndef PRIORITY_HPP
#define PRIORITY_HPP

#include "Process.hpp"
#include <vector>
#include <functional>
#include "Fcfs.hpp" 

void runPriority(std::vector<Process>& processes, std::function<void(const Process& )>onStateChange);

#endif //PRIORITY_HPP