<!-- Actualizar ProcessForm.vue -->
<template>
  <div class="form-section">
    <h2>📝 Configurar Simulación</h2>
    
    <!-- Selector de algoritmo compacto -->
    <div class="algorithm-selector">
      <h3>🔧 Algoritmo de Planificación</h3>
      <div class="algorithm-compact">
        <select 
          v-model="selectedAlgorithm" 
          @change="selectAlgorithm(selectedAlgorithm)"
          class="algorithm-select"
        >
          <option 
            v-for="algorithm in algorithms" 
            :key="algorithm.id"
            :value="algorithm.id"
          >
            {{ algorithm.icon }} {{ algorithm.name }} - {{ algorithm.description }}
          </option>
        </select>
        
        <!--Botón de comparación -->
        <button 
          class="btn btn-comparison" 
          @click="showComparison"
          title="Ver comparación de algoritmos"
        >
          📊 Comparar Algoritmos
        </button>
      </div>
    </div>

    <!-- Quantum para Round Robin -->
    <div v-if="selectedAlgorithm === 'RR'" class="quantum-section">
      <div class="form-group">
        <label>⏱️ Quantum (Round Robin):</label>
        <input 
          type="number" 
          v-model.number="quantum" 
          placeholder="Ej: 2"
          min="1"
          class="quantum-input"
        >
      </div>
    </div>

    <!--Sección de acciones rápidas -->
    <div class="quick-actions">
      
      <button 
        class="btn btn-load-processes" 
        @click="loadProcessesFromFile"
        title="Cargar procesos desde archivo"
      >
        📁 Cargar Procesos
      </button>
      
      <button 
        class="btn btn-clear-metrics" 
        @click="clearMetrics"
        title="Limpiar métricas anteriores"
      >
        🗑️ Limpiar Métricas
      </button>
    </div>

    <!-- Formulario para agregar procesos -->
    <div class="process-form">
      <h3>➕ Agregar Proceso</h3>
      <div class="form-grid">
        <div class="form-group">
          <label>ID del Proceso:</label>
          <input 
            type="number" 
            v-model.number="newProcess.id" 
            placeholder="Ej: 1"
            min="1"
          >
        </div>
        <div class="form-group">
          <label>Tiempo de Llegada:</label>
          <input 
            type="number" 
            v-model.number="newProcess.arrivalTime" 
            placeholder="Ej: 0"
            min="0"
          >
        </div>
        <div class="form-group">
          <label>Tiempo de Ráfaga:</label>
          <input 
            type="number" 
            v-model.number="newProcess.burstTime" 
            placeholder="Ej: 5"
            min="1"
          >
        </div>
        <div class="form-group">
          <label>Prioridad:</label>
          <input 
            type="number" 
            v-model.number="newProcess.priority" 
            placeholder="Ej: 1"
            min="1"
          >
        </div>
      </div>
      
      <div class="button-group">
        <button class="btn btn-primary" @click="addProcess">
          ➕ Agregar Proceso
        </button>
        <button class="btn btn-secondary" @click="clearProcesses">
          🗑️ Limpiar Lista
        </button>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  name: 'ProcessForm',
  props: {
    hasProcesses: {
      type: Boolean,
      default: false
    }
  },
  data() {
    return {
      selectedAlgorithm: 'FCFS',
      quantum: 2,
      algorithms: [
        {
          id: 'FCFS',
          name: 'FCFS',
          icon: '⏰',
          description: 'First Come First Served'
        },
        {
          id: 'SJF',
          name: 'SJF',
          icon: '⚡',
          description: 'Shortest Job First'
        },
        {
          id: 'RR',
          name: 'Round Robin',
          icon: '🔄',
          description: 'Tiempo compartido con quantum'
        },
        {
          id: 'PRIORITY',
          name: 'Priority',
          icon: '🎯',
          description: 'Planificación por prioridad'
        }
      ],
      newProcess: {
        id: 1,
        arrivalTime: 0,
        burstTime: 1,
        priority: 1
      }
    }
  },
  methods: {
    selectAlgorithm(algorithmId) {
      this.selectedAlgorithm = algorithmId;
      this.$emit('algorithm-changed', {
        algorithm: algorithmId,
        quantum: this.quantum
      });
    },
    
    addProcess() {
      if (!this.newProcess.id || 
          this.newProcess.arrivalTime < 0 || 
          this.newProcess.burstTime <= 0) {
        this.$emit('show-message', 'Por favor, complete todos los campos con valores válidos', 'alert-error');
        return;
      }
      
      if (this.selectedAlgorithm === 'PRIORITY' && this.newProcess.priority <= 0) {
        this.$emit('show-message', 'La prioridad debe ser mayor a 0', 'alert-error');
        return;
      }
      
      const processData = {
        ...this.newProcess,
        priority: this.selectedAlgorithm === 'PRIORITY' ? this.newProcess.priority : 1
      };
      
      const success = this.$emit('add-process', processData);
      
      if (success !== false) {
        this.newProcess.id++;
      }
    },
    
    clearProcesses() {
      this.newProcess.id = 1;
      this.$emit('clear-processes');
    },

    showComparison() {
      this.$emit('show-comparison');
    },
    
    clearMetrics() {
      this.$emit('clear-metrics');
    },
    
    //Cargar procesos desde archivo
    async loadProcessesFromFile() {
      try {
        this.$emit('show-message', 'Cargando procesos desde archivo...', 'alert-success');
        
        const response = await fetch('http://localhost:8081/api/load-processes');
        
        if (response.ok) {
          const data = await response.json();
          
          // Emitir evento para cargar procesos en el componente padre
          this.$emit('load-processes', data.processes);
          
          this.$emit('show-message', 
            `${data.count} procesos cargados desde ${data.source}`, 
            'alert-success'
          );
          
          // Resetear ID para nuevos procesos
          if (data.processes.length > 0) {
            const maxId = Math.max(...data.processes.map(p => p.id));
            this.newProcess.id = maxId + 1;
          }
          
        } else {
          const errorData = await response.json();
          this.$emit('show-message', 
            errorData.error || 'Error al cargar procesos', 
            'alert-error'
          );
        }
        
      } catch (error) {
        this.$emit('show-message', 
          'Error de conexión al cargar procesos: ' + error.message, 
          'alert-error'
        );
      }
    }
  },
  
  watch: {
    quantum(newValue) {
      if (this.selectedAlgorithm === 'RR') {
        this.$emit('algorithm-changed', {
          algorithm: this.selectedAlgorithm,
          quantum: newValue
        });
      }
    }
  },
  
  mounted() {
    this.$emit('algorithm-changed', {
      algorithm: this.selectedAlgorithm,
      quantum: this.quantum
    });
  }
}
</script>

<style scoped>
.form-section {
  background: #f8f9fa;
  padding: 30px;
  border-radius: 15px;
  margin-bottom: 30px;
  box-shadow: 0 5px 15px rgba(0,0,0,0.05);
}

.form-section h2 {
  color: #333;
  margin-bottom: 25px;
  font-size: 1.5em;
}

.form-section h3 {
  color: #555;
  margin-bottom: 15px;
  font-size: 1.2em;
}

.algorithm-selector {
  margin-bottom: 25px;
}

.algorithm-compact {
  display: flex;
  align-items: center;
  gap: 15px;
  flex-wrap: wrap;
}

.algorithm-select {
  flex: 1;
  max-width: 400px;
  padding: 12px 16px;
  border: 2px solid #e9ecef;
  border-radius: 8px;
  font-size: 16px;
  font-weight: 500;
  background: white;
  cursor: pointer;
  transition: all 0.3s ease;
}

.algorithm-select:focus {
  outline: none;
  border-color: #667eea;
  box-shadow: 0 0 0 3px rgba(102, 126, 234, 0.1);
}

.algorithm-select:hover {
  border-color: #667eea;
}

.btn-comparison {
  background: linear-gradient(135deg, #ff6b6b 0%, #ee5a24 100%);
  color: white;
  padding: 12px 20px;
  border: none;
  border-radius: 8px;
  font-size: 14px;
  font-weight: 600;
  cursor: pointer;
  transition: all 0.3s ease;
  white-space: nowrap;
}

.btn-comparison:hover {
  transform: translateY(-2px);
  box-shadow: 0 5px 15px rgba(255, 107, 107, 0.3);
}

.quick-actions {
  display: flex;
  gap: 15px;
  margin-bottom: 25px;
  flex-wrap: wrap;
}

.btn-load-processes {
  background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
  color: white;
  padding: 12px 20px;
  border: none;
  border-radius: 8px;
  font-size: 14px;
  font-weight: 600;
  cursor: pointer;
  transition: all 0.3s ease;
  white-space: nowrap;
}

.btn-load-processes:hover {
  transform: translateY(-2px);
  box-shadow: 0 5px 15px rgba(102, 126, 234, 0.3);
}

.btn-clear-metrics {
  background: #6c757d;
  color: white;
  padding: 12px 20px;
  border: none;
  border-radius: 8px;
  font-size: 14px;
  font-weight: 600;
  cursor: pointer;
  transition: all 0.3s ease;
}

.btn-clear-metrics:hover {
  background: #5a6268;
  transform: translateY(-2px);
}

.quantum-section {
  background: #e3f2fd;
  padding: 15px;
  border-radius: 10px;
  margin-bottom: 25px;
  border-left: 4px solid #2196f3;
}

.quantum-section .form-group {
  margin-bottom: 0;
  display: flex;
  align-items: center;
  gap: 15px;
}

.quantum-section label {
  margin-bottom: 0;
  min-width: 180px;
}

.quantum-input {
  max-width: 100px;
}

.process-form {
  background: white;
  padding: 20px;
  border-radius: 12px;
  box-shadow: 0 2px 8px rgba(0,0,0,0.1);
}

.form-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
  gap: 20px;
  margin-bottom: 20px;
}

.form-group {
  display: flex;
  flex-direction: column;
}

.form-group label {
  margin-bottom: 5px;
  font-weight: 600;
  color: #555;
}

.form-group input {
  padding: 12px;
  border: 2px solid #e9ecef;
  border-radius: 8px;
  font-size: 16px;
  transition: all 0.3s ease;
}

.form-group input:focus {
  outline: none;
  border-color: #667eea;
  box-shadow: 0 0 0 3px rgba(102, 126, 234, 0.1);
}

.button-group {
  display: flex;
  gap: 15px;
  justify-content: center;
}

.btn {
  padding: 12px 30px;
  border: none;
  border-radius: 8px;
  font-size: 16px;
  font-weight: 600;
  cursor: pointer;
  transition: all 0.3s ease;
  text-transform: uppercase;
  letter-spacing: 1px;
}

.btn-primary {
  background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
  color: white;
}

.btn-primary:hover {
  transform: translateY(-2px);
  box-shadow: 0 5px 15px rgba(102, 126, 234, 0.4);
}

.btn-secondary {
  background: #6c757d;
  color: white;
}

.btn-secondary:hover {
  background: #5a6268;
  transform: translateY(-2px);
}

@media (max-width: 768px) {
  .algorithm-compact {
    flex-direction: column;
    align-items: stretch;
  }
  
  .algorithm-select {
    max-width: 100%;
  }
  
  .quick-actions {
    flex-direction: column;
  }
  
  .btn-run-all {
    min-width: auto;
  }
  
  .quantum-section .form-group {
    flex-direction: column;
    align-items: flex-start;
    gap: 8px;
  }
  
  .quantum-section label {
    min-width: auto;
  }
}
</style>