<!-- src/components/ProcessDisplay.vue -->
<template>
  <div :class="['process-card', processStateClass, 'bg-white p-6 rounded-xl shadow-md border']">
    <h3 class="text-2xl font-bold mb-3 text-gray-800">ID del Proceso: {{ process.id }}</h3>
    <p class="text-lg font-semibold mb-4">
      <strong class="text-gray-700">Estado:</strong>
      <span :class="['state-text', processStateClass]">{{ processStateText }}</span>
    </p>
    <div class="process-details text-gray-600 text-base space-y-2">
      <p><strong>Tiempo de Llegada:</strong> {{ process.arrivalTime }}</p>
      <p><strong>Tiempo de Ráfaga:</strong> {{ process.burstTime }}</p>
      <p><strong>Prioridad:</strong> {{ process.priority }}</p>
      <p v-if="process.startTime !== -1"><strong>Tiempo de Inicio:</strong> {{ process.startTime }}</p>
      <p v-if="process.completionTime !== 0"><strong>Tiempo de Finalización:</strong> {{ process.completionTime }}</p>
      <p v-if="process.turnaroundTime !== 0"><strong>Tiempo de Retorno:</strong> {{ process.turnaroundTime }}</p>
      <p v-if="process.waitingTime !== 0"><strong>Tiempo de Espera:</strong> {{ process.waitingTime }}</p>
      <p v-if="process.responseTime !== 0"><strong>Tiempo de Respuesta:</strong> {{ process.responseTime }}</p>
    </div>
  </div>
</template>

<script>
export default {
  props: {
    process: {
      type: Object,
      required: true,
    },
  },
  computed: {
    /**
     * Mapea el estado numérico del backend a una cadena de texto legible.
     * @returns {string} El nombre del estado del proceso.
     */
    processStateText() {
      switch (this.process.currentState) {
        case 0: return 'NUEVO'; // NEW
        case 1: return 'LISTO'; // READY
        case 2: return 'EJECUTANDO'; // RUNNING
        case 3: return 'TERMINADO'; // TERMINATED
        default: return 'DESCONOCIDO';
      }
    },
    /**
     * Aplica una clase CSS basada en el estado del proceso para el estilo visual.
     * @returns {string} La clase CSS correspondiente al estado.
     */
    processStateClass() {
      switch (this.process.currentState) {
        case 0: return 'state-new';
        case 1: return 'state-ready';
        case 2: return 'state-running';
        case 3: return 'state-terminated';
        default: return '';
      }
    },
  },
};
</script>

<style scoped>
/* Estilos base para la tarjeta de proceso */
.process-card {
  border-radius: 0.75rem; /* rounded-xl */
  padding: 1.5rem; /* p-6 */
  margin: 1rem; /* m-4 */
  width: 18rem; /* w-72 */
  box-shadow: 0 4px 6px -1px rgba(0, 0, 0, 0.1), 0 2px 4px -1px rgba(0, 0, 0, 0.06); /* shadow-lg */
  text-align: left;
  transition: all 0.5s ease; /* transition-all duration-500 ease-in-out */
  transform: scale(1); /* initial transform for hover effect */
  border: 1px solid #e5e7eb; /* border */
}

.process-card:hover {
  transform: scale(1.05); /* hover:scale-105 */
}

.process-card h3 {
  color: #1f2937; /* text-gray-800 */
}

.process-card p {
  color: #4b5563; /* text-gray-600 */
}

.process-details {
  margin-top: 1rem; /* mt-4 */
  padding-top: 1rem; /* pt-4 */
  border-top: 1px solid #e5e7eb; /* border-t border-gray-200 */
}

/* Estilos específicos para el texto del estado */
.state-text {
  font-weight: 700; /* font-bold */
  padding-left: 0.75rem; /* px-3 */
  padding-right: 0.75rem; /* px-3 */
  padding-top: 0.25rem; /* py-1 */
  padding-bottom: 0.25rem; /* py-1 */
  border-radius: 9999px; /* rounded-full */
  color: #ffffff; /* text-white */
  font-size: 0.875rem; /* text-sm */
}

/* Estilos de fondo y borde basados en el estado */
.state-new {
  background-color: #dbeafe; /* bg-blue-100 */
  border-color: #60a5fa; /* border-blue-400 */
}
.state-new .state-text {
  background-color: #3b82f6; /* bg-blue-500 */
}

.state-ready {
  background-color: #fffbeb; /* bg-yellow-100 */
  border-color: #fcd34d; /* border-yellow-400 */
}
.state-ready .state-text {
  background-color: #f59e0b; /* bg-yellow-500 */
}

.state-running {
  background-color: #dcfce7; /* bg-green-100 */
  border-color: #4ade80; /* border-green-400 */
  box-shadow: 0 8px 16px rgba(76, 175, 80, 0.2); /* Sombra más prominente para el estado de ejecución */
}
.state-running .state-text {
  background-color: #22c55e; /* bg-green-500 */
}

.state-terminated {
  background-color: #fee2e2; /* bg-red-100 */
  border-color: #f87171; /* border-red-400 */
  opacity: 0.8; /* opacity-80 */
}
.state-terminated .state-text {
  background-color: #ef4444; /* bg-red-500 */
}
</style>
