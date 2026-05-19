#include <iostream>
#include "conexion.h"
#include "cache.h"
#include "menu.h"

using namespace std;

int main() {
    Conexion con;

    if (!con.conectar("127.0.0.1", 5000)) {
        cout << "No se pudo conectar al servidor." << endl;
        cout << "Asegurate de que el servidor esta arrancado." << endl;
        return 1;
    }

    Cache cache(con);
    Menu menu(con, cache);

    if (menu.pantallaLogin()) {
        menu.menuPrincipal();
    }

    con.enviar("SALIR");
    con.recibir();
    con.desconectar();

    cout << "Desconectado." << endl;
    return 0;
}