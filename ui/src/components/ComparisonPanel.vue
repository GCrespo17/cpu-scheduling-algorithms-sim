<!-- ComparisonPanel.vue -->
<template>
  <div class="comparison-panel" v-if="comparisonData">
    <div class="panel-header">
      <h2>🏆 Panel de Comparación de Algoritmos</h2>
      <button class="close-btn" @click="$emit('close')">✕</button>
    </div>

    <!-- Tabla Comparativa -->
    <div class="comparison-table">
      <h3>📊 Tabla Comparativa de Métricas</h3>
      <div class="table-container">
        <table>
          <thead>
            <tr>
              <th>Algoritmo</th>
              <th>Avg Turnaround</th>
              <th>Avg Espera</th>
              <th>Avg Respuesta</th>
              <th>CPU Utilización</th>
              <th>Tiempo Total</th>
              <th>Puntuación</th>
            </tr>
          </thead>
          <tbody>
            <tr 
              v-for="algorithm in comparisonData.algorithms" 
              :key="algorithm.algorithm"
              :class="{ 'winner-row': algorithm.algorithm === comparisonData.overallBest }"
            >
              <td class="algorithm-cell">
                <span class="algorithm-badge" :class="getAlgorithmClass(algorithm.algorithm)">
                  {{ algorithm.algorithm }}
                </span>
              </td>
              <td>{{ algorithm.avgTurnaroundTime.toFixed(2) }}s</td>
              <td>{{ algorithm.avgWaitingTime.toFixed(2) }}s</td>
              <td>{{ algorithm.avgResponseTime.toFixed(2) }}s</td>
              <td>
                <div class="utilization-bar">
                  <div 
                    class="utilization-fill" 
                    :style="{ width: algorithm.cpuUtilization + '%' }"
                  ></div>
                  <span class="utilization-text">{{ algorithm.cpuUtilization.toFixed(1) }}%</span>
                </div>
              </td>
              <td>{{ algorithm.totalTime }}s</td>
              <td>
                <div class="score-badge" :class="getScoreClass(calculateScore(algorithm))">
                  {{ calculateScore(algorithm).toFixed(0) }}
                </div>
              </td>
            </tr>
          </tbody>
        </table>
      </div>
    </div>

    <!-- Diagramas de Gantt -->
    <div class="gantt-section" v-if="comparisonData.ganttData">
      <h3>📈 Diagramas de Gantt por Algoritmo</h3>
      <div class="gantt-container">
        <div 
          v-for="chart in comparisonData.ganttData.charts" 
          :key="chart.algorithm"
          class="gantt-chart"
        >
          <h4>
            {{ chart.algorithm }}
            <span v-if="chart.algorithm === 'RR'" class="quantum-info">
              (Quantum: {{ getQuantumForAlgorithm(chart.algorithm) }}s)
            </span>
          </h4>
          <div class="gantt-timeline">
            <div class="timeline-header">
              <span v-for="i in getMaxTime(chart)" :key="i" class="time-marker">{{ i-1 }}</span>
            </div>
            <div class="gantt-bars">
              <!-- Mostrar procesos/segmentos -->
              <div 
                v-for="(item, index) in chart.processes" 
                :key="index"
                class="gantt-bar"
                :class="getProcessClass(item.id)"
                :style="{ 
                  left: (item.startTime * 40) + 'px',
                  width: ((item.endTime - item.startTime) * 40) + 'px'
                }"
                :title="`P${item.id}: ${item.startTime}s - ${item.endTime}s (${item.duration}s)`"
              >
                P{{ item.id }}
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>

    <!-- Recomendación Detallada -->
    <div class="recommendation-section">
      <h3>💡 Análisis y Recomendación</h3>
      <div class="recommendation-text">
        <pre>{{ comparisonData.recommendation }}</pre>
      </div>
    </div>

    <!-- Acciones -->
    <div class="actions">
      <button class="btn btn-primary" @click="exportComparison">
        📊 Exportar Comparación
      </button>
      <button class="btn btn-secondary" @click="$emit('close')">
        🔙 Cerrar Panel
      </button>
    </div>
  </div>
</template>

<script>
export default {
  name: 'ComparisonPanel',
  props: {
    comparisonData: {
      type: Object,
      default: null
    }
  },
  methods: {
    getAlgorithmClass(algorithm) {
      const classes = {
        'FCFS': 'fcfs',
        'SJF': 'sjf', 
        'RR': 'rr',
        'PRIORITY': 'priority'
      };
      return classes[algorithm] || 'default';
    },

    calculateScore(algorithm) {
      // Score simple: CPU utilization - tiempo de espera promedio
      return Math.max(0, algorithm.cpuUtilization - algorithm.avgWaitingTime);
    },

    getScoreClass(score) {
      if (score >= 80) return 'excellent';
      if (score >= 60) return 'good';
      if (score >= 40) return 'average';
      return 'poor';
    },

    getMaxTime(chart) {
      let maxTime = 0;
      if (chart.processes) {
        chart.processes.forEach(item => {
          maxTime = Math.max(maxTime, item.endTime);
        });
      }
      return maxTime + 1;
    },

    getProcessClass(processId) {
      // Asignar colores diferentes por proceso
      const colors = ['process-1', 'process-2', 'process-3', 'process-4', 'process-5'];
      return colors[(processId - 1) % colors.length];
    },

    getQuantumForAlgorithm(algorithm) {
      // Buscar el quantum del algoritmo RR en los datos
      const rrAlgorithm = this.comparisonData.algorithms.find(alg => alg.algorithm === algorithm);
      return rrAlgorithm ? rrAlgorithm.quantum : 2;
    },

    exportComparison() {
      const data = {
        timestamp: new Date().toISOString(),
        comparison: this.comparisonData
      };
      
      const blob = new Blob([JSON.stringify(data, null, 2)], { type: 'application/json' });
      const url = URL.createObjectURL(blob);
      const link = document.createElement('a');
      link.href = url;
      link.download = `comparison_${Date.now()}.json`;
      link.click();
      URL.revokeObjectURL(url);
    }
  }
}
</script>

<style scoped>
.comparison-panel {
  position: fixed;
  top: 50%;
  left: 50%;
  transform: translate(-50%, -50%);
  width: 95vw;
  max-width: 1400px;
  height: 90vh;
  background: white;
  border-radius: 20px;
  box-shadow: 0 20px 60px rgba(0,0,0,0.3);
  z-index: 1000;
  overflow-y: auto;
  padding: 30px;
}

.panel-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 30px;
  border-bottom: 3px solid #667eea;
  padding-bottom: 15px;
}

.panel-header h2 {
  color: #333;
  margin: 0;
}

.close-btn {
  background: #dc3545;
  color: white;
  border: none;
  width: 40px;
  height: 40px;
  border-radius: 50%;
  font-size: 20px;
  cursor: pointer;
  transition: all 0.3s ease;
}

.close-btn:hover {
  background: #c82333;
  transform: scale(1.1);
}

.stat-item {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 15px;
  background: #f8f9fa;
  border-radius: 10px;
  border-left: 4px solid #667eea;
}

.stat-label {
  font-weight: 600;
  color: #555;
}

.stat-value {
  font-weight: bold;
  font-size: 1.2em;
  color: #333;
}

.comparison-table {
  margin-bottom: 30px;
}

.comparison-table h3 {
  color: #333;
  margin-bottom: 20px;
}

.table-container {
  overflow-x: auto;
  border-radius: 10px;
  box-shadow: 0 5px 15px rgba(0,0,0,0.1);
}

table {
  width: 100%;
  border-collapse: collapse;
  background: white;
}

th {
  background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
  color: white;
  padding: 15px 12px;
  text-align: left;
  font-weight: 600;
  font-size: 0.9em;
}

td {
  padding: 12px;
  border-bottom: 1px solid #eee;
}

tr:hover {
  background: #f8f9fa;
}

.winner-row {
  background: linear-gradient(135deg, #d4edda 0%, #c3e6cb 100%);
  font-weight: bold;
}

.algorithm-badge {
  padding: 6px 12px;
  border-radius: 20px;
  color: white;
  font-weight: bold;
  font-size: 0.85em;
}

.algorithm-badge.fcfs { background: #007bff; }
.algorithm-badge.sjf { background: #28a745; }
.algorithm-badge.rr { background: #ffc107; color: #333; }
.algorithm-badge.priority { background: #dc3545; }

.utilization-bar {
  position: relative;
  background: #e9ecef;
  border-radius: 10px;
  height: 20px;
  min-width: 80px;
}

.utilization-fill {
  height: 100%;
  background: linear-gradient(90deg, #28a745, #20c997);
  border-radius: 10px;
  transition: width 0.5s ease;
}

.utilization-text {
  position: absolute;
  top: 50%;
  left: 50%;
  transform: translate(-50%, -50%);
  font-size: 0.75em;
  font-weight: bold;
  color: #333;
}

.score-badge {
  padding: 4px 8px;
  border-radius: 15px;
  color: white;
  font-weight: bold;
  text-align: center;
  min-width: 40px;
}

.score-badge.excellent { background: #28a745; }
.score-badge.good { background: #ffc107; color: #333; }
.score-badge.average { background: #fd7e14; }
.score-badge.poor { background: #dc3545; }

.gantt-section {
  margin-bottom: 30px;
}

.gantt-section h3 {
  color: #333;
  margin-bottom: 20px;
}

.gantt-container {
  display: grid;
  gap: 20px;
}

.gantt-chart {
  background: white;
  padding: 20px;
  border-radius: 10px;
  box-shadow: 0 3px 10px rgba(0,0,0,0.1);
}

.gantt-chart h4 {
  margin: 0 0 15px 0;
  color: #555;
}

.gantt-timeline {
  position: relative;
  min-height: 60px;
}

.timeline-header {
  display: flex;
  margin-bottom: 10px;
}

.time-marker {
  width: 40px;
  text-align: center;
  font-size: 0.8em;
  color: #666;
  border-right: 1px solid #eee;
}

.gantt-bars {
  position: relative;
  height: 40px;
  background: #f8f9fa;
  border-radius: 5px;
}

.gantt-bar {
  position: absolute;
  height: 30px;
  top: 5px;
  background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
  color: white;
  display: flex;
  align-items: center;
  justify-content: center;
  border-radius: 5px;
  font-weight: bold;
  font-size: 0.8em;
  min-width: 40px;
  border: 1px solid rgba(255,255,255,0.3);
}

/* Colores diferentes por proceso */
.gantt-bar.process-1 {
  background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
}

.gantt-bar.process-2 {
  background: linear-gradient(135deg, #f093fb 0%, #f5576c 100%);
}

.gantt-bar.process-3 {
  background: linear-gradient(135deg, #4facfe 0%, #00f2fe 100%);
}

.gantt-bar.process-4 {
  background: linear-gradient(135deg, #43e97b 0%, #38f9d7 100%);
}

.gantt-bar.process-5 {
  background: linear-gradient(135deg, #fa709a 0%, #fee140 100%);
}

.quantum-info {
  font-size: 0.8em;
  color: #666;
  font-weight: normal;
}

.recommendation-section {
  margin-bottom: 30px;
}

.recommendation-section h3 {
  color: #333;
  margin-bottom: 15px;
}

.recommendation-text {
  background: #f8f9fa;
  padding: 20px;
  border-radius: 10px;
  border-left: 4px solid #667eea;
}

.recommendation-text pre {
  font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', sans-serif;
  white-space: pre-wrap;
  margin: 0;
  line-height: 1.6;
  color: #333;
}

.actions {
  display: flex;
  gap: 15px;
  justify-content: center;
  padding-top: 20px;
  border-top: 2px solid #eee;
}

.btn {
  padding: 12px 24px;
  border: none;
  border-radius: 8px;
  font-weight: 600;
  cursor: pointer;
  transition: all 0.3s ease;
}

.btn-primary {
  background: #667eea;
  color: white;
}

.btn-primary:hover {
  background: #5a6fd8;
  transform: translateY(-2px);
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
  .comparison-panel {
    width: 100vw;
    height: 100vh;
    border-radius: 0;
    padding: 20px;
  }
  
  
  .gantt-chart {
    overflow-x: auto;
  }
}
</style>