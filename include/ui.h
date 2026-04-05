#ifndef UI_H
#define UI_H

void ui_ejecutar(void);

void ui_pantalla_bienvenida(void);

int ui_pantalla_login(void);

void ui_menu_principal(void);

void ui_menu_peliculas(void);
void ui_menu_salas(void);
void ui_menu_sesiones(void);
void ui_menu_clientes(void);
void ui_menu_venta_entradas(void);
void ui_menu_administracion(void);

void ui_imprimir_cabecera(const char *titulo);
void ui_imprimir_error(const char *mensaje);
void ui_imprimir_exito(const char *mensaje);

#endif