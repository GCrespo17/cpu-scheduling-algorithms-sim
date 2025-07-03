#include "crow_all.h"

int main() {
    crow::SimpleApp app;

    // Ruta de prueba para enviar un saludo
    CROW_ROUTE(app, "/hello").methods("GET"_method)(
        [](){
            crow::json::wvalue x; // Objeto JSON para la respuesta
            x["message"] = "Hola desde el backend de C++ (Crow)!";
            x["status"] = "ok";
            
            crow::response res({200});
            res.add_header("Content-Type", "application/json");
            // **IMPORTANTE para CORS**: Permite que cualquier origen acceda a este recurso.
            res.add_header("Access-Control-Allow-Origin", "*");
            res.body = x.dump(); // Convierte el objeto JSON a string
            return res;
        }
    );

    // Ruta OPTIONS para CORS preflight requests (para /hello)
    CROW_ROUTE(app, "/hello").methods("OPTIONS"_method)(
        [](const crow::request&){
            crow::response res(204); // 204 No Content para preflight
            res.add_header("Access-Control-Allow-Origin", "*");
            res.add_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS"); // Incluye los métodos que tu API usará
            res.add_header("Access-Control-Allow-Headers", "Content-Type"); // Incluye las cabeceras que tu frontend enviará
            return res;
        }
    );


    // Asegura de que Crow escuche en el puerto 8080, como se configuro en docker-compose.yml
    app.port(8080).multithreaded().run();
}