# Guía de Uso: Gestión de Cine

Esta guía documenta el uso de la aplicación de Gestión de Cines, explicando los módulos disponibles y cómo interactuar con ellos tanto desde el Administrador Local como desde el Cliente Remoto.

## 1. Inicio de Sesión
Al arrancar cualquiera de las tres aplicaciones (Servidor, Cliente o Administrador Local) se solicitará inicio de sesión.
El usuario administrador por defecto se encuentra definido en `config/servidor.conf`.
- **Usuario:** `admin`
- **Contraseña:** `admin123`

## 2. Panel Principal
Una vez iniciada sesión, dispondrá de un menú principal numérico con varias opciones:
1. **Peliculas**
2. **Salas**
3. **Sesiones**
4. **Clientes**
5. **Venta de entradas**
6. **Reportes**

Para seleccionar una opción, simplemente introduzca el número correspondiente y pulse `ENTER`. 
*Nota: Si se introducen letras en un menú numérico, el sistema advertirá de que la entrada es inválida, evitando fallos en la aplicación.*

## 3. Gestión de Datos (CRUD)
En las secciones de Películas, Salas, Sesiones y Clientes, el comportamiento es similar:
- **Listar**: Muestra todos los registros guardados en la base de datos (SQLite) en forma de tabla.
- **Añadir**: Solicitará los diferentes campos (título, precio, sala, etc.). Es importante usar formatos de fecha correctos (ej: YYYY-MM-DD).
- **Modificar**: Solicita el ID del registro a editar, lo busca en base de datos y pide los nuevos datos.
- **Borrar**: Pide el ID del registro y requiere una confirmación (`s/n`) antes de eliminarlo de forma permanente de la BD.

## 4. Venta de Entradas
Simula la venta de tickets para una sesión concreta.
1. Al acceder, muestra las sesiones actuales disponibles.
2. Pedirá el **ID de la Sesión** a la que desea asistir.
3. Preguntará si el cliente está registrado en el sistema. En caso afirmativo, le pedirá su ID de cliente.
4. Una vez procesado, se incrementará la ocupación de la sala y se sumará al recuento de ingresos automáticamente en la base de datos. Si la sala excede la capacidad, mostrará un error y abortará la compra.

## 5. Reportes
La opción de reportes permite consultar rápidamente estadísticas del negocio:
- **Peliculas en cartelera**: Muestra todas las sesiones activas en este momento agrupadas por película.
- **Ingresos por pelicula**: Calcula todo el dinero recolectado de la venta de entradas de una película en concreto.
- **Ingresos por sala**: Útil para saber el rendimiento económico por proyector.
- **Ocupación de salas**: Lista el total de asientos ocupados vs libres.

## 6. Sockets y Cliente Remoto
Cuando utilice el programa desde el **Cliente Remoto (C++)**, este enviará todas las peticiones (login, crear película, modificar sala, vender entrada) a través de la red al **Servidor Remoto (C)**, que actuará de intermediario con la base de datos.
El servidor debe estar en ejecución antes de abrir el cliente, y registrará cada acción del usuario en el archivo `data/servidor.log`.
