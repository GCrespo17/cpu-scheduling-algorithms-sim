<template>
  <div class="process-card" :class="getStateClass(process.state)">
    <div class="process-header">
      <h4>🔧 P{{ process.id }}</h4>
      <span class="state-badge" :class="'state-' + getStateClass(process.state)">
        {{ getStateName(process.state) }}
      </span>
    </div>
    
    <div class="metrics-grid">
      <div class="metric">
        <div class="metric-label">Llegada</div>
        <div class="metric-value">{{ process.arrival_time }}s</div>
      </div>
      <div class="metric">
        <div class="metric-label">Ráfaga</div>
        <div class="metric-value">{{ process.burst_time }}s</div>
      </div>
      <div class="metric" v-if="process.start_time >= 0">
        <div class="metric-label">Inicio</div>
        <div class="metric-value">{{ process.start_time }}s</div>
      </div>
      <div class="metric" v-if="process.completion_time > 0">
        <div class="metric-label">Fin</div>
        <div class="metric-value">{{ process.completion_time }}s</div>
      </div>
      <div class="metric" v-if="process.turnaround_time > 0">
        <div class="metric-label">Retorno</div>
        <div class="metric-value">{{ process.turnaround_time }}s</div>
      </div>
      <div class="metric" v-if="process.waiting_time >= 0">
        <div class="metric-label">Espera</div>
        <div class="metric-value">{{ process.waiting_time }}s</div>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  name: 'ProcessCard',
  props: {
    process: {
      type: Object,
      required: true
    }
  },
  methods: {
    getStateClass(state) {
      const stateMap = {
        0: 'new',
        1: 'ready', 
        2: 'running',
        3: 'terminated'
      };
      return stateMap[state] || 'new';
    },
    
    getStateName(state) {
      const stateNames = {
        0: 'Nuevo',
        1: 'Listo',
        2: 'Ejecutando',
        3: 'Terminado'
      };
      return stateNames[state] || 'Desconocido';
    }
  }
}
</script>

<style scoped>
.process-card {
  background: white;
  padding: 12px;
  border-radius: 10px;
  border-left: 4px solid #ddd;
  box-shadow: 0 3px 10px rgba(0,0,0,0.1);
  transition: all 0.3s ease;
  min-width: 200px;
  max-width: 280px;
}

.process-card.new {
  border-left-color: #6c757d;
  background: linear-gradient(135deg, #f8f9fa 0%, #e9ecef 100%);
}

.process-card.ready {
  border-left-color: #ffc107;
  background: linear-gradient(135deg, #fff3cd 0%, #ffeaa7 100%);
  transform: scale(1.02);
}

.process-card.running {
  border-left-color: #28a745;
  background: linear-gradient(135deg, #d4edda 0%, #a3e9a4 100%);
  transform: scale(1.05);
  animation: pulse 2s infinite;
}

.process-card.terminated {
  border-left-color: #dc3545;
  background: linear-gradient(135deg, #f8d7da 0%, #f1b0b7 100%);
  opacity: 0.8;
}

@keyframes pulse {
  0% { box-shadow: 0 3px 10px rgba(0,0,0,0.1); }
  50% { box-shadow: 0 6px 20px rgba(40, 167, 69, 0.3); }
  100% { box-shadow: 0 3px 10px rgba(0,0,0,0.1); }
}

.process-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 10px;
}

.process-header h4 {
  margin: 0;
  font-size: 0.9em;
  font-weight: 700;
}

.state-badge {
  display: inline-block;
  padding: 2px 8px;
  border-radius: 12px;
  font-size: 0.7em;
  font-weight: 600;
  text-transform: uppercase;
  letter-spacing: 0.5px;
}

.state-new { background: #6c757d; color: white; }
.state-ready { background: #ffc107; color: #212529; }
.state-running { background: #28a745; color: white; }
.state-terminated { background: #dc3545; color: white; }

.metrics-grid {
  display: grid;
  grid-template-columns: repeat(3, 1fr);
  gap: 6px;
}

.metric {
  background: rgba(255,255,255,0.8);
  padding: 4px 6px;
  border-radius: 6px;
  text-align: center;
  min-height: 35px;
  display: flex;
  flex-direction: column;
  justify-content: center;
}

.metric-label {
  font-size: 0.6em;
  color: #666;
  text-transform: uppercase;
  letter-spacing: 0.3px;
  margin-bottom: 2px;
}

.metric-value {
  font-size: 0.8em;
  font-weight: 600;
  color: #333;
}
</style>