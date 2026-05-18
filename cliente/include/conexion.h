#ifndef CONEXION_H
#define CONEXION_H

#include <string>

class Conexion {
private:
    long long sock;
    bool conectado_flag;

public:
    Conexion();
    ~Conexion();

    bool conectar(const std::string &ip, int puerto);
    bool enviar(const std::string &mensaje);
    std::string recibir();
    void desconectar();
    bool estaConectado() const;
};

#endif