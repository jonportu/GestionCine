#ifndef RED_SERVIDOR_H
#define RED_SERVIDOR_H

int red_iniciar(int puerto);
int red_esperar_cliente(void);
int red_enviar(const char *mensaje);
int red_recibir(char *buffer, int max);
void red_cerrar_cliente(void);
void red_cerrar(void);

#endif