# Se usara Ubuntu 24.04 LTS Sistema Operativo base para mi contenedor
FROM ubuntu:24.04

# Evita prompts interactivos durante la instalación
ENV DEBIAN_FRONTEND=noninteractive

# Definicion del directorio dentro del contenedor
WORKDIR /app

# Instala CMake, build-essential, y las dependencias comunes de Crow
RUN apt-get update && \
    apt-get install -y \
    cmake \
    build-essential \
    libasio-dev \
    libssl-dev \
    libboost-dev && \ 
    rm -rf /var/lib/apt/lists/*

# Copia el código fuente
COPY . .

# Construye el proyecto
RUN mkdir -p build && \
    cd build && \
    cmake .. && \
    make -j$(nproc)

# Expone el puerto
EXPOSE 8081

# Ejecuta la aplicación
CMD ["./build/CPUSimulator"]