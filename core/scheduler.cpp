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
    crow::SimpleApp app;

    //RUTA PARA WEBSOCKET
    CROW_ROUTE(app, "/ws")
    .websocket(&app)
    .onopen([](crow::websocket::connection& conn){
        std::lock_guard<std::mutex> lock(connMutex);
        connections.insert(&conn);
    })
    .onclose([](crow::websocket::connection& conn, const std::string& reason){
        std::lock_guard<std::mutex> lock(connMutex);
        connections.erase(&conn);
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