# Versión mínima de CMake requerida
cmake_minimum_required(VERSION 3.10)

# Define el nombre de tu proyecto
project(CPUSimulator) 

# Establece el estándar de C++ a usar.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Añade el directorio 'libs' a la ruta de búsqueda de includes
# Añade el directorio 'algorithms' a la ruta de búsqueda de includes
include_directories(
    libs 
    algorithms
    process-metrics) 

# Añade el ejecutable principal de tu simulador.
# Asegúrate de incluir main.cpp, ya que ahí estará tu servidor Crow.
add_executable(CPUSimulator
    server.cpp   #El archivo que contiene la logica de crow y ejecuta los procesos
    algorithms/priority.cpp
    algorithms/round_robin.cpp
    algorithms/sjf.cpp
)

target_link_libraries(CPUSimulator
    pthread
    ssl
    crypto
)


# Opcional: Define un directorio de salida para los ejecutables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Configuración de flags de compilación (Release para optimización)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")