<template>
  <div id="app">
    <div class="container">
      <AppHeader :algorithm="simulationConfig.algorithm" />
      
      <div class="content">
        <!-- Formulario para agregar procesos -->
        <ProcessForm 
          @add-process="addProcess"
          @clear-processes="clearProcesses"
          @algorithm-changed="updateSimulationConfig"
        />

        <!-- Lista de procesos -->
        <ProcessList 
          :processes="processes"
          :algorithm="simulationConfig.algorithm"
          :quantum="simulationConfig.quantum"
          @start-simulation="startSimulation"
          v-if="processes.length > 0"
        />

        <!-- Área de simulación -->
        <SimulationArea 
          :simulation-running="simulationRunning"
          :process-states="processStates"
          :message="message"
          :message-type="messageType"
          :algorithm="simulationConfig.algorithm"
          v-if="simulationRunning || processStates.length > 0"
        />
      </div>
    </div>

    <!-- Indicador de conexión -->
    <ConnectionStatus :ws-connected="wsConnected" />
  </div>
</template>

<script>
import AppHeader from './components/AppHeader.vue'
import ProcessForm from './components/ProcessForm.vue'
import ProcessList from './components/ProcessList.vue'
import SimulationArea from './components/SimulationArea.vue'
import ConnectionStatus from './components/ConnectionStatus.vue'

export default {
  name: 'App',
  components: {
    AppHeader,
    ProcessForm,
    ProcessList,
    SimulationArea,
    ConnectionStatus
  },
  data() {
    return {
      // Lista de procesos
      processes: [],
      
      // Configuración de simulación
      simulationConfig: {
        algorithm: 'FCFS',
        quantum: 2
      },
      
      // Estados de simulación
      processStates: [],
      simulationRunning: false,
      
      // WebSocket
      ws: null,
      wsConnected: false,
      
      // Mensajes
      message: '',
      messageType: 'alert-success'
    }
  },
  
  mounted() {
    this.connectWebSocket();
  },
  
  methods: {
    updateSimulationConfig(config) {
      this.simulationConfig = { ...config };
      console.log('Configuración actualizada:', this.simulationConfig);
    },
    
    addProcess(processData) {
      // Verificar si el ID ya existe
      if (this.processes.some(p => p.id === processData.id)) {
        this.showMessage('El ID del proceso ya existe', 'alert-error');
        return false;
      }
      
      // Agregar proceso
      this.processes.push({...processData});
      this.showMessage('Proceso agregado exitosamente', 'alert-success');
      return true;
    },
    
    clearProcesses() {
      this.processes = [];
      this.processStates = [];
      this.simulationRunning = false;
      this.showMessage('Lista de procesos limpiada', 'alert-success');
    },
    
    async startSimulation() {
      if (this.processes.length === 0) {
        this.showMessage('No hay procesos para simular', 'alert-error');
        return;
      }
      
      this.simulationRunning = true;
      this.processStates = [];
      
      // Preparar datos para enviar al backend
      const simulationData = {
        processes: this.processes,
        algorithm: this.simulationConfig.algorithm,
        quantum: this.simulationConfig.quantum
      };
      
      try {
        const response = await fetch('http://localhost:8081/api/processes', {
          method: 'POST',
          headers: {
            'Content-Type': 'application/json'
          },
          body: JSON.stringify(simulationData)
        });
        
        if (response.ok) {
          this.showMessage(
            `Simulación ${this.getAlgorithmName(this.simulationConfig.algorithm)} iniciada`, 
            'alert-success'
          );
        } else {
          throw new Error('Error al iniciar simulación');
        }
      } catch (error) {
        this.showMessage('Error al conectar con el servidor: ' + error.message, 'alert-error');
        this.simulationRunning = false;
      }
    },
    
    getAlgorithmName(algorithm) {
      const names = {
        'FCFS': 'FCFS',
        'SJF': 'SJF',
        'RR': 'Round Robin',
        'PRIORITY': 'Priority'
      };
      return names[algorithm] || algorithm;
    },
    
    connectWebSocket() {
      try {
        this.ws = new WebSocket('ws://localhost:8081/ws');
        
        this.ws.onopen = () => {
          this.wsConnected = true;
          console.log('WebSocket conectado');
        };
        
        this.ws.onmessage = (event) => {
          try {
            const processData = JSON.parse(event.data);
            this.updateProcessState(processData);
          } catch (error) {
            console.error('Error al parsear mensaje:', error);
          }
        };
        
        this.ws.onclose = () => {
          this.wsConnected = false;
          console.log('WebSocket desconectado');
          // Reconectar después de 3 segundos
          setTimeout(() => {
            this.connectWebSocket();
          }, 3000);
        };
        
        this.ws.onerror = (error) => {
          console.error('Error en WebSocket:', error);
        };
      } catch (error) {
        console.error('Error al conectar WebSocket:', error);
      }
    },
    
    updateProcessState(processData) {
      const existingIndex = this.processStates.findIndex(p => p.id === processData.id);
      
      if (existingIndex !== -1) {
        // Actualizar proceso existente
        this.processStates[existingIndex] = processData;
      } else {
        // Agregar nuevo proceso
        this.processStates.push(processData);
      }
      
      // Verificar si la simulación terminó
      if (processData.state === 3) { // TERMINATED
        const allTerminated = this.processStates.every(p => p.state === 3);
        if (allTerminated) {
          this.simulationRunning = false;
          this.showMessage('Simulación completada', 'alert-success');
        }
      }
    },
    
    showMessage(text, type) {
      this.message = text;
      this.messageType = type;
      setTimeout(() => {
        this.message = '';
      }, 3000);
    }
  }
}
</script>

<style>
* {
  margin: 0;
  padding: 0;
  box-sizing: border-box;
}

body {
  font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
  background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
  min-height: 100vh;
  padding: 20px;
}

.container {
  max-width: 100rem;
  margin: 0 auto;
  background: white;
  border-radius: 20px;
  box-shadow: 0 20px 40px rgba(0,0,0,0.1);
  overflow: hidden;
}

.content {
  padding: 40px;
}

.alert {
  padding: 15px;
  border-radius: 8px;
  margin-bottom: 20px;
  font-weight: 500;
}

.alert-success {
  background: #d4edda;
  color: #155724;
  border: 1px solid #c3e6cb;
}

.alert-error {
  background: #f8d7da;
  color: #721c24;
  border: 1px solid #f5c6cb;
}
</style>