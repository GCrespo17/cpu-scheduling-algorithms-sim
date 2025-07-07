#include "Metric.hpp"

std::vector<Metric> allMetrics;

Metric calculateMetrics(std::vector<Process>& processes, const std::string& algorithmName, int quantum) {
    Metric metric(processes, algorithmName, quantum);
    allMetrics.push_back(metric);
    return metric;
}

void clearAllMetrics() {
    allMetrics.clear();
}

std::string findBestAlgorithm(const std::string& criterion) {
    if (allMetrics.empty()) return "N/A";
    
    auto best = allMetrics.begin();
    for (auto it = allMetrics.begin(); it != allMetrics.end(); ++it) {
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

std::string findOverallBest() {
    if (allMetrics.empty()) return "N/A";
    
    auto best = allMetrics.begin();
    double bestScore = best->getCpuUtilization() - best->getAvgWaitingTime();
    
    for (auto it = allMetrics.begin(); it != allMetrics.end(); ++it) {
        double score = it->getCpuUtilization() - it->getAvgWaitingTime();
        if (score > bestScore) {
            bestScore = score;
            best = it;
        }
    }
    return best->getAlgorithmName();
}

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
            recommendation += "• Priority: Control fino de prioridades, riesgo de starvation.\n";
        }
    }
    
    return recommendation;
}