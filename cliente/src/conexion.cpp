#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
#endif

#include <cstdio>
#include <cstring>
#include "conexion.h"

Conexion::Conexion() {
    sock = -1;
    conectado_flag = false;

#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
}

Conexion::~Conexion() {
    desconectar();

#ifdef _WIN32
    WSACleanup();
#endif
}

bool Conexion::conectar(const std::string &ip, int puerto) {
    sock = socket(AF_INET, SOCK_STREAM, 0);

#ifdef _WIN32
    if (sock == INVALID_SOCKET) {
        printf("[CONEXION] Error creando socket\n");
        return false;
    }
#else
    if (sock < 0) {
        printf("[CONEXION] Error creando socket\n");
        return false;
    }
#endif

    struct sockaddr_in dir_servidor;
    memset(&dir_servidor, 0, sizeof(dir_servidor));
    dir_servidor.sin_family = AF_INET;
    dir_servidor.sin_port = htons(puerto);

    if (inet_pton(AF_INET, ip.c_str(), &dir_servidor.sin_addr) <= 0) {
        printf("[CONEXION] Direccion IP invalida: %s\n", ip.c_str());
        return false;
    }

    if (connect((int)sock, (struct sockaddr *)&dir_servidor, sizeof(dir_servidor)) < 0) {
        printf("[CONEXION] No se pudo conectar a %s:%d\n", ip.c_str(), puerto);
        return false;
    }

    conectado_flag = true;
    printf("[CONEXION] Conectado a %s:%d\n", ip.c_str(), puerto);
    return true;
}

bool Conexion::enviar(const std::string &mensaje) {
    if (!conectado_flag) return false;

    std::string linea = mensaje + "\n";

    int enviado = send((int)sock, linea.c_str(), linea.length(), 0);
    if (enviado < 0) {
        printf("[CONEXION] Error enviando\n");
        return false;
    }
    return true;
}

std::string Conexion::recibir() {
    std::string resultado;
    char c;

    while (true) {
        int recibido = recv((int)sock, &c, 1, 0);

        if (recibido <= 0) {
            conectado_flag = false;
            return "";
        }

        if (c == '\n') {
            break;
        }

        if (c != '\r') {
            resultado += c;
        }
    }

    return resultado;
}

void Conexion::desconectar() {
    if (!conectado_flag) return;

#ifdef _WIN32
    closesocket((SOCKET)sock);
#else
    close((int)sock);
#endif

    sock = -1;
    conectado_flag = false;
}

bool Conexion::estaConectado() const {
    return conectado_flag;
}