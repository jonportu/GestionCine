#include <iostream>
#include <sstream>
#include "cache.h"

using namespace std;

Cache::Cache(Conexion &conexion) : con(conexion) {
    peliculasCargadas = false;
    salasCargadas = false;
    sesionesCargadas = false;
    clientesCargados = false;
    entradasCargadas = false;
}

template <typename T>
static vector<T> pedirLista(Conexion &con, const string &comando) {
    vector<T> lista;

    con.enviar(comando);
    string primera = con.recibir();

    if (primera.substr(0, 2) != "OK") {
        return lista;
    }

    int n = 0;
    size_t pos = primera.find('|');
    if (pos != string::npos) {
        n = stoi(primera.substr(pos + 1));
    }

    for (int i = 0; i < n; i++) {
        string linea = con.recibir();
        T obj;
        obj.fromRespuesta(linea);
        lista.push_back(obj);
    }

    return lista;
}

vector<Pelicula> &Cache::getPeliculas() {
    if (!peliculasCargadas) {
        peliculas = pedirLista<Pelicula>(con, "LISTAR_PELICULAS");
        peliculasCargadas = true;
    }
    return peliculas;
}

vector<Sala> &Cache::getSalas() {
    if (!salasCargadas) {
        salas = pedirLista<Sala>(con, "LISTAR_SALAS");
        salasCargadas = true;
    }
    return salas;
}

vector<Sesion> &Cache::getSesiones() {
    if (!sesionesCargadas) {
        sesiones = pedirLista<Sesion>(con, "LISTAR_SESIONES");
        sesionesCargadas = true;
    }
    return sesiones;
}

vector<ClienteCine> &Cache::getClientes() {
    if (!clientesCargados) {
        clientes = pedirLista<ClienteCine>(con, "LISTAR_CLIENTES");
        clientesCargados = true;
    }
    return clientes;
}

vector<Entrada> &Cache::getEntradas() {
    if (!entradasCargadas) {
        entradas = pedirLista<Entrada>(con, "LISTAR_ENTRADAS");
        entradasCargadas = true;
    }
    return entradas;
}

void Cache::invalidarPeliculas() { peliculasCargadas = false; peliculas.clear(); }
void Cache::invalidarSalas() { salasCargadas = false; salas.clear(); }
void Cache::invalidarSesiones() { sesionesCargadas = false; sesiones.clear(); }
void Cache::invalidarClientes() { clientesCargados = false; clientes.clear(); }
void Cache::invalidarEntradas() { entradasCargadas = false; entradas.clear(); }
void Cache::invalidarTodo() {
    invalidarPeliculas();
    invalidarSalas();
    invalidarSesiones();
    invalidarClientes();
    invalidarEntradas();
}