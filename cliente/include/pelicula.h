#ifndef PELICULA_H
#define PELICULA_H

#include "entidad.h"

using namespace std;

class Pelicula : public Entidad {
private:
    string titulo;
    string genero;
    int duracion;
    int edad;

public:
    Pelicula();

    string getTitulo() const;
    string getGenero() const;
    int getDuracion() const;
    int getEdad() const;

    void setTitulo(const string &t);
    void setGenero(const string &g);
    void setDuracion(int d);
    void setEdad(int e);

    string toString() const override;
    void fromRespuesta(const string &linea) override;
    string toComando() const;
    string toComandoConId() const;
};

#endif