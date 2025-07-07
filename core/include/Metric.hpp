#ifndef METRIC_HPP
#define METRIC_HPP

#include "Process.hpp"
#include <vector>
#include <string>
#include <algorithm>

class Metric{
    private:
        std::vector<Process> processes; 
        double avgTurnaroundTime;
        double avgWaitingTime;
        double avgResponseTime;
        double cpuUtilization;
        int totalTime;
        int totalCpuTime;
        std::string algorithmName;
        int quantum;

        double calculateAvgTurnaroundTime(std::vector<Process>& processes){
            if (processes.empty()) return 0.0;
            int turnAroundTimeSum=0;
            for(int i=0; i<processes.size(); i++){
                turnAroundTimeSum += processes[i].getTurnaroundTime();
            }
            return (double)turnAroundTimeSum/processes.size();
        }

        double calculateAvgWaitingTime(std::vector<Process>& processes){
            if (processes.empty()) return 0.0;
            int waitingTimeSum=0;
            for(int i=0; i<processes.size(); i++){
                waitingTimeSum += processes[i].getWaitingTime();
            }
            return (double)waitingTimeSum/processes.size();
        }

        double calculateAvgResponseTime(std::vector<Process>& processes){
            if (processes.empty()) return 0.0;
            int responseTimeSum=0;
            for(int i=0; i<processes.size(); i++){
                responseTimeSum += processes[i].getResponseTime();
            }
            return (double)responseTimeSum/processes.size();
        }

        int calculateTotalTime(std::vector<Process>& processes){
            if (processes.empty()) return 0;
            int maxCompletionTime = 0;
            for (int i = 0; i < processes.size(); i++) {
                maxCompletionTime = std::max(maxCompletionTime, processes[i].getCompletionTime());
            }
            return maxCompletionTime;
        }

        int calculateTotalCpuTime(std::vector<Process>& processes){
            int burstTimeSum=0;
            for(int i=0; i<processes.size(); i++){
                burstTimeSum += processes[i].getBurstTime();
            }
            return burstTimeSum;
        }

        double calculateCpuUtilization(std::vector<Process>& processes){
            int total = calculateTotalTime(processes);
            if (total == 0) return 0.0;
            return ((double)calculateTotalCpuTime(processes) * 100.0) / total;
        }

    public:
        Metric(std::vector<Process>& processesVal, const std::string& algorithmNameVal, int quantumVal):
            processes(processesVal),
            avgTurnaroundTime(calculateAvgTurnaroundTime(processesVal)),
            avgWaitingTime(calculateAvgWaitingTime(processesVal)),
            avgResponseTime(calculateAvgResponseTime(processesVal)),
            cpuUtilization(calculateCpuUtilization(processesVal)),
            totalTime(calculateTotalTime(processesVal)),
            totalCpuTime(calculateTotalCpuTime(processesVal)),
            algorithmName(algorithmNameVal),
            quantum(quantumVal){}

        // ✅ CORREGIDO: Todos los getters ahora son const
        double getAvgTurnaroundTime() const {
            return avgTurnaroundTime;
        }

        double getAvgWaitingTime() const {
            return avgWaitingTime;
        }

        double getAvgResponseTime() const {
            return avgResponseTime;
        }

        double getCpuUtilization() const {
            return cpuUtilization;
        }

        int getTotalTime() const {
            return totalTime;
        }

        int getTotalCpuTime() const {
            return totalCpuTime;
        }

        std::string getAlgorithmName() const {
            return algorithmName;
        }
        const std::vector<Process>& getProcesses() const {
            return processes;
        }
        int getQuantum() const{
            return quantum;
        }
};

extern std::vector<Metric> allMetrics;
Metric calculateMetrics(std::vector<Process>& processes, const std::string& algorithmName, const int quantum);
void clearAllMetrics();
std::string findBestAlgorithm(const std::string& criterion);
std::string findOverallBest();
std::string findBestAlgorithmFromSet(const std::vector<Metric>& metrics, const std::string& criterion);
std::string findOverallBestFromSet(const std::vector<Metric>& metrics);
std::string generateRecommendation(const std::vector<Metric>& metrics);

#endif //METRIC_HPP