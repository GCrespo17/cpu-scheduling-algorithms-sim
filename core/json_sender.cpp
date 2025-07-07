#include "crow_all.h"
#include "Process.hpp"
#include "Metric.hpp"
#include "json_sender.hpp"
#include <mutex>
#include <set>
#include <ctime>


//-------------------------------------------------------------------------------------------------//
//----------------------------------------------PROCESS-------------------------------------------//
//-----------------------------------------------------------------------------------------------//

crow::json::wvalue processToJson(const Process& p) {
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

std::vector<Process> parseProcessesFromJson(const crow::json::rvalue& json){
    std::vector<Process> result;
    if(!json){
        return result;
    }
    // Iterar sobre el array de procesos
    for(const auto& p: json){
        // Verificar que todos los campos requeridos existen
        if(!p.has("id") || !p.has("arrivalTime") || !p.has("burstTime")){
            continue; // Saltar procesos con datos incompletos
        }
        
        // Extraer prioridad (con valor por defecto si no existe)
        int priority = 1;
        if(p.has("priority")){
            priority = p["priority"].i();
        }
        
        Process proc(
            p["id"].i(),
            p["arrivalTime"].i(),
            p["burstTime"].i(),
            priority
        );
        result.push_back(proc);
    }
    return result;
}

void notifyClients(const Process& p){
    auto json = processToJson(p).dump();
    std::lock_guard<std::mutex> lock(connMutex);
    for(auto* conn: connections){
        conn->send_text(json);
    }
}


//-------------------------------------------------------------------------------------------------//
//----------------------------------------------METRICS-------------------------------------------//
//-----------------------------------------------------------------------------------------------//

crow::json::wvalue metricToJson(const Metric& metric) {
    crow::json::wvalue json;
    
    json["algorithm"] = metric.getAlgorithmName();
    json["quantum"] = metric.getQuantum(); // ✅ INCLUIR QUANTUM
    json["avgTurnaroundTime"] = metric.getAvgTurnaroundTime();
    json["avgWaitingTime"] = metric.getAvgWaitingTime();
    json["avgResponseTime"] = metric.getAvgResponseTime();
    json["cpuUtilization"] = metric.getCpuUtilization();
    json["totalTime"] = metric.getTotalTime();
    json["totalCpuTime"] = metric.getTotalCpuTime();
    json["idleTime"] = metric.getTotalTime() - metric.getTotalCpuTime();
    
    // Array de métricas por proceso
    json["processMetrics"] = crow::json::wvalue::list();
    const auto& processes = metric.getProcesses();
    for (size_t i = 0; i < processes.size(); i++) {
        crow::json::wvalue processMetric;
        processMetric["id"] = processes[i].getId();
        processMetric["turnaroundTime"] = processes[i].getTurnaroundTime();
        processMetric["waitingTime"] = processes[i].getWaitingTime();
        processMetric["responseTime"] = processes[i].getResponseTime();
        processMetric["burstTime"] = processes[i].getBurstTime();
        processMetric["arrivalTime"] = processes[i].getArrivalTime();
        processMetric["completionTime"] = processes[i].getCompletionTime();
        processMetric["startTime"] = processes[i].getStartTime();
        json["processMetrics"][i] = std::move(processMetric);
    }
    
    return json;
}

void notifyMetrics(const Metric& metric) {
    crow::json::wvalue json = metricToJson(metric);
    json["type"] = "METRICS_COMPLETE";
    
    std::string jsonStr = json.dump();
    
    std::lock_guard<std::mutex> lock(connMutex);
    for(auto* conn: connections) {
        conn->send_text(jsonStr);
    }
}

void notifyComparison() {
    if (allMetrics.empty()) return;
    
    crow::json::wvalue json;
    json["type"] = "COMPARISON_PANEL";
    
    // Tomar los últimos 4 algoritmos (o todos si hay menos de 4)
    size_t startIndex = allMetrics.size() >= 4 ? allMetrics.size() - 4 : 0;
    std::vector<Metric> recentMetrics(allMetrics.begin() + startIndex, allMetrics.end());
    
    // Mejores por categoría (solo de los recientes)
    json["bestTurnaround"] = findBestAlgorithmFromSet(recentMetrics, "turnaround");
    json["bestWaiting"] = findBestAlgorithmFromSet(recentMetrics, "waiting");
    json["bestResponse"] = findBestAlgorithmFromSet(recentMetrics, "response");
    json["bestCpu"] = findBestAlgorithmFromSet(recentMetrics, "cpu");
    json["overallBest"] = findOverallBestFromSet(recentMetrics);
    
    // Estadísticas
    json["totalAlgorithms"] = (int)recentMetrics.size();
    json["timestamp"] = (int)time(nullptr);
    
    // Array con métricas recientes
    json["algorithms"] = crow::json::wvalue::list();
    for (size_t i = 0; i < recentMetrics.size(); i++) {
        json["algorithms"][i] = metricToJson(recentMetrics[i]);
    }
    
    // Generar recomendación
    std::string recommendation = generateRecommendation(recentMetrics);
    json["recommendation"] = recommendation;
    
    // Datos para Gantt Chart
    json["ganttData"] = generateGanttData(recentMetrics);
    
    std::string jsonStr = json.dump();
    
    std::lock_guard<std::mutex> lock(connMutex);
    for(auto* conn: connections) {
        conn->send_text(jsonStr);
    }
}

crow::json::wvalue generateGanttData(const std::vector<Metric>& metrics) {
    crow::json::wvalue ganttData;
    ganttData["charts"] = crow::json::wvalue::list();
    
    for (size_t i = 0; i < metrics.size(); i++) {
        const auto& metric = metrics[i];
        crow::json::wvalue chart;
        chart["algorithm"] = metric.getAlgorithmName();
        chart["processes"] = crow::json::wvalue::list();
        
        const auto& processes = metric.getProcesses();
        
        // Para Round Robin, simular segmentos usando el quantum real del usuario
        if (metric.getAlgorithmName() == "RR") {
            std::vector<crow::json::wvalue> segments;
            
            //USAR EL QUANTUM REAL DEL USUARIO
            int quantum = metric.getQuantum();
            
            // Simular la ejecución de Round Robin
            std::vector<int> arrivalTimes;
            std::vector<int> burstTimes;
            
            // Recopilar datos de procesos
            for (const auto& p : processes) {
                arrivalTimes.push_back(p.getArrivalTime());
                burstTimes.push_back(p.getBurstTime());
            }
            
            int currentTime = 0;
            std::vector<int> remainingTime = burstTimes;
            std::vector<bool> inSystem(processes.size(), false);
            std::queue<int> readyQueue;
            int completed = 0;
            
            while (completed < processes.size()) {
                // Agregar procesos que han llegado
                for (size_t j = 0; j < processes.size(); j++) {
                    if (!inSystem[j] && arrivalTimes[j] <= currentTime && remainingTime[j] > 0) {
                        readyQueue.push(j);
                        inSystem[j] = true;
                    }
                }
                
                if (readyQueue.empty()) {
                    currentTime++;
                    continue;
                }
                
                int processIndex = readyQueue.front();
                readyQueue.pop();
                
                //USAR QUANTUM REAL DEL USUARIO
                int executeTime = std::min(quantum, remainingTime[processIndex]);
                
                // Crear segmento
                crow::json::wvalue segment;
                segment["id"] = processes[processIndex].getId();
                segment["startTime"] = currentTime;
                segment["endTime"] = currentTime + executeTime;
                segment["duration"] = executeTime;
                segments.push_back(std::move(segment));
                
                currentTime += executeTime;
                remainingTime[processIndex] -= executeTime;
                
                // Verificar nuevos arribos durante la ejecución
                for (size_t j = 0; j < processes.size(); j++) {
                    if (!inSystem[j] && arrivalTimes[j] <= currentTime && remainingTime[j] > 0) {
                        readyQueue.push(j);
                        inSystem[j] = true;
                    }
                }
                
                if (remainingTime[processIndex] == 0) {
                    completed++;
                } else {
                    readyQueue.push(processIndex);
                }
            }
            
            // Convertir segmentos a formato JSON
            for (size_t j = 0; j < segments.size(); j++) {
                chart["processes"][j] = std::move(segments[j]);
            }
            
        } else {
            // Para FCFS, SJF, PRIORITY - usar datos directos
            for (size_t j = 0; j < processes.size(); j++) {
                crow::json::wvalue process;
                process["id"] = processes[j].getId();
                process["startTime"] = processes[j].getStartTime();
                process["endTime"] = processes[j].getCompletionTime();
                process["duration"] = processes[j].getBurstTime();
                process["waitTime"] = processes[j].getWaitingTime();
                chart["processes"][j] = std::move(process);
            }
        }
        
        ganttData["charts"][i] = std::move(chart);
    }
    
    return ganttData;
}