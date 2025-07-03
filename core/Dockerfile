# Se usara Ubuntu 24.04 LTS Sistema Operativo base para mi contenedor
FROM ubuntu:24.04

# Definicion del directorio dentro del contenedor, para que todos los comandos sean ejecutados
# en este directorio a no ser que se indique lo contrario
WORKDIR /app

# Se copiaran todos los archivos y carpetas del diurectorio de mi proyecto local
# (donde se encuentra este dockerfile) a el directorio /app dentro del contenedor docker.
# Asi nuestro codigo fuente esta en nuestro entorno de desarrollo.
COPY . /app

## Instala CMake, build-essential, y las dependencias comunes de Crow
RUN apt-get update && \
    apt-get install -y \
    cmake \
    build-essential \
    libasio-dev \
    libssl-dev \
    libboost-dev && \ 
    rm -rf /var/lib/apt/lists/*

# Create a crea los directorios para la construccion del proyecto
RUN mkdir build && \
    cd build && \
    cmake .. && \
    make

# Define el comando para ejecutar la aplicación de servidor C++
CMD ["./build/CPUSimulator"]