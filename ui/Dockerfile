#Utilizamos la version Long Term Support de Vue
FROM node:lts-alpine AS build-stage

# Establecer el directorio de trabajo
WORKDIR /app

# Copiar package.json y package-lock.json (si existe)
COPY package*.json ./

# Instalar dependencias
RUN npm install

# Copiar el resto de los archivos del proyecto
COPY . .

# Exponer el puerto del servidor de desarrollo de Vite
EXPOSE 5173

# Comando para iniciar el servidor de desarrollo
# El host 0.0.0.0 es necesario para que sea accesible desde fuera del contenedor
CMD ["npm", "run", "dev", "--", "--host", "0.0.0.0"] 


