#ifndef ENTRADA_H
#define ENTRADA_H

#include "entidad.h"

using namespace std;

class Entrada : public Entidad {
private:
    int sesionId;
    int clienteId;
    float precio;
    string fecha;

public:
    Entrada();

    int getSesionId() const;
    int getClienteId() const;
    float getPrecio() const;
    string getFecha() const;

    void setSesionId(int id);
    void setClienteId(int id);

    string toString() const override;
    void fromRespuesta(const string &linea) override;
};

#endif