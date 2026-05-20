# Script para limpiar y generar el ZIP del Hito Final

Write-Host "Iniciando limpieza del proyecto..." -ForegroundColor Cyan

# Eliminar binarios .exe en todos los subdirectorios bin/
if (Test-Path "bin\*.exe") { Remove-Item "bin\*.exe" -Force }
if (Test-Path "servidor\bin\*.exe") { Remove-Item "servidor\bin\*.exe" -Force }
if (Test-Path "cliente\bin\*.exe") { Remove-Item "cliente\bin\*.exe" -Force }

# Eliminar objetos .o en todos los subdirectorios src/
Get-ChildItem -Path . -Filter "*.o" -Recurse | Remove-Item -Force

# Eliminar log temporal
if (Test-Path "data\servidor.log") { Remove-Item "data\servidor.log" -Force }

# Eliminar el ejecutable temporal de insercion de datos
if (Test-Path "insertar_datos.exe") { Remove-Item "insertar_datos.exe" -Force }

Write-Host "Limpieza completada." -ForegroundColor Green

$ZipPath = "GestionCine_Final.zip"
if (Test-Path $ZipPath) { Remove-Item $ZipPath -Force }

Write-Host "Generando fichero ZIP ($ZipPath)..." -ForegroundColor Cyan

# Excluir la carpeta oculta .git y el propio zip
Compress-Archive -Path ".\*" -DestinationPath $ZipPath -Update -ErrorAction SilentlyContinue

Write-Host "==========================================" -ForegroundColor Green
Write-Host "  ZIP generado con exito: $ZipPath" -ForegroundColor Green
Write-Host "==========================================" -ForegroundColor Green
