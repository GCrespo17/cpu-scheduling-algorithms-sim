#define CROW_ENABLE_CORS
#include "crow_all.h"
#include "Process.hpp"
#include "scheduler.hpp"
#include "file_loader.hpp"
#include "json_sender.hpp"
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <set> 

std::set<crow::websocket::connection*> connections;
std::mutex connMutex;
std::vector<Process> lastRecievedProcesses;

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

    // Limpiar métricas
    CROW_ROUTE(app, "/api/clear-metrics").methods("POST"_method)
    ([](const crow::request& req){
        clearAllMetrics();
        return crow::response(200, "Metrics cleared");
    });

    //Cargar procesos desde archivo
    CROW_ROUTE(app, "/api/load-processes").methods("GET"_method)
        ([](const crow::request& req){
            // Intentar cargar desde diferentes ubicaciones
            std::vector<std::string> possiblePaths = {
                "files/processes.txt",
                "processes.txt",
            };
            
            std::vector<Process> processes;
            std::string usedPath = "";
            
            for (const auto& path : possiblePaths) {
                processes = FileLoader::loadProcessesFromFile(path);
                if (!processes.empty()) {
                    usedPath = path;
                    break;
                }
            }
            
            if (processes.empty()) {
                return crow::response(404, "{\"error\":\"No se encontró el archivo de procesos o está vacío\"}");
            }
            
            // Convertir procesos a JSON
            crow::json::wvalue json;
            json["processes"] = crow::json::wvalue::list();
            json["count"] = (int)processes.size();
            json["source"] = usedPath;
            
            for (size_t i = 0; i < processes.size(); i++) {
                crow::json::wvalue processJson;
                processJson["id"] = processes[i].getId();
                processJson["arrivalTime"] = processes[i].getArrivalTime();
                processJson["burstTime"] = processes[i].getBurstTime();
                processJson["priority"] = processes[i].getPriority();
                json["processes"][i] = std::move(processJson);
            }
            
            return crow::response(200, json.dump());
        });


    app.port(8081).multithreaded().run();
}