<template>
    <div class="processes-list">
        <div class="list-header">
            <h3>📋 Procesos en Cola ({{ processes.length }})</h3>
            <div class="algorithm-info">
                <span class="algorithm-badge">{{ algorithmConfig.name }}</span>
                <span v-if="algorithm === 'RR'" class="quantum-info">Quantum: {{ quantum }}s</span>
            </div>
        </div>
        
        <div class="process-grid">
            <div
                class="process-item"
                v-for="process in processes"
                :key="process.id"
            >
                <div class="process-main">
                    <strong>Proceso {{ process.id }}</strong>
                    <span class="process-details">
                        Llegada: {{ process.arrivalTime }}s | 
                        Ráfaga: {{ process.burstTime }}s
                        <span v-if="algorithm === 'PRIORITY'"> | Prioridad: {{ process.priority }}</span>
                    </span>
                </div>
            </div>
        </div>
        
        <div class="button-group">
            <button class="btn btn-success" @click="startSimulation">
                🚀 Iniciar Simulación {{ algorithmConfig.name }}
            </button>
        </div>
    </div>
</template>

<script>
export default {
    name: "ProcessList",
    props: {
        processes: {
            type: Array,
            required: true,
        },
        algorithm: {
            type: String,
            default: 'FCFS'
        },
        quantum: {
            type: Number,
            default: 2
        }
    },
    computed: {
        algorithmConfig() {
            const configs = {
                'FCFS': { name: 'FCFS', icon: '⏰', color: '#28a745' },
                'SJF': { name: 'SJF', icon: '⚡', color: '#ffc107' },
                'RR': { name: 'Round Robin', icon: '🔄', color: '#007bff' },
                'PRIORITY': { name: 'Priority', icon: '🎯', color: '#dc3545' }
            };
            return configs[this.algorithm] || configs['FCFS'];
        }
    },
    methods: {
        startSimulation() {
            this.$emit("start-simulation");
        },
    },
};
</script>

<style scoped>
.processes-list {
    background: #f8f9fa;
    padding: 20px;
    border-radius: 15px;
    margin-bottom: 30px;
    border: 2px solid #e9ecef;
}

.list-header {
    display: flex;
    justify-content: space-between;
    align-items: center;
    margin-bottom: 20px;
    flex-wrap: wrap;
    gap: 10px;
}

.list-header h3 {
    color: #333;
    margin: 0;
}

.algorithm-info {
    display: flex;
    align-items: center;
    gap: 10px;
}

.algorithm-badge {
    background: v-bind('algorithmConfig.color');
    color: white;
    padding: 5px 12px;
    border-radius: 20px;
    font-size: 0.8em;
    font-weight: 600;
    text-transform: uppercase;
    letter-spacing: 0.5px;
}

.quantum-info {
    background: #e9ecef;
    color: #495057;
    padding: 4px 8px;
    border-radius: 12px;
    font-size: 0.75em;
    font-weight: 500;
}

.process-grid {
    display: grid;
    gap: 10px;
    margin-bottom: 20px;
}

.process-item {
    background: white;
    padding: 15px;
    border-radius: 10px;
    border-left: 4px solid v-bind('algorithmConfig.color');
    box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);
    transition: transform 0.2s ease;
}

.process-item:hover {
    transform: translateY(-2px);
    box-shadow: 0 4px 12px rgba(0, 0, 0, 0.15);
}

.process-main {
    display: flex;
    flex-direction: column;
    gap: 5px;
}

.process-main strong {
    color: v-bind('algorithmConfig.color');
    font-size: 1.1em;
}

.process-details {
    color: #666;
    font-size: 0.9em;
    font-weight: 400;
}

.button-group {
    display: flex;
    gap: 15px;
    justify-content: center;
    margin-top: 20px;
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

.btn-success {
    background: v-bind('algorithmConfig.color');
    color: white;
}

.btn-success:hover {
    filter: brightness(0.9);
    transform: translateY(-2px);
    box-shadow: 0 5px 15px rgba(0, 0, 0, 0.2);
}

@media (max-width: 768px) {
    .list-header {
        flex-direction: column;
        align-items: flex-start;
    }
    
    .algorithm-info {
        align-self: stretch;
        justify-content: center;
    }
}
</style>