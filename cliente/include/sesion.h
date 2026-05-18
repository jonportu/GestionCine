#ifndef SESION_H
#define SESION_H

#include "entidad.h"

using namespace std;

class Sesion : public Entidad {
private:
    int peliculaId;
    int salaId;
    string fecha;
    string hora;
    float precio;
    int vendidas;

public:
    Sesion();

    int getPeliculaId() const;
    int getSalaId() const;
    string getFecha() const;
    string getHora() const;
    float getPrecio() const;
    int getVendidas() const;

    void setPeliculaId(int id);
    void setSalaId(int id);
    void setFecha(const string &f);
    void setHora(const string &h);
    void setPrecio(float p);

    string toString() const override;
    void fromRespuesta(const string &linea) override;
    string toComando() const;
    string toComandoConId() const;
};

#endif