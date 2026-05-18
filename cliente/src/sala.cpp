#include <sstream>
#include <vector>
#include "sala.h"

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

Sala::Sala() : numero(0), capacidad(0) {}

int Sala::getNumero() const { return numero; }
int Sala::getCapacidad() const { return capacidad; }

void Sala::setNumero(int n) { numero = n; }
void Sala::setCapacidad(int c) { capacidad = c; }

void Sala::fromRespuesta(const string &linea) {
    vector<string> p = separar(linea);
    if (p.size() >= 3) {
        id = stoi(p[0]);
        numero = stoi(p[1]);
        capacidad = stoi(p[2]);
    }
}

string Sala::toString() const {
    return "[" + to_string(id) + "] Sala " + to_string(numero) +
           " | Capacidad: " + to_string(capacidad);
}

string Sala::toComando() const {
    return to_string(numero) + "|" + to_string(capacidad);
}

string Sala::toComandoConId() const {
    return to_string(id) + "|" + toComando();
}