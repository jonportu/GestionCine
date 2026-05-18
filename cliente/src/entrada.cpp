#include <sstream>
#include <vector>
#include "entrada.h"

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

Entrada::Entrada() : sesionId(0), clienteId(0), precio(0) {}

int Entrada::getSesionId() const { return sesionId; }
int Entrada::getClienteId() const { return clienteId; }
float Entrada::getPrecio() const { return precio; }
string Entrada::getFecha() const { return fecha; }

void Entrada::setSesionId(int i) { sesionId = i; }
void Entrada::setClienteId(int i) { clienteId = i; }

void Entrada::fromRespuesta(const string &linea) {
    vector<string> p = separar(linea);
    if (p.size() >= 5) {
        id = stoi(p[0]);
        sesionId = stoi(p[1]);
        clienteId = stoi(p[2]);
        precio = stof(p[3]);
        fecha = p[4];
    }
}

string Entrada::toString() const {
    return "[" + to_string(id) + "] Sesion:" + to_string(sesionId) +
           " Cliente:" + to_string(clienteId) + " | " +
           to_string(precio) + " eur | " + fecha;
}