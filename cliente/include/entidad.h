#ifndef ENTIDAD_H
#define ENTIDAD_H

#include <string>

using namespace std;

class Entidad {
protected:
    int id;

public:
    Entidad();
    virtual ~Entidad();

    int getId() const;
    void setId(int id);

    virtual string toString() const = 0;
    virtual void fromRespuesta(const string &linea) = 0;
};

#endif