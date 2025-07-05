<template>
    <div class="processes-list">
        <h3>📋 Procesos en Cola ({{ processes.length }})</h3>
        <div
            class="process-item"
            v-for="process in processes"
            :key="process.id"
        >
            <strong>Proceso {{ process.id }}</strong> - Llegada:
            {{ process.arrivalTime }}s, Ráfaga: {{ process.burstTime }}s,
            Prioridad: {{ process.priority }}
        </div>
        <div class="button-group">
            <button class="btn btn-success" @click="startSimulation">
                🚀 Iniciar Simulación
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
}

.processes-list h3 {
    color: #333;
    margin-bottom: 15px;
}

.process-item {
    background: white;
    padding: 15px;
    border-radius: 10px;
    margin-bottom: 10px;
    border-left: 4px solid #667eea;
    box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);
    color: #333; /* Texto oscuro para buena legibilidad */
    font-weight: 500;
}

.process-item:last-child {
    margin-bottom: 0;
}

.process-item strong {
    color: #667eea; /* Color destacado para el nombre del proceso */
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
    background: #28a745;
    color: white;
}

.btn-success:hover {
    background: #218838;
    transform: translateY(-2px);
}
</style>