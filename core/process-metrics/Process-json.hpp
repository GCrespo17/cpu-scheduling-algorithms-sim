#ifndef PROCESS_JSON_HPP
#define PROCESS_JSON_HPP
#include "Process.hpp"
#include "crow_all.h"

// Serializa un Process a JSON usando crow
inline crow::json::wvalue processToJson(const Process& p) {
    crow::json::wvalue x;
    x["id"] = p.getId();
    x["arrival_time"] = p.getArrivalTime();
    x["burst_time"] = p.getBurstTime();
    x["priority"] = p.getPriority();
    x["state"] = p.getCurrentState(); // O puedes mapear a string si prefieres
    x["start_time"] = p.getStartTime();
    x["completion_time"] = p.getCompletionTime();
    x["turnaround_time"] = p.getTurnaroundTime();
    x["waiting_time"] = p.getWaitingTime();
    x["response_time"] = p.getResponseTime();
    return x;
}

#endif