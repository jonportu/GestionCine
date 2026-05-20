#!/bin/bash
gcc -Iinclude -Wall -std=c11 -c src/main.c -o src/main.o
gcc -Iinclude -Wall -std=c11 -c src/ui.c -o src/ui.o
gcc -Iinclude -Wall -std=c11 -c src/db.c -o src/db.o
gcc -Iinclude -Wall -std=c11 -c src/auth.c -o src/auth.o
gcc -Iinclude -Wall -std=c11 -c src/utiles.c -o src/utiles.o
gcc -Iinclude -Wall -std=c11 -c src/reportes.c -o src/reportes.o
gcc -Iinclude -O2 -DSQLITE_THREADSAFE=0 -DSQLITE_OMIT_LOAD_EXTENSION -c src/sqlite3.c -o src/sqlite3.o
gcc src/main.o src/ui.o src/db.o src/auth.o src/utiles.o src/reportes.o src/sqlite3.o -o bin/programa.exe -lm
echo "Compilado!"