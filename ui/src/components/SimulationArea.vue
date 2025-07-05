<template>
  <div class="simulation-area">
    <h3>🔄 Simulación en Tiempo Real</h3>
    
    <div v-if="message" class="alert" :class="messageType">
      {{ message }}
    </div>

    <div class="process-visualization">
      <ProcessCard 
        v-for="process in processStates" 
        :key="process.id"
        :process="process"
      />
    </div>
  </div>
</template>

<script>
import ProcessCard from './ProcessCard.vue'

export default {
  name: 'SimulationArea',
  components: {
    ProcessCard
  },
  props: {
    simulationRunning: {
      type: Boolean,
      default: false
    },
    processStates: {
      type: Array,
      required: true
    },
    message: {
      type: String,
      default: ''
    },
    messageType: {
      type: String,
      default: 'alert-success'
    }
  }
}
</script>

<style scoped>
.simulation-area {
  background: #f8f9fa;
  padding: 20px;
  border-radius: 15px;
  box-shadow: 0 5px 15px rgba(0,0,0,0.05);
}

.simulation-area h3 {
  color: #333;
  margin-bottom: 15px;
  font-size: 1.3em;
}

.process-visualization {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
  gap: 12px;
  max-width: 100%;
}

/* Para pantallas más grandes, limitar a 5 columnas máximo */
@media (min-width: 1200px) {
  .process-visualization {
    grid-template-columns: repeat(5, 1fr);
  }
}

/* Para pantallas medianas, 3 columnas */
@media (min-width: 768px) and (max-width: 1199px) {
  .process-visualization {
    grid-template-columns: repeat(3, 1fr);
  }
}

/* Para pantallas pequeñas, 2 columnas */
@media (max-width: 767px) {
  .process-visualization {
    grid-template-columns: repeat(2, 1fr);
  }
}

.alert {
  padding: 12px;
  border-radius: 8px;
  margin-bottom: 15px;
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