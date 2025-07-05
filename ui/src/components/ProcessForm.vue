<template>
  <div class="form-section">
    <h2>📝 Agregar Proceso</h2>
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
</template>

<script>
export default {
  name: 'ProcessForm',
  data() {
    return {
      newProcess: {
        id: 1,
        arrivalTime: 0,
        burstTime: 1,
        priority: 1
      }
    }
  },
  methods: {
    addProcess() {
      // Validar datos
      if (!this.newProcess.id || 
          this.newProcess.arrivalTime < 0 || 
          this.newProcess.burstTime <= 0 || 
          this.newProcess.priority <= 0) {
        this.$emit('show-message', 'Por favor, complete todos los campos con valores válidos', 'alert-error');
        return;
      }
      
      // Emitir evento al componente padre
      const success = this.$emit('add-process', {...this.newProcess});
      
      // Si se agregó exitosamente, incrementar ID
      if (success !== false) {
        this.newProcess.id++;
      }
    },
    
    clearProcesses() {
      this.newProcess.id = 1;
      this.$emit('clear-processes');
    }
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
  margin-bottom: 20px;
  font-size: 1.5em;
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
</style>