#define CROW_ENABLE_CORS

#include "crow_all.h"
#include "Process.hpp"
#include "Fcfs.hpp"
#include "Process-json.hpp"
#include <vector>
#include <thread>
#include <mutex>
#include <set> 


std::set<crow::websocket::connection*> connections;
std::mutex connMutex;
std::vector<Process> lastRecievedProcesses;

std::vector<Process> parseProcessesFromJson(const crow::json::rvalue& json){
    std::vector<Process> result;
    for(const auto& p: json){
        Process proc(
            p["id"].i(),
            p["arrivalTime"].i(),
            p["burstTime"].i(),
            p["priority"].i()
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

        auto processes = parseProcessesFromJson(body);

        std::thread([processes](){
            auto procs = processes;
            runFCFS(procs, notifyClients);
        }).detach();

        return crow::response(200, "Simulation Started");
        
    });

    app.port(8081).multithreaded().run();
}