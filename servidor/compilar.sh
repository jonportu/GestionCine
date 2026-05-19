#!/bin/bash
cd "$(dirname "$0")"
gcc -Iinclude -Wall -std=c11 -c src/main.c -o src/main.o
gcc -Iinclude -Wall -std=c11 -c src/red.c -o src/red.o
gcc -Iinclude -Wall -std=c11 -c src/protocolo.c -o src/protocolo.o
gcc -Iinclude -Wall -std=c11 -c src/auth.c -o src/auth.o
gcc -Iinclude -Wall -std=c11 -c src/utiles.c -o src/utiles.o
gcc -Iinclude -Wall -std=c11 -c src/db.c -o src/db.o
gcc -Iinclude -O2 -DSQLITE_THREADSAFE=0 -DSQLITE_OMIT_LOAD_EXTENSION -c src/sqlite3.c -o src/sqlite3.o
gcc src/main.o src/red.o src/protocolo.o src/auth.o src/utiles.o src/db.o src/sqlite3.o -o bin/servidor.exe -lws2_32 -lm
echo "Compilado!"