<!-- <script setup>
import HelloWorld from './components/HelloWorld.vue'
</script>

<template>
  <div>
    <a href="https://vite.dev" target="_blank">
      <img src="/vite.svg" class="logo" alt="Vite logo" />
    </a>
    <a href="https://vuejs.org/" target="_blank">
      <img src="./assets/vue.svg" class="logo vue" alt="Vue logo" />
    </a>
  </div>
  <HelloWorld msg="Vite + Vue" />
</template>

<style scoped>
.logo {
  height: 6em;
  padding: 1.5em;
  will-change: filter;
  transition: filter 300ms;
}
.logo:hover {
  filter: drop-shadow(0 0 2em #646cffaa);
}
.logo.vue:hover {
  filter: drop-shadow(0 0 2em #42b883aa);
}
</style> -->

<script setup>
import { ref, onMounted } from 'vue';
import axios from 'axios'; // Importa Axios

const messageFromBackend = ref('Cargando mensaje del backend...');

// La URL base de tu backend
// process.env.VUE_APP_BACKEND_URL será 'http://core:8080' cuando corra en Docker Compose
// En desarrollo local (npm run dev sin Docker Compose), puedes tener un fallback o usar el proxy de vite.config.js
const BACKEND_URL = import.meta.env.VITE_APP_BACKEND_URL || 'http://localhost:8080';

// Función para obtener el mensaje del backend
const fetchMessage = async () => {
  try {
    const response = await axios.get(`${BACKEND_URL}/hello`);
    messageFromBackend.value = response.data.message;
  } catch (error) {
    console.error('Error al conectar con el backend:', error);
    messageFromBackend.value = 'Error al cargar el mensaje: ' + error.message;
  }
};

// Llama a la función cuando el componente se monta
onMounted(() => {
  fetchMessage();
});
</script>

<template>
  <div>
    <h1>Simulador de Planificación de CPU</h1>
    <p>Mensaje del Backend: <strong>{{ messageFromBackend }}</strong></p>

    <img alt="Vue logo" class="logo" src="./assets/vue.svg" width="125" height="125" />
    <HelloWorld msg="Bienvenido a tu Simulador" />
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
</style>
