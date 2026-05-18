#include <sstream>
#include <vector>
#include "sesion.h"

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

Sesion::Sesion() : peliculaId(0), salaId(0), precio(0), vendidas(0) {}

int Sesion::getPeliculaId() const { return peliculaId; }
int Sesion::getSalaId() const { return salaId; }
string Sesion::getFecha() const { return fecha; }
string Sesion::getHora() const { return hora; }
float Sesion::getPrecio() const { return precio; }
int Sesion::getVendidas() const { return vendidas; }

void Sesion::setPeliculaId(int i) { peliculaId = i; }
void Sesion::setSalaId(int i) { salaId = i; }
void Sesion::setFecha(const string &f) { fecha = f; }
void Sesion::setHora(const string &h) { hora = h; }
void Sesion::setPrecio(float p) { precio = p; }

void Sesion::fromRespuesta(const string &linea) {
    vector<string> p = separar(linea);
    if (p.size() >= 7) {
        id = stoi(p[0]);
        peliculaId = stoi(p[1]);
        salaId = stoi(p[2]);
        fecha = p[3];
        hora = p[4];
        precio = stof(p[5]);
        vendidas = stoi(p[6]);
    }
}

string Sesion::toString() const {
    return "[" + to_string(id) + "] Pelicula:" + to_string(peliculaId) +
           " Sala:" + to_string(salaId) + " | " + fecha + " " + hora +
           " | " + to_string(precio) + " eur | Vendidas:" + to_string(vendidas);
}

string Sesion::toComando() const {
    return to_string(peliculaId) + "|" + to_string(salaId) + "|" +
           fecha + "|" + hora + "|" + to_string(precio);
}

string Sesion::toComandoConId() const {
    return to_string(id) + "|" + toComando();
}