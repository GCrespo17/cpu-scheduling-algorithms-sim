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
        <div class="form-group" v-if="selectedAlgorithm === 'PRIORITY'">
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
      // Emitir el cambio de algoritmo al componente padre
      this.$emit('algorithm-changed', {
        algorithm: algorithmId,
        quantum: this.quantum
      });
    },
    
    addProcess() {
      // Validar datos básicos
      if (!this.newProcess.id || 
          this.newProcess.arrivalTime < 0 || 
          this.newProcess.burstTime <= 0) {
        this.$emit('show-message', 'Por favor, complete todos los campos con valores válidos', 'alert-error');
        return;
      }
      
      // Validar prioridad para algoritmo PRIORITY
      if (this.selectedAlgorithm === 'PRIORITY' && this.newProcess.priority <= 0) {
        this.$emit('show-message', 'La prioridad debe ser mayor a 0', 'alert-error');
        return;
      }
      
      // Preparar datos del proceso
      const processData = {
        ...this.newProcess,
        // Solo incluir prioridad si el algoritmo la requiere
        priority: this.selectedAlgorithm === 'PRIORITY' ? this.newProcess.priority : 1
      };
      
      // Emitir evento al componente padre
      const success = this.$emit('add-process', processData);
      
      // Si se agregó exitosamente, incrementar ID
      if (success !== false) {
        this.newProcess.id++;
      }
    },
    
    clearProcesses() {
      this.newProcess.id = 1;
      this.$emit('clear-processes');
    }
  },
  
  watch: {
    quantum(newValue) {
      // Notificar cambio de quantum si estamos en Round Robin
      if (this.selectedAlgorithm === 'RR') {
        this.$emit('algorithm-changed', {
          algorithm: this.selectedAlgorithm,
          quantum: newValue
        });
      }
    }
  },
  
  mounted() {
    // Emitir el algoritmo inicial
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

/* Selector de algoritmo compacto */
.algorithm-selector {
  margin-bottom: 25px;
}

.algorithm-compact {
  display: flex;
  align-items: center;
  gap: 10px;
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

.algorithm-select option {
  padding: 10px;
  font-size: 14px;
}

/* Sección de quantum compacta */
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

/* Formulario de procesos */
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

/* Responsivo */
@media (max-width: 768px) {
  .quantum-section .form-group {
    flex-direction: column;
    align-items: flex-start;
    gap: 8px;
  }
  
  .quantum-section label {
    min-width: auto;
  }
  
  .algorithm-select {
    max-width: 100%;
  }
}
</style>