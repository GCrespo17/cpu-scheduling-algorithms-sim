<template>
  <div class="connection-status" :class="wsConnected ? 'connected' : 'disconnected'">
    {{ wsConnected ? '🟢 Conectado' : '🔴 Desconectado' }}
  </div>
</template>

<script>
export default {
  name: 'ConnectionStatus',
  props: {
    wsConnected: {
      type: Boolean,
      required: true
    }
  }
}
</script>

<style scoped>
.connection-status {
  position: fixed;
  top: 20px;
  right: 20px;
  padding: 10px 20px;
  border-radius: 25px;
  font-weight: 600;
  font-size: 14px;
  z-index: 1000;
  transition: all 0.3s ease;
}

.connected {
  background: #28a745;
  color: white;
  box-shadow: 0 2px 10px rgba(40, 167, 69, 0.3);
}

.disconnected {
  background: #dc3545;
  color: white;
  box-shadow: 0 2px 10px rgba(220, 53, 69, 0.3);
  animation: blink 1s infinite;
}

@keyframes blink {
  0%, 50% { opacity: 1; }
  51%, 100% { opacity: 0.5; }
}
</style>