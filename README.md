# Proyecto Final: Gestión de Cine

Sistema de gestión para cines compuesto por tres aplicaciones interconectadas: un servidor central, un cliente remoto y un panel de administración local.
El proyecto está escrito en C (Servidor y Admin) y C++ (Cliente), y utiliza Sockets para la comunicación de red y SQLite para la persistencia.

## Requisitos previos
- Entorno Windows con MinGW instalado (y añadido al PATH del sistema). Los comandos `gcc` y `g++` deben estar disponibles en consola.
- PowerShell o Bash para ejecutar los scripts proporcionados.

## Estructura del Proyecto
- **Servidor Remoto (C)**: Se encarga de gestionar las conexiones de red por el puerto 5000 y se comunica con la base de datos `cine.db`.
- **Cliente Remoto (C++)**: Aplicación que simula un cajero remoto. Se conecta al Servidor vía TCP/IP para consultar e insertar ventas.
- **Administrador Local (C)**: Programa local que no utiliza sockets y lee directamente la BBDD para tareas administrativas (creación de películas, reportes locales).

## Instrucciones de Compilación

Para facilitar la tarea de compilación se incluyen scripts `.sh` en cada módulo y un `Makefile` principal en la raíz.

### Utilizando los scripts proporcionados (Recomendado en Windows con Bash)
1. **Administrador Local**: En la raíz del proyecto ejecuta `bash compilar.sh`. El binario quedará en `bin/programa.exe`.
2. **Servidor Remoto**: Entra en la carpeta `servidor` (`cd servidor`) y ejecuta `bash compilar.sh`. El binario quedará en `servidor/bin/servidor.exe`.
3. **Cliente Remoto**: Entra en la carpeta `cliente` (`cd cliente`) y ejecuta `bash compilar.sh`. El binario quedará en `cliente/bin/cliente.exe`.

*Nota: También puedes usar el Makefile en la raíz (`mingw32-make` o `make`) para el administrador local.*

## Instrucciones de Ejecución

Para el correcto funcionamiento del sistema Cliente-Servidor necesitas dos consolas:

1. **Terminal 1 - Iniciar Servidor:**
   ```bash
   cd GestionCine
   ./servidor/bin/servidor.exe
   ```
2. **Terminal 2 - Iniciar Cliente:**
   ```bash
   cd GestionCine/cliente
   ./bin/cliente.exe
   ```

3. **Login por defecto:**
   - **Usuario**: `admin`
   - **Contraseña**: `admin123`

Para usar el **Administrador Local**, simplemente ejecuta desde la raíz:
```bash
./bin/programa.exe
```

## Credenciales de Base de Datos y Configuración
El archivo `config/servidor.conf` permite configurar la ruta de la base de datos y los datos iniciales del usuario administrador. La base de datos es inicializada automáticamente en su primera ejecución si no existe.