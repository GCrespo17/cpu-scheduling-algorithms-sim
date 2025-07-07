# cpu-scheduling-algorithms-sim
# Simulador de Algoritmos de Planificación de CPU

Un simulador interactivo de algoritmos de planificación de CPU construido con **C++** y **Vue.js**, que permite visualizar y comparar el rendimiento de diferentes algoritmos de scheduling en tiempo real.

## Características

- **Simulación en Tiempo Real**: Visualiza la ejecución de procesos paso a paso
- **Múltiples Algoritmos**: Soporta FCFS, SJF, Round Robin y Priority Scheduling
- **Interfaz Interactiva**: Interfaz web moderna y responsiva
- **Métricas Detalladas**: Calcula tiempo de respuesta, espera, turnaround y utilización de CPU
- **Panel de Comparación**: Compara el rendimiento de diferentes algoritmos
- **Gráficos de Gantt**: Visualización temporal de la ejecución de procesos
- **Carga de Archivos**: Importa procesos desde archivos de texto
- **WebSocket**: Comunicación en tiempo real entre frontend y backend

## Tecnologías

### Backend
- **C++17** con framework **Crow** para utilizarlo como servidor
- **CMake** para construcción del proyecto
- **WebSocket** para comunicación en tiempo real
- **JSON** para intercambio de datos

### Frontend
- **Vue.js 3** con Composition API
- **Vite** como bundler y servidor de desarrollo
- **CSS3** con diseño responsivo
- **WebSocket Client** para conexión en tiempo real

### Infraestructura
- **Docker** y **Docker Compose** para contenedorización
- **Ubuntu 24.04 LTS** como sistema base

## Inicio Rápido

### Prerequisitos

- Docker y Docker Compose instalados
- Git para clonar el repositorio

### Instalación

1. **Clonar el repositorio**
```bash
git clone https://github.com/GCrespo17/cpu-scheduling-algorithms-sim.git
cd cpu-scheduler-simulator
```

2. **Ejecutar con Docker Compose**
```bash
docker-compose up -d
```

3. **Acceder a la aplicación**
   - Frontend: http://localhost:5173
   - Backend API: http://localhost:8081

## Estructura del Proyecto

```
├── docker-compose.yml          # Configuración de servicios
├── core/                       # Backend C++
│   ├── Dockerfile
│   ├── CMakeLists.txt
│   ├── src/
│   │   ├── server.cpp         # Servidor principal
│   │   ├── scheduler.cpp      # Coordinador de algoritmos
│   │   ├── Fcfs.cpp          # Algoritmo FCFS
│   │   ├── sjf.cpp           # Algoritmo SJF
│   │   ├── round_robin.cpp   # Algoritmo Round Robin
│   │   ├── priority.cpp      # Algoritmo Priority
│   │   ├── metric.cpp        # Cálculo de métricas
│   │   └── json_sender.cpp   # Comunicación JSON/WebSocket
│   └── include/              # Headers C++
└── ui/                       # Frontend Vue.js
    ├── Dockerfile
    ├── package.json
    ├── src/
    │   ├── App.vue           # Componente principal
    │   └── components/       # Componentes Vue
    └── public/
```

## Algoritmos Implementados

### 1. First Come First Served (FCFS)
- **Descripción**: Los procesos se ejecutan en orden de llegada
- **Ventajas**: Simple de implementar, predecible
- **Desventajas**: Puede sufrir el "convoy effect"

### 2. Shortest Job First (SJF) No apropiativo
- **Descripción**: Ejecuta primero el proceso con menor tiempo de ráfaga
- **Ventajas**: Minimiza el tiempo promedio de espera
- **Desventajas**: Puede causar starvation en procesos largos

### 3. Round Robin (RR)
- **Descripción**: Cada proceso recibe un quantum de tiempo fijo
- **Ventajas**: Justo, buena respuesta interactiva
- **Desventajas**: Overhead por cambios de contexto

### 4. Priority Scheduling Sin desalojo
- **Descripción**: Los procesos se ejecutan según su prioridad
- **Ventajas**: Control fino sobre la ejecución
- **Desventajas**: Riesgo de starvation

## Métricas Calculadas

- **Tiempo de Turnaround**: Tiempo total desde llegada hasta completación
- **Tiempo de Espera**: Tiempo esperando en la cola de ready
- **Tiempo de Respuesta**: Tiempo desde llegada hasta primera ejecución
- **Utilización de CPU**: Porcentaje de tiempo que la CPU está ocupada

## API Endpoints

### POST `/api/processes`
Inicia una simulación con los procesos proporcionados.

**Request Body:**
```json
{
  "algorithm": "FCFS|SJF|RR|PRIORITY",
  "quantum": 2,
  "processes": [
    {
      "id": 1,
      "arrivalTime": 0,
      "burstTime": 5,
      "priority": 1
    }
  ]
}
```

### GET `/api/comparison-panel`
Obtiene el panel de comparación de algoritmos ejecutados.

### POST `/api/clear-metrics`
Limpia todas las métricas almacenadas.

### GET `/api/load-processes`
Carga procesos desde archivo `processes.txt`.

## Formato de Archivo de Procesos

Crea un archivo `processes.txt` en el directorio `core/files/`:

```
1 0 5 1
2 1 3 2
3 2 8 1
4 3 6 3
```

Formato: `ID ArrivalTime BurstTime Priority`

## Cómo Usar

1. **Agregar Procesos**: Usa el formulario para agregar procesos individualmente
2. **Cargar desde Archivo**: Importa procesos desde un archivo de texto
3. **Seleccionar Algoritmo**: Elige entre FCFS, SJF, Round Robin o Priority
4. **Configurar Quantum**: Para Round Robin, establece el valor del quantum
5. **Iniciar Simulación**: Observa la ejecución en tiempo real
6. **Ver Métricas**: Analiza las métricas de rendimiento
7. **Comparar Algoritmos**: Usa el panel de comparación para evaluar diferentes enfoques

## Desarrollo

### Ejecutar en Modo Desarrollo

**Backend:**
```bash
cd core
mkdir build && cd build
cmake ..
make
./CPUSimulator
```

**Frontend:**
```bash
cd ui
npm install
npm run dev
```
## Solución de Problemas

### Puerto ya en uso
```bash
docker-compose down
sudo lsof -i :8081
sudo kill -9 <PID>
```

### Problemas de conexión WebSocket
- Verifica que ambos contenedores estén ejecutándose
- Revisa los logs: `docker-compose logs`

### Frontend no se actualiza
- Limpia el caché del navegador
- Reinicia el contenedor: `docker-compose restart ui`

## Propósito Educativo
Este programa fue diseñado para la materia Sistemas Operativos.
Universidad Católica Andrés Bello.
