<!-- MetricsTable.vue -->
<template>
  <div class="metrics-container" v-if="metrics">
    <h3>📊 Métricas de Rendimiento - {{ metrics.algorithm }}</h3>
    
    <!-- Métricas del Sistema -->
    <div class="system-metrics">
      <h4>📈 Métricas del Sistema</h4>
      <table class="metrics-table">
        <thead>
          <tr>
            <th>Métrica</th>
            <th>Valor</th>
            <th>Unidad</th>
          </tr>
        </thead>
        <tbody>
          <tr>
            <td>Tiempo Promedio de Turnaround</td>
            <td class="metric-value">{{ metrics.avgTurnaroundTime.toFixed(2) }}</td>
            <td>segundos</td>
          </tr>
          <tr>
            <td>Tiempo Promedio de Espera</td>
            <td class="metric-value">{{ metrics.avgWaitingTime.toFixed(2) }}</td>
            <td>segundos</td>
          </tr>
          <tr>
            <td>Tiempo Promedio de Respuesta</td>
            <td class="metric-value">{{ metrics.avgResponseTime.toFixed(2) }}</td>
            <td>segundos</td>
          </tr>
          <tr class="highlight-row">
            <td>Utilización de CPU</td>
            <td class="metric-value cpu-utilization" :class="getCpuClass()">
              {{ metrics.cpuUtilization.toFixed(1) }}%
            </td>
            <td>porcentaje</td>
          </tr>
          <tr>
            <td>Tiempo Total de Simulación</td>
            <td class="metric-value">{{ metrics.totalTime }}</td>
            <td>segundos</td>
          </tr>
          <tr>
            <td>Tiempo de CPU Activo</td>
            <td class="metric-value">{{ metrics.totalCpuTime }}</td>
            <td>segundos</td>
          </tr>
          <tr>
            <td>Tiempo de CPU Inactivo</td>
            <td class="metric-value">{{ metrics.idleTime }}</td>
            <td>segundos</td>
          </tr>
        </tbody>
      </table>
    </div>

    <!-- Métricas por Proceso -->
    <div class="process-metrics" v-if="metrics.processMetrics">
      <h4>🔧 Métricas por Proceso</h4>
      <table class="metrics-table">
        <thead>
          <tr>
            <th>Proceso</th>
            <th>Llegada</th>
            <th>Ráfaga</th>
            <th>Turnaround</th>
            <th>Espera</th>
            <th>Respuesta</th>
            <th>Finalización</th>
          </tr>
        </thead>
        <tbody>
          <tr v-for="(process, index) in metrics.processMetrics" :key="process.id">
            <td class="process-id">P{{ process.id }}</td>
            <td>{{ process.arrivalTime }}s</td>
            <td>{{ process.burstTime }}s</td>
            <td class="metric-value">{{ process.turnaroundTime }}s</td>
            <td class="metric-value">{{ process.waitingTime }}s</td>
            <td class="metric-value">{{ process.responseTime }}s</td>
            <td>{{ process.completionTime }}s</td>
          </tr>
        </tbody>
      </table>
    </div>

    <!-- Resumen Rápido -->
    <div class="quick-summary">
      <div class="summary-item">
        <span class="summary-label">🏆 Mejor Proceso:</span>
        <span class="summary-value">P{{ getBestProcess().id }} ({{ getBestProcess().efficiency }}% eficiencia)</span>
      </div>
      <div class="summary-item">
        <span class="summary-label">⚡ Proceso Más Rápido:</span>
        <span class="summary-value">P{{ getFastestProcess().id }} ({{ getFastestProcess().turnaroundTime }}s turnaround)</span>
      </div>
      <div class="summary-item">
        <span class="summary-label">📊 Rendimiento General:</span>
        <span class="summary-value" :class="getPerformanceClass()">{{ getPerformanceRating() }}</span>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  name: 'MetricsTable',
  props: {
    metrics: {
      type: Object,
      default: null
    }
  },
  methods: {
    getCpuClass() {
      const utilization = this.metrics.cpuUtilization;
      if (utilization >= 90) return 'excellent';
      if (utilization >= 75) return 'good';
      if (utilization >= 60) return 'average';
      return 'poor';
    },
    
    getEfficiency(process) {
      // Eficiencia = (Tiempo de Ráfaga / Tiempo de Turnaround) * 100
      if (process.turnaroundTime === 0) return 0;
      return Math.round((process.burstTime / process.turnaroundTime) * 100);
    },
    
    getBestProcess() {
      if (!this.metrics.processMetrics) return { id: 0, efficiency: 0 };
      
      let best = this.metrics.processMetrics[0];
      let bestEfficiency = this.getEfficiency(best);
      
      this.metrics.processMetrics.forEach(process => {
        const efficiency = this.getEfficiency(process);
        if (efficiency > bestEfficiency) {
          best = process;
          bestEfficiency = efficiency;
        }
      });
      
      return { id: best.id, efficiency: bestEfficiency };
    },
    
    getFastestProcess() {
      if (!this.metrics.processMetrics) return { id: 0, turnaroundTime: 0 };
      
      let fastest = this.metrics.processMetrics[0];
      
      this.metrics.processMetrics.forEach(process => {
        if (process.turnaroundTime < fastest.turnaroundTime) {
          fastest = process;
        }
      });
      
      return { id: fastest.id, turnaroundTime: fastest.turnaroundTime };
    },
    
    getPerformanceRating() {
      const cpu = this.metrics.cpuUtilization;
      const avgWaiting = this.metrics.avgWaitingTime;
      
      if (cpu >= 85 && avgWaiting <= 3) return 'Excelente';
      if (cpu >= 70 && avgWaiting <= 5) return 'Bueno';
      if (cpu >= 50 && avgWaiting <= 8) return 'Regular';
      return 'Mejorable';
    },
    
    getPerformanceClass() {
      const rating = this.getPerformanceRating();
      if (rating === 'Excelente') return 'excellent';
      if (rating === 'Bueno') return 'good';
      if (rating === 'Regular') return 'average';
      return 'poor';
    }
  }
}
</script>

<style scoped>
.metrics-container {
  background: #f8f9fa;
  padding: 25px;
  border-radius: 15px;
  margin-top: 20px;
  box-shadow: 0 5px 15px rgba(0,0,0,0.1);
}

.metrics-container h3 {
  color: #333;
  margin-bottom: 25px;
  text-align: center;
  font-size: 1.5em;
  border-bottom: 3px solid #667eea;
  padding-bottom: 10px;
}

.system-metrics, .process-metrics {
  margin-bottom: 30px;
}

.system-metrics h4, .process-metrics h4 {
  color: #555;
  margin-bottom: 15px;
  font-size: 1.2em;
  display: flex;
  align-items: center;
  gap: 8px;
}

.metrics-table {
  width: 100%;
  border-collapse: collapse;
  background: white;
  border-radius: 10px;
  overflow: hidden;
  box-shadow: 0 3px 10px rgba(0,0,0,0.1);
}

.metrics-table th {
  background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
  color: white;
  padding: 15px 12px;
  text-align: left;
  font-weight: 600;
  font-size: 0.9em;
  text-transform: uppercase;
  letter-spacing: 0.5px;
}

.metrics-table td {
  padding: 12px;
  border-bottom: 1px solid #eee;
  font-size: 0.95em;
}

.metrics-table tbody tr:hover {
  background: #f8f9fa;
  transition: background-color 0.3s ease;
}

.metrics-table tbody tr:last-child td {
  border-bottom: none;
}

.metric-value {
  font-weight: 600;
  color: #333;
  text-align: center;
}

.process-id {
  font-weight: bold;
  color: #667eea;
  text-align: center;
}

.cpu-utilization {
  font-size: 1.1em;
  padding: 5px 10px;
  border-radius: 20px;
  color: white !important;
}

.cpu-utilization.excellent { background: #28a745; }
.cpu-utilization.good { background: #ffc107; color: #333 !important; }
.cpu-utilization.average { background: #fd7e14; }
.cpu-utilization.poor { background: #dc3545; }

.highlight-row {
  background: #f1f3f4;
}

.efficiency {
  width: 120px;
}

.efficiency-bar {
  position: relative;
  background: #e9ecef;
  border-radius: 10px;
  height: 20px;
  overflow: hidden;
  min-width: 80px;
}

.efficiency-fill {
  height: 100%;
  background: linear-gradient(90deg, #28a745, #20c997);
  transition: width 0.5s ease;
  border-radius: 10px;
}

.efficiency-text {
  position: absolute;
  top: 50%;
  left: 50%;
  transform: translate(-50%, -50%);
  font-size: 0.75em;
  font-weight: bold;
  color: #333;
  text-shadow: 1px 1px 2px rgba(255,255,255,0.8);
}

.quick-summary {
  background: white;
  padding: 20px;
  border-radius: 10px;
  box-shadow: 0 2px 8px rgba(0,0,0,0.05);
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
  gap: 15px;
}

.summary-item {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 10px;
  border-left: 4px solid #667eea;
  background: #f8f9fa;
  border-radius: 5px;
}

.summary-label {
  font-weight: 600;
  color: #555;
}

.summary-value {
  font-weight: bold;
  color: #333;
}

.summary-value.excellent { color: #28a745; }
.summary-value.good { color: #ffc107; }
.summary-value.average { color: #fd7e14; }
.summary-value.poor { color: #dc3545; }

/* Responsive */
@media (max-width: 768px) {
  .metrics-table {
    font-size: 0.8em;
  }
  
  .metrics-table th, .metrics-table td {
    padding: 8px 6px;
  }
  
  .quick-summary {
    grid-template-columns: 1fr;
  }
  
  .summary-item {
    flex-direction: column;
    text-align: center;
    gap: 5px;
  }
}
</style>