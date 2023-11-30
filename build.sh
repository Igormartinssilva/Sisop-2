#!/bin/bash

# Diretório Server
echo "Compilando Server..."
cd server
make
cd ..

# Diretório client
echo "Compilando client..."
cd client
make
cd ..

echo "Compilação concluída."
