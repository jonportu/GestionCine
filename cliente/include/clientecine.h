#ifndef CLIENTECINE_H
#define CLIENTECINE_H

#include "entidad.h"

using namespace std;

class ClienteCine : public Entidad {
private:
    string nombre;
    string email;
    string telefono;

public:
    ClienteCine();

    string getNombre() const;
    string getEmail() const;
    string getTelefono() const;

    void setNombre(const string &n);
    void setEmail(const string &e);
    void setTelefono(const string &t);

    string toString() const override;
    void fromRespuesta(const string &linea) override;
    string toComando() const;
    string toComandoConId() const;
};

#endif