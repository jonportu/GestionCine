#include <iostream>
#include <string>
#include "conexion.h"

int main() {
    Conexion con;

    if (!con.conectar("127.0.0.1", 5000)) {
        std::cout << "No se pudo conectar al servidor" << std::endl;
        return 1;
    }

    std::cout << "Conectado! Escribe comandos (SALIR para terminar)" << std::endl;
    std::cout << std::endl;

    while (con.estaConectado()) {
        std::cout << "> ";
        std::string mensaje;
        std::getline(std::cin, mensaje);

        if (mensaje.empty()) continue;

        con.enviar(mensaje);

        if (mensaje == "SALIR") {
            std::string respuesta = con.recibir();
            std::cout << "Servidor: " << respuesta << std::endl;
            break;
        }

        std::string respuesta = con.recibir();
        std::cout << "Servidor: " << respuesta << std::endl;
        std::cout << std::endl;
    }

    con.desconectar();
    std::cout << "Desconectado." << std::endl;

    return 0;
}