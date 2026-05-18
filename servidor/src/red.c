#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
    #include <arpa/inet.h>
#endif

#include <stdio.h>
#include <string.h>
#include "red.h"

#ifdef _WIN32
    static SOCKET server_fd = INVALID_SOCKET;
    static SOCKET client_fd = INVALID_SOCKET;
#else
    static int server_fd = -1;
    static int client_fd = -1;
#endif

int red_iniciar(int puerto) {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("[RED] Error: WSAStartup fallo\n");
        return -1;
    }
#endif

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

#ifdef _WIN32
    if (server_fd == INVALID_SOCKET) {
        printf("[RED] Error creando socket\n");
        WSACleanup();
        return -1;
    }
#else
    if (server_fd < 0) {
        printf("[RED] Error creando socket\n");
        return -1;
    }
#endif

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt));

    struct sockaddr_in direccion;
    memset(&direccion, 0, sizeof(direccion));
    direccion.sin_family = AF_INET;
    direccion.sin_addr.s_addr = INADDR_ANY;
    direccion.sin_port = htons(puerto);

    if (bind(server_fd, (struct sockaddr *)&direccion, sizeof(direccion)) < 0) {
        printf("[RED] Error en bind (puerto %d ocupado?)\n", puerto);
#ifdef _WIN32
        closesocket(server_fd);
        WSACleanup();
#else
        close(server_fd);
#endif
        return -1;
    }

    if (listen(server_fd, 1) < 0) {
        printf("[RED] Error en listen\n");
#ifdef _WIN32
        closesocket(server_fd);
        WSACleanup();
#else
        close(server_fd);
#endif
        return -1;
    }

    printf("[RED] Servidor escuchando en puerto %d...\n", puerto);
    return 0;
}

int red_esperar_cliente(void) {
    struct sockaddr_in dir_cliente;
    int tam = sizeof(dir_cliente);

    printf("[RED] Esperando cliente...\n");

#ifdef _WIN32
    client_fd = accept(server_fd, (struct sockaddr *)&dir_cliente, &tam);
    if (client_fd == INVALID_SOCKET) {
        printf("[RED] Error en accept\n");
        return -1;
    }
#else
    client_fd = accept(server_fd, (struct sockaddr *)&dir_cliente, (socklen_t *)&tam);
    if (client_fd < 0) {
        printf("[RED] Error en accept\n");
        return -1;
    }
#endif

    printf("[RED] Cliente conectado!\n");
    return 0;
}

int red_enviar(const char *mensaje) {
    char buffer[4096];
    int len = snprintf(buffer, sizeof(buffer), "%s\n", mensaje);

    int enviado = send(client_fd, buffer, len, 0);
    if (enviado < 0) {
        printf("[RED] Error enviando\n");
        return -1;
    }
    return 0;
}

int red_recibir(char *buffer, int max) {
    int total = 0;
    char c;

    while (total < max - 1) {
        int recibido = recv(client_fd, &c, 1, 0);

        if (recibido <= 0) {
            return -1;
        }

        if (c == '\n') {
            break;
        }

        buffer[total] = c;
        total++;
    }

    buffer[total] = '\0';

    if (total > 0 && buffer[total - 1] == '\r') {
        buffer[total - 1] = '\0';
        total--;
    }

    return total;
}

void red_cerrar_cliente(void) {
#ifdef _WIN32
    if (client_fd != INVALID_SOCKET) {
        closesocket(client_fd);
        client_fd = INVALID_SOCKET;
    }
#else
    if (client_fd >= 0) {
        close(client_fd);
        client_fd = -1;
    }
#endif
    printf("[RED] Cliente desconectado\n");
}

void red_cerrar(void) {
    red_cerrar_cliente();

#ifdef _WIN32
    if (server_fd != INVALID_SOCKET) {
        closesocket(server_fd);
        server_fd = INVALID_SOCKET;
    }
    WSACleanup();
#else
    if (server_fd >= 0) {
        close(server_fd);
        server_fd = -1;
    }
#endif
    printf("[RED] Servidor cerrado\n");
}   