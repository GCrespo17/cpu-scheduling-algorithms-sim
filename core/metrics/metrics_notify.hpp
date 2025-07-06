#ifndef METRICS_JSON_HPP
#define METRICS_JSON_HPP

#include "crow_all.h"
#include "Metric.hpp"
#include <mutex>
#include <set>
#include <ctime>

// Declarar variables externas del servidor
extern std::set<crow::websocket::connection*> connections;
extern std::mutex connMutex;
extern std::vector<Metric> allMetrics;

// Declarar funciones auxiliares
std::string findBestAlgorithmFromSet(const std::vector<Metric>& metrics, const std::string& criterion);
std::string findOverallBestFromSet(const std::vector<Metric>& metrics);
std::string generateRecommendation(const std::vector<Metric>& metrics);
crow::json::wvalue generateGanttData(const std::vector<Metric>& metrics);

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

// Implementaciones de funciones auxiliares
std::string findBestAlgorithmFromSet(const std::vector<Metric>& metrics, const std::string& criterion) {
    if (metrics.empty()) return "N/A";
    
    auto best = metrics.begin();
    for (auto it = metrics.begin(); it != metrics.end(); ++it) {
        if (criterion == "turnaround" && it->getAvgTurnaroundTime() < best->getAvgTurnaroundTime()) {
            best = it;
        } else if (criterion == "waiting" && it->getAvgWaitingTime() < best->getAvgWaitingTime()) {
            best = it;
        } else if (criterion == "response" && it->getAvgResponseTime() < best->getAvgResponseTime()) {
            best = it;
        } else if (criterion == "cpu" && it->getCpuUtilization() > best->getCpuUtilization()) {
            best = it;
        }
    }
    return best->getAlgorithmName();
}

std::string findOverallBestFromSet(const std::vector<Metric>& metrics) {
    if (metrics.empty()) return "N/A";
    
    auto best = metrics.begin();
    double bestScore = best->getCpuUtilization() - best->getAvgWaitingTime();
    
    for (auto it = metrics.begin(); it != metrics.end(); ++it) {
        double score = it->getCpuUtilization() - it->getAvgWaitingTime();
        if (score > bestScore) {
            bestScore = score;
            best = it;
        }
    }
    return best->getAlgorithmName();
}

std::string generateRecommendation(const std::vector<Metric>& metrics) {
    if (metrics.empty()) return "No hay datos para analizar.";
    
    std::string best = findOverallBestFromSet(metrics);
    
    std::string recommendation = "Basado en el analisis de " + std::to_string(metrics.size()) + " algoritmos:\n\n";
    recommendation += "ALGORITMO RECOMENDADO: " + best + "\n\n";
    
    // Análisis por tipo de algoritmo
    for (const auto& metric : metrics) {
        std::string algo = metric.getAlgorithmName();
        if (algo == "FCFS") {
            recommendation += "• FCFS: Simple y predecible, pero puede sufrir el 'convoy effect'.\n";
        } else if (algo == "SJF") {
            recommendation += "• SJF: Excelente para minimizar tiempo de espera, riesgo de starvation.\n";
        } else if (algo == "RR") {
            recommendation += "• Round Robin: Justo y responsivo, overhead por cambios de contexto.\n";
        } else if (algo == "PRIORITY") {
            recommendation += "• Priority: Control fino de prioridades, cuidado con starvation.\n";
        }
    }
    
    return recommendation;
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


#endif //METRICS_JSON_HPP