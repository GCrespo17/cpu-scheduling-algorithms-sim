<script setup>
import { ref, onMounted, onBeforeUnmount } from 'vue';
import axios from 'axios'; // Importa Axios
import HelloWorld from './components/HelloWorld.vue'; // Asegúrate de que esta ruta sea correcta

const messageFromBackend = ref('Cargando mensaje del backend...');
const wsMessages = ref([]); // Para almacenar los mensajes recibidos por WebSocket
const wsInput = ref(''); // Para el input del mensaje a enviar por WebSocket
let ws = null; // Variable para la instancia de WebSocket

// La URL base de tu backend
const BACKEND_URL = import.meta.env.VITE_APP_BACKEND_URL || 'http://localhost:8081';
const WEBSOCKET_URL = `ws://localhost:8081/ws`; // URL de tu servidor WebSocket

// Función para obtener el mensaje del backend (HTTP)
const fetchMessage = async () => {
  try {
    const response = await axios.get(`${BACKEND_URL}/hello`);
    messageFromBackend.value = response.data.message;
  } catch (error) {
    console.error('Error al conectar con el backend HTTP:', error);
    messageFromBackend.value = 'Error al cargar el mensaje HTTP: ' + error.message;
  }
};

// Función para inicializar la conexión WebSocket
const initWebSocket = () => {
  ws = new WebSocket(WEBSOCKET_URL);

  ws.onopen = () => {
    wsMessages.value.push({ type: 'status', text: 'Conectado al servidor WebSocket.' });
    console.log('Conectado al servidor WebSocket');
  };

  ws.onmessage = (event) => {
    wsMessages.value.push({ type: 'received', text: event.data });
    console.log('Mensaje del servidor (WS):', event.data);
  };

  ws.onclose = (event) => {
    wsMessages.value.push({ type: 'status', text: `Desconectado del servidor WebSocket. Código: ${event.code}, Razón: ${event.reason}` });
    console.log('Desconectado del servidor WebSocket', event);
  };

  ws.onerror = (error) => {
    wsMessages.value.push({ type: 'error', text: `Error en WebSocket: ${error.message}` });
    console.error('Error en WebSocket:', error);
  };
};

// Función para enviar un mensaje a través del WebSocket
const sendMessage = () => {
  if (ws && ws.readyState === WebSocket.OPEN && wsInput.value.trim() !== '') {
    ws.send(wsInput.value);
    wsMessages.value.push({ type: 'sent', text: wsInput.value });
    wsInput.value = ''; // Limpiar el input
  } else if (ws && ws.readyState !== WebSocket.OPEN) {
    wsMessages.value.push({ type: 'error', text: 'WebSocket no está abierto. Intentando reconectar...' });
    console.warn('WebSocket no está abierto. Intentando reconectar...');
    initWebSocket(); // Intentar reconectar
  } else if (wsInput.value.trim() === '') {
    wsMessages.value.push({ type: 'info', text: 'No puedes enviar un mensaje vacío.' });
  }
};

// Llama a las funciones cuando el componente se monta
onMounted(() => {
  fetchMessage();
  initWebSocket();
});

// Cierra la conexión WebSocket cuando el componente se desmonta
onBeforeUnmount(() => {
  if (ws) {
    ws.close();
  }
});
</script>

<template>
  <div>
    <h1>Simulador de Planificación de CPU</h1>
    <p>Mensaje del Backend (HTTP): <strong>{{ messageFromBackend }}</strong></p>

    <img alt="Vue logo" class="logo" src="./assets/vue.svg" width="125" height="125" />
    <HelloWorld msg="Bienvenido a tu Simulador" />

    <hr style="margin: 20px 0;">

    <h2>Comunicación WebSocket</h2>
    <div id="ws-messages" style="border: 1px solid #ccc; padding: 10px; min-height: 150px; max-height: 300px; overflow-y: auto; margin-bottom: 10px; background-color: #f9f9f9; border-radius: 8px;">
      <p v-for="(msg, index) in wsMessages" :key="index" :style="{ color: msg.type === 'status' ? 'green' : (msg.type === 'error' ? 'red' : (msg.type === 'sent' ? 'blue' : 'black')) }">
        <strong v-if="msg.type === 'received'">Servidor (WS):</strong>
        <strong v-else-if="msg.type === 'sent'">Tú (WS):</strong>
        <strong v-else-if="msg.type === 'status'">Estado:</strong>
        <strong v-else-if="msg.type === 'error'">Error:</strong>
        {{ msg.text }}
      </p>
    </div>
    <input type="text" v-model="wsInput" @keyup.enter="sendMessage" placeholder="Escribe un mensaje WebSocket..." style="width: 70%; padding: 8px; border-radius: 5px; border: 1px solid #ddd;">
    <button @click="sendMessage" style="padding: 8px 15px; margin-left: 10px; background-color: #007bff; color: white; border: none; border-radius: 5px; cursor: pointer;">Enviar WS</button>
  </div>
</template>

<style scoped>
/* Tus estilos CSS aquí */
.logo {
  display: block;
  margin: 0 auto 2rem;
}

@media (min-width: 1024px) {
  .logo {
    margin: 0 2rem 0 0;
  }
}

/* Estilos adicionales para la sección WebSocket */
#ws-messages p {
  margin: 5px 0;
  word-wrap: break-word; /* Para asegurar que los mensajes largos se ajusten */
}
</style>
