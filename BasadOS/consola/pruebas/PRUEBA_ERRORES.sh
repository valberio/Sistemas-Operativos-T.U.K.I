#! /usr/bin/bash
cd ../cpu
> ./log_cpu.log
echo "RETARDO_INSTRUCCION=1000
IP_MEMORIA=127.0.0.1
PUERTO_MEMORIA=8002
PUERTO_ESCUCHA=80001
TAM_MAX_SEGMENTO=128" > ./configs/cpu.config
make
./bin/cpu.out > /dev/null &
sleep 2
cd ../kernel
> ./log_kernel.log
echo "IP_MEMORIA=127.0.0.1
PUERTO_MEMORIA=8002
IP_FILESYSTEM=127.0.0.1
PUERTO_FILESYSTEM=8003
IP_CPU=127.0.0.1
PUERTO_CPU=80001
PUERTO_ESCUCHA=80000
ALGORITMO_PLANIFICACION=FIFO
ESTIMACION_INICIAL=10000
HRRN_ALFA=0.5
GRADO_MAX_MULTIPROGRAMACION=4
RECURSOS=[DISCO]
INSTANCIAS_RECURSOS=[1]" > ./configs/config_kernel.config
make
./bin/kernel.out > /dev/null &
sleep 2
cd ../consola
> ./log_consola.log
make
./bin/consola.out "configs/consola.config" "pruebas/ERROR_1.txt" > /dev/null &
process1=$!
sleep 1
./bin/consola.out "configs/consola.config" "pruebas/ERROR_2.txt" > /dev/null & 
process2=$!
wait $process1
wait $process2
opcion="sas"
while [ $opcion != "exit" ]
do
read -p "Imprimir resultados (consola,kernel,cpu) o exit: " opcion
case $opcion in 
    consola)
        clear && cat ../consola/log_consola.log
        ;;
    kernel)
        clear && cat ../kernel/log_kernel.log
        ;;
    cpu)
        clear && cat ../cpu/log_cpu.log
        ;;
    exit)
        echo "Cerrando script."
        ;;
    *)
        echo "No es una opcion."
        ;;
esac
done


