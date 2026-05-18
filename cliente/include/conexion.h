#ifndef CONEXION_H
#define CONEXION_H

#include <string>

using namespace std;

class Conexion {
private:
    long long sock;
    bool conectado_flag;

public:
    Conexion();
    ~Conexion();

    bool conectar(const string &ip, int puerto);
    bool enviar(const string &mensaje);
    string recibir();
    void desconectar();
    bool estaConectado() const;
};

#endif