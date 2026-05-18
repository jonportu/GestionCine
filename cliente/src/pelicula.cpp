#include <sstream>
#include <vector>
#include "pelicula.h"

using namespace std;

static vector<string> separar(const string &linea) {
    vector<string> partes;
    stringstream ss(linea);
    string parte;
    while (getline(ss, parte, '|')) {
        partes.push_back(parte);
    }
    return partes;
}

Pelicula::Pelicula() : duracion(0), edad(0) {}

string Pelicula::getTitulo() const { return titulo; }
string Pelicula::getGenero() const { return genero; }
int Pelicula::getDuracion() const { return duracion; }
int Pelicula::getEdad() const { return edad; }

void Pelicula::setTitulo(const string &t) { titulo = t; }
void Pelicula::setGenero(const string &g) { genero = g; }
void Pelicula::setDuracion(int d) { duracion = d; }
void Pelicula::setEdad(int e) { edad = e; }

void Pelicula::fromRespuesta(const string &linea) {
    vector<string> p = separar(linea);
    if (p.size() >= 5) {
        id = stoi(p[0]);
        titulo = p[1];
        genero = p[2];
        duracion = stoi(p[3]);
        edad = stoi(p[4]);
    }
}

string Pelicula::toString() const {
    return "[" + to_string(id) + "] " + titulo + " | " + genero +
           " | " + to_string(duracion) + " min | +" + to_string(edad);
}

string Pelicula::toComando() const {
    return titulo + "|" + genero + "|" + to_string(duracion) + "|" + to_string(edad);
}

string Pelicula::toComandoConId() const {
    return to_string(id) + "|" + toComando();
}