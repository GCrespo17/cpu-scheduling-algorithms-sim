#define CROW_ENABLE_CORS

#include "crow_all.h"
#include "Process.hpp"
#include "Process-json.hpp"
#include "scheduler.hpp"
#include "metrics/metrics_notify.hpp"
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <set> 

std::set<crow::websocket::connection*> connections;
std::mutex connMutex;
std::vector<Process> lastRecievedProcesses;

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

int main(){
    // Usar CORSHandler en lugar de la estructura personalizada
    crow::App<crow::CORSHandler> app;
    
    // Configurar CORS globalmente
    auto& cors = app.get_middleware<crow::CORSHandler>();
    cors
        .global()
        .headers("Content-Type", "Authorization")
        .methods("GET"_method, "POST"_method, "OPTIONS"_method)
        .origin("*");
    
    //RUTA PARA WEBSOCKET
    CROW_ROUTE(app, "/ws")
    .websocket(&app)
    .onopen([](crow::websocket::connection& conn){
        std::lock_guard<std::mutex> lock(connMutex);
        connections.insert(&conn);
    })
    .onclose([](crow::websocket::connection& conn, const std::string& reason, unsigned short code){
        std::lock_guard<std::mutex> lock(connMutex);
        connections.erase(&conn);
    });

    // Manejar OPTIONS para preflight
    CROW_ROUTE(app, "/api/processes").methods("OPTIONS"_method)
    ([](const crow::request& req){
        crow::response res(200);
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.add_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        return res;
    });

    //HTTP POST PARA RECIBIR PROCESOS Y ARRANCAR LA SIMULACION
    CROW_ROUTE(app, "/api/processes").methods("POST"_method)
    ([](const crow::request& req){
        auto body = crow::json::load(req.body);
        if(!body){
            return crow::response(400, "Invalid JSON");
        }

        std::string algorithm = "FCFS";
        if(body.has("algorithm")){
            algorithm = body["algorithm"].s();
        }

        int quantum = 2;
        if(body.has("quantum")){
            quantum = body["quantum"].i();
        }

        std::vector<Process> processes;
        if(body.has("processes")){
            processes = parseProcessesFromJson(body["processes"]);
        }else{
            return crow::response(400, "No process provided");
        }

        std::thread([processes, algorithm, quantum]() mutable{
            schedule(processes, algorithm, quantum, notifyClients);
            if(!allMetrics.empty()){
                Metric lastMetric = allMetrics.back();
                notifyMetrics(lastMetric);
            }
        }).detach();

        return crow::response(200, "Simulation Started");
    });

    // Obtener panel de comparación
    CROW_ROUTE(app, "/api/comparison-panel").methods("GET"_method)
    ([](const crow::request& req){
        if (allMetrics.empty()) {
            return crow::response(404, "{\"error\":\"No metrics available\"}");
        }
        
        // Enviar comparación por WebSocket
        notifyComparison();
        
        return crow::response(200, "{\"status\":\"comparison_sent\"}");
    });

    //Ejecutar comparación completa de 4 algoritmos
    CROW_ROUTE(app, "/api/run-all-algorithms").methods("POST"_method)
    ([](const crow::request& req){
        auto body = crow::json::load(req.body);
        if(!body){
            return crow::response(400, "Invalid JSON");
        }
        
        std::vector<Process> originalProcesses = parseProcessesFromJson(body["processes"]);
        
        
        int quantum = 2;
        if(body.has("quantum")){
            quantum = body["quantum"].i();
        }
        
        std::thread([originalProcesses, quantum]() mutable {
            // Limpiar métricas anteriores para comparación limpia
            clearAllMetrics();
            
            std::vector<std::string> algorithms = {"FCFS", "SJF", "RR", "PRIORITY"};
            
            for (const std::string& algo : algorithms) {
                // Copiar procesos para cada algoritmo
                std::vector<Process> processes = originalProcesses;
                
                // Ejecutar simulación
                schedule(processes, algo, quantum, notifyClients);
                
                // Pequeña pausa entre algoritmos
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
            
            // Enviar comparación final automáticamente
            notifyComparison();
            
        }).detach();
        
        return crow::response(200, "Running all algorithms");
    });

    // Limpiar métricas
    CROW_ROUTE(app, "/api/clear-metrics").methods("POST"_method)
    ([](const crow::request& req){
        clearAllMetrics();
        return crow::response(200, "Metrics cleared");
    });

    app.port(8081).multithreaded().run();
}