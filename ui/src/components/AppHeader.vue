<template>
  <div class="header">
    <h1>{{ algorithmConfig.icon }} Simulador de Procesos {{ algorithmConfig.name }}</h1>
    <p>{{ algorithmConfig.description }}</p>
  </div>
</template>

<script>
export default {
  name: 'AppHeader',
  props: {
    algorithm: {
      type: String,
      default: 'FCFS'
    }
  },
  computed: {
    algorithmConfig() {
      const configs = {
        'FCFS': {
          name: 'FCFS',
          icon: '⏰',
          description: 'Algoritmo First Come First Served - El primero en llegar, el primero en ser atendido'
        },
        'SJF': {
          name: 'SJF',
          icon: '⚡',
          description: 'Algoritmo Shortest Job First - El trabajo más corto primero'
        },
        'RR': {
          name: 'Round Robin',
          icon: '🔄',
          description: 'Algoritmo Round Robin - Tiempo compartido con quantum fijo'
        },
        'PRIORITY': {
          name: 'Priority',
          icon: '🎯',
          description: 'Algoritmo de Planificación por Prioridad - Mayor prioridad primero'
        }
      };
      
      return configs[this.algorithm] || configs['FCFS'];
    }
  }
}
</script>

<style scoped>
.header {
  background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
  color: white;
  padding: 30px;
  text-align: center;
  position: relative;
  overflow: hidden;
}

.header::before {
  content: '';
  position: absolute;
  top: -50%;
  left: -50%;
  width: 200%;
  height: 200%;
  background: radial-gradient(circle, rgba(255,255,255,0.1) 0%, transparent 70%);
  animation: pulse 4s ease-in-out infinite;
}

@keyframes pulse {
  0%, 100% { transform: scale(1); opacity: 0.3; }
  50% { transform: scale(1.1); opacity: 0.1; }
}

.header h1 {
  font-size: 2.5em;
  margin-bottom: 10px;
  text-shadow: 2px 2px 4px rgba(0,0,0,0.3);
  position: relative;
  z-index: 1;
}

.header p {
  font-size: 1.2em;
  opacity: 0.9;
  position: relative;
  z-index: 1;
  max-width: 600px;
  margin: 0 auto;
}

@media (max-width: 768px) {
  .header h1 {
    font-size: 2em;
  }
  
  .header p {
    font-size: 1em;
  }
}
</style>