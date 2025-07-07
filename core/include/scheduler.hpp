#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include "Process.hpp"     
#include <vector>          
#include <string>          
#include <functional>      

void schedule(std::vector<Process>& processes, std::string algorithm, int quantum, std::function<void(const Process&)> onStateChange);

#endif // SCHEDULER_HPP