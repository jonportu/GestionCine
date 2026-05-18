#ifndef CACHE_H
#define CACHE_H

#include <vector>
#include "conexion.h"
#include "pelicula.h"
#include "sala.h"
#include "sesion.h"
#include "clientecine.h"
#include "entrada.h"

using namespace std;

class Cache {
private:
    Conexion &con;

    vector<Pelicula> peliculas;
    vector<Sala> salas;
    vector<Sesion> sesiones;
    vector<ClienteCine> clientes;
    vector<Entrada> entradas;

    bool peliculasCargadas;
    bool salasCargadas;
    bool sesionesCargadas;
    bool clientesCargados;
    bool entradasCargadas;

public:
    Cache(Conexion &conexion);

    vector<Pelicula> &getPeliculas();
    vector<Sala> &getSalas();
    vector<Sesion> &getSesiones();
    vector<ClienteCine> &getClientes();
    vector<Entrada> &getEntradas();

    void invalidarPeliculas();
    void invalidarSalas();
    void invalidarSesiones();
    void invalidarClientes();
    void invalidarEntradas();
    void invalidarTodo();
};

#endif