#ifndef UTILES_H
#define UTILES_H

void leer_string(const char *prompt, char *destino, int max);
int leer_entero(const char *prompt);
float leer_float(const char *prompt);
int confirmar(const char *prompt);
void pausar(void);
void limpiar(void);
void escribir_log(const char *mensaje);

#endif