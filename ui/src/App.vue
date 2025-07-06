<template>
  <div id="app">
    <div class="container">
      <AppHeader :algorithm="simulationConfig.algorithm" />
      
      <div class="content">
        <!-- Formulario para agregar procesos -->
        <ProcessForm 
          :has-processes="processes.length > 0"
          @add-process="addProcess"
          @clear-processes="clearProcesses"
          @algorithm-changed="updateSimulationConfig"
          @show-comparison="showComparisonPanel"
          @run-all-algorithms="runAllAlgorithms"
          @clear-metrics="clearMetrics"
          @show-message="showMessage"
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

        <!-- Mostrar métricas tabulares -->
        <MetricsTable 
          :metrics="currentMetrics"
          v-if="currentMetrics"
        />
      </div>
    </div>

    <!-- Panel de comparación (modal) -->
    <ComparisonPanel 
      :comparison-data="comparisonData"
      v-if="showComparison"
      @close="closeComparisonPanel"
    />

    <!-- Overlay para el modal -->
    <div class="modal-overlay" v-if="showComparison" @click="closeComparisonPanel"></div>

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
import MetricsTable from './components/MetricsTable.vue'
import ComparisonPanel from './components/ComparisonPanel.vue'

export default {
  name: 'App',
  components: {
    AppHeader,
    ProcessForm,
    ProcessList,
    SimulationArea,
    ConnectionStatus,
    MetricsTable,
    ComparisonPanel
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
      messageType: 'alert-success',

      // Métricas
      currentMetrics: null,
      
      // NUEVO: Panel de comparación
      comparisonData: null,
      showComparison: false
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
      if (this.processes.some(p => p.id === processData.id)) {
        this.showMessage('El ID del proceso ya existe', 'alert-error');
        return false;
      }
      
      this.processes.push({...processData});
      this.showMessage('Proceso agregado exitosamente', 'alert-success');
      return true;
    },
    
    clearProcesses() {
      this.processes = [];
      this.processStates = [];
      this.simulationRunning = false;
      this.showMessage('Lista de procesos limpiada', 'alert-success');
      this.currentMetrics = null;
    },
    
    async startSimulation() {
      if (this.processes.length === 0) {
        this.showMessage('No hay procesos para simular', 'alert-error');
        return;
      }
      
      this.simulationRunning = true;
      this.processStates = [];
      this.currentMetrics = null;
      
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

    async runAllAlgorithms() {
      if (this.processes.length === 0) {
        this.showMessage('No hay procesos para comparar', 'alert-error');
        return;
      }
      
      this.simulationRunning = true;
      this.processStates = [];
      this.currentMetrics = null;
      
      const simulationData = {
        processes: this.processes,
        quantum: this.simulationConfig.quantum
      };
      
      try {
        const response = await fetch('http://localhost:8081/api/run-all-algorithms', {
          method: 'POST',
          headers: {
            'Content-Type': 'application/json'
          },
          body: JSON.stringify(simulationData)
        });
        
        if (response.ok) {
          this.showMessage('Ejecutando todos los algoritmos...', 'alert-success');
        } else {
          throw new Error('Error al ejecutar comparación');
        }
      } catch (error) {
        this.showMessage('Error al conectar con el servidor: ' + error.message, 'alert-error');
        this.simulationRunning = false;
      }
    },

    async showComparisonPanel() {
      try {
        const response = await fetch('http://localhost:8081/api/comparison-panel');
        if (response.ok) {
          // Los datos llegarán por WebSocket
          this.showMessage('Cargando panel de comparación...', 'alert-success');
        } else {
          this.showMessage('No hay métricas disponibles para comparar', 'alert-error');
        }
      } catch (error) {
        this.showMessage('Error al cargar comparación: ' + error.message, 'alert-error');
      }
    },

    closeComparisonPanel() {
      this.showComparison = false;
      this.comparisonData = null;
    },

    async clearMetrics() {
      try {
        const response = await fetch('http://localhost:8081/api/clear-metrics', {
          method: 'POST'
        });
        if (response.ok) {
          this.showMessage('Métricas limpiadas', 'alert-success');
        }
      } catch (error) {
        this.showMessage('Error al limpiar métricas: ' + error.message, 'alert-error');
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
            const data = JSON.parse(event.data);
            this.handleWebSocketMessage(data);
          } catch (error) {
            console.error('Error al parsear mensaje:', error);
          }
        };
        
        this.ws.onclose = () => {
          this.wsConnected = false;
          console.log('WebSocket desconectado');
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

    handleWebSocketMessage(data) {
      switch(data.type) {
        case 'METRICS_COMPLETE':
          this.handleMetricsComplete(data);
          break;
        
        case 'COMPARISON_PANEL':
          this.handleComparisonPanel(data);
          break;
        
        default:
          this.updateProcessState(data);
          break;
      }
    },

    handleMetricsComplete(metricsData) {
      this.currentMetrics = metricsData;
      this.simulationRunning = false;
      
      this.showMessage(
        `Simulación ${metricsData.algorithm} completada. CPU: ${metricsData.cpuUtilization.toFixed(1)}%`,
        'alert-success'
      );
      
      console.log('Métricas recibidas:', metricsData);
    },

    handleComparisonPanel(comparisonData) {
      this.comparisonData = comparisonData;
      this.showComparison = true;
      this.simulationRunning = false;
      
      this.showMessage(
        `Panel de comparación cargado. Mejor: ${comparisonData.overallBest}`,
        'alert-success'
      );
      
      console.log('Comparación recibida:', comparisonData);
    },
    
    updateProcessState(processData) {
      const existingIndex = this.processStates.findIndex(p => p.id === processData.id);
      
      if (existingIndex !== -1) {
        this.processStates[existingIndex] = processData;
      } else {
        this.processStates.push(processData);
      }
      
      if (processData.state === 3) { // TERMINATED
        const allTerminated = this.processStates.every(p => p.state === 3);
        if (allTerminated) {
          console.log('Todos los procesos terminaron, esperando métricas...');
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

.modal-overlay {
  position: fixed;
  top: 0;
  left: 0;
  width: 100vw;
  height: 100vh;
  background: rgba(0, 0, 0, 0.5);
  z-index: 999;
  backdrop-filter: blur(5px);
}
</style>