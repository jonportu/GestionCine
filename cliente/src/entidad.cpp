#include "entidad.h"

Entidad::Entidad() : id(0) {}
Entidad::~Entidad() {}

int Entidad::getId() const { return id; }
void Entidad::setId(int i) { id = i; }