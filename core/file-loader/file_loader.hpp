#ifndef FILE_LOADER_HPP
#define FILE_LOADER_HPP

#include "Process.hpp"
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class FileLoader {
public:
    static std::vector<Process> loadProcessesFromFile(const std::string& filename) {
        std::vector<Process> processes;
        std::ifstream file(filename);
        
        if (!file.is_open()) {
            std::cerr << "Error: No se pudo abrir el archivo " << filename << std::endl;
            return processes;
        }
        
        std::string line;
        bool isFirstLine = true;
        
        while (std::getline(file, line)) {
            // Saltar línea de encabezado si existe
            if (isFirstLine && (line.find("ID") != std::string::npos || 
                               line.find("id") != std::string::npos ||
                               line.find("#") != std::string::npos)) {
                isFirstLine = false;
                continue;
            }
            isFirstLine = false;
            
            // Saltar líneas vacías o comentarios
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            Process process = parseProcessLine(line);
            if (process.getId() > 0) { // Verificar que el proceso sea válido
                processes.push_back(process);
            }
        }
        
        file.close();
        
        std::cout << "Cargados " << processes.size() << " procesos desde " << filename << std::endl;
        return processes;
    }
    
private:
    static Process parseProcessLine(const std::string& line) {
        std::istringstream iss(line);
        std::string token;
        std::vector<std::string> tokens;
        
        // Separar por comas, espacios o tabs
        while (iss >> token) {
            // Remover comas si existen
            if (token.back() == ',') {
                token.pop_back();
            }
            tokens.push_back(token);
        }
        
        // Si no hay suficientes tokens, intentar separar por comas
        if (tokens.size() < 3) {
            tokens.clear();
            std::istringstream commaStream(line);
            while (std::getline(commaStream, token, ',')) {
                // Trim espacios
                token.erase(0, token.find_first_not_of(" \t"));
                token.erase(token.find_last_not_of(" \t") + 1);
                if (!token.empty()) {
                    tokens.push_back(token);
                }
            }
        }
        
        // Validar que tenemos al menos 3 campos (id, arrival, burst)
        if (tokens.size() < 3) {
            std::cerr << "Error: Línea inválida: " << line << std::endl;
            return Process(0, 0, 0, 1); // Proceso inválido
        }
        
        try {
            int id = std::stoi(tokens[0]);
            int arrivalTime = std::stoi(tokens[1]);
            int burstTime = std::stoi(tokens[2]);
            int priority = tokens.size() >= 4 ? std::stoi(tokens[3]) : 1;
            
            // Validar valores
            if (id <= 0 || arrivalTime < 0 || burstTime <= 0 || priority <= 0) {
                std::cerr << "Error: Valores inválidos en línea: " << line << std::endl;
                return Process(0, 0, 0, 1);
            }
            
            return Process(id, arrivalTime, burstTime, priority);
            
        } catch (const std::exception& e) {
            std::cerr << "Error al parsear línea: " << line << " - " << e.what() << std::endl;
            return Process(0, 0, 0, 1);
        }
    }
};

#endif //FILE_LOADER_HPP