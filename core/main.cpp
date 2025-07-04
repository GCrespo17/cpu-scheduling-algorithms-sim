#include "crow_all.h"
#include <set> // Para manejar las conexiones de WebSocket
#include <mutex> // Para proteger el acceso al conjunto de usuarios

int main() {
    crow::SimpleApp app;

    // Conjunto para almacenar las conexiones de WebSocket activas.
    std::set<crow::websocket::connection*> users;
    // Un mutex para proteger el acceso al conjunto 'users' en un entorno multihilo.
    std::mutex mtx;

    // Ruta WebSocket
    CROW_WEBSOCKET_ROUTE(app, "/ws")
        .onopen([&](crow::websocket::connection& conn) {
            std::lock_guard<std::mutex> _(mtx);
            users.insert(&conn);
            CROW_LOG_INFO << "Nueva conexión WebSocket establecida. Total de clientes: " << users.size();
        })
        // CORRECCIÓN: Se añade 'uint16_t code' como tercer argumento para onclose.
        .onclose([&](crow::websocket::connection& conn, const std::string& reason, uint16_t code) {
            std::lock_guard<std::mutex> _(mtx);
            users.erase(&conn);
            CROW_LOG_INFO << "Conexión WebSocket cerrada. Código: " << code << ", Razón: " << reason << ". Clientes restantes: " << users.size();
        })
        .onmessage([&](crow::websocket::connection& conn, const std::string& data, bool is_binary) {
            CROW_LOG_INFO << "Mensaje recibido de " << (void*)&conn << ": " << data;

            std::lock_guard<std::mutex> _(mtx);
            for (auto* user : users) {
                // CORRECCIÓN: Se elimina la verificación user->is_open()
                // La librería Crow gestiona el estado de las conexiones.
                // Si una conexión está en el set 'users', se asume que es válida para enviar.
                user->send_text("Mensaje de broadcast desde el servidor: " + data);
            }
        })
        .onerror([&](crow::websocket::connection& conn, const std::string& error_msg) {
            CROW_LOG_ERROR << "Error en conexión WebSocket " << (void*)&conn << ": " << error_msg;
        });

    // Ruta de prueba para enviar un saludo (opcional, para mantener un endpoint HTTP)
    CROW_ROUTE(app, "/hello").methods("GET"_method)(
        [](){
            crow::json::wvalue x;
            x["message"] = "Hola desde el backend de C++ (Crow)!";
            x["status"] = "ok";

            crow::response res({200});
            res.add_header("Content-Type", "application/json");
            res.add_header("Access-Control-Allow-Origin", "*");
            res.body = x.dump();
            return res;
        }
    );

    // Ruta OPTIONS para CORS preflight requests (para /hello)
    CROW_ROUTE(app, "/hello").methods("OPTIONS"_method)(
        [](const crow::request&){
            crow::response res(204);
            res.add_header("Access-Control-Allow-Origin", "*");
            res.add_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
            res.add_header("Access-Control-Allow-Headers", "Content-Type");
            return res;
        }
    );

    app.port(8081).multithreaded().run();
}
