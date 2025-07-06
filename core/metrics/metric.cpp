// Metric.cpp
#include "Metric.hpp"

// Variable global
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