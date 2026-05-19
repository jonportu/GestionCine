#!/bin/bash
cd "$(dirname "$0")"
g++ -Iinclude -Wall -std=c++11 -c src/main.cpp -o src/main.o
g++ -Iinclude -Wall -std=c++11 -c src/conexion.cpp -o src/conexion.o
g++ -Iinclude -Wall -std=c++11 -c src/entidad.cpp -o src/entidad.o
g++ -Iinclude -Wall -std=c++11 -c src/pelicula.cpp -o src/pelicula.o
g++ -Iinclude -Wall -std=c++11 -c src/sala.cpp -o src/sala.o
g++ -Iinclude -Wall -std=c++11 -c src/sesion.cpp -o src/sesion.o
g++ -Iinclude -Wall -std=c++11 -c src/clientecine.cpp -o src/clientecine.o
g++ -Iinclude -Wall -std=c++11 -c src/entrada.cpp -o src/entrada.o
g++ -Iinclude -Wall -std=c++11 -c src/cache.cpp -o src/cache.o
g++ -Iinclude -Wall -std=c++11 -c src/menu.cpp -o src/menu.o
g++ src/main.o src/conexion.o src/entidad.o src/pelicula.o src/sala.o src/sesion.o src/clientecine.o src/entrada.o src/cache.o src/menu.o -o bin/cliente.exe -lws2_32
echo "Compilado!"