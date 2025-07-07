#ifndef JSON_SENDER_HPP
#define JSON_SENDER_HPP

#include "crow_all.h"
#include "Process.hpp"   
#include "Metric.hpp"  
#include <mutex>        
#include <set>          
#include <ctime>  


extern std::set<crow::websocket::connection*> connections;
extern std::mutex connMutex;
extern std::vector<Metric> allMetrics;


// PROCESSES //
crow::json::wvalue processToJson(const Process& p);
std::vector<Process> parseProcessesFromJson(const crow::json::rvalue& json);
void notifyClients(const Process& p);

//METRICS//
crow::json::wvalue generateGanttData(const std::vector<Metric>& metrics);
crow::json::wvalue metricToJson(const Metric& metric);
void notifyMetrics(const Metric& metric);
void notifyComparison();


#endif //JSON_SENDER_HPP