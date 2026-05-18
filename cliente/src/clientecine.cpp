#include <sstream>
#include <vector>
#include "clientecine.h"

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

ClienteCine::ClienteCine() {}

string ClienteCine::getNombre() const { return nombre; }
string ClienteCine::getEmail() const { return email; }
string ClienteCine::getTelefono() const { return telefono; }

void ClienteCine::setNombre(const string &n) { nombre = n; }
void ClienteCine::setEmail(const string &e) { email = e; }
void ClienteCine::setTelefono(const string &t) { telefono = t; }

void ClienteCine::fromRespuesta(const string &linea) {
    vector<string> p = separar(linea);
    if (p.size() >= 4) {
        id = stoi(p[0]);
        nombre = p[1];
        email = p[2];
        telefono = p[3];
    }
}

string ClienteCine::toString() const {
    return "[" + to_string(id) + "] " + nombre + " | " + email + " | " + telefono;
}

string ClienteCine::toComando() const {
    return nombre + "|" + email + "|" + telefono;
}

string ClienteCine::toComandoConId() const {
    return to_string(id) + "|" + toComando();
}