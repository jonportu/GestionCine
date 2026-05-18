#ifndef SALA_H
#define SALA_H

#include "entidad.h"

using namespace std;

class Sala : public Entidad {
private:
    int numero;
    int capacidad;

public:
    Sala();

    int getNumero() const;
    int getCapacidad() const;

    void setNumero(int n);
    void setCapacidad(int c);

    string toString() const override;
    void fromRespuesta(const string &linea) override;
    string toComando() const;
    string toComandoConId() const;
};

#endif