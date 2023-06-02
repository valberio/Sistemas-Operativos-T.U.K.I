#! /usr/bin/bash
cd ../memoria
> ./log_memoria.log
echo "PUERTO_ESCUCHA=8002
TAM_MEMORIA=4096
TAM_SEGMENTO_0=128
CANT_SEGMENTOS=16
RETARDO_MEMORIA=1000
RETARDO_COMPACTACION=60000
ALGORITMO_ASIGNACION=BEST" > ./configs/memoria.config
make
./bin/memoria.out > /dev/null & 
processmemoria=$!
sleep 2
cd ../cpu
> ./log_cpu.log
echo "RETARDO_INSTRUCCION=1000
IP_MEMORIA=127.0.0.1
PUERTO_MEMORIA=8002
PUERTO_ESCUCHA=8001
TAM_MAX_SEGMENTO=128" > ./configs/cpu.config
make
./bin/cpu.out > /dev/null &
processcpu=$!
sleep 2
cd ../kernel
> ./log_kernel.log
echo "IP_MEMORIA=127.0.0.1
PUERTO_MEMORIA=8002
IP_FILESYSTEM=127.0.0.1
PUERTO_FILESYSTEM=8003
IP_CPU=127.0.0.1
PUERTO_CPU=8001
PUERTO_ESCUCHA=8000
ALGORITMO_PLANIFICACION=FIFO
ESTIMACION_INICIAL=10000
HRRN_ALFA=0.5
GRADO_MAX_MULTIPROGRAMACION=3
RECURSOS=[IMPRESORA, DISCO,SCANNER]
INSTANCIAS_RECURSOS=[1,1,1]" > ./configs/config_kernel.config
make
./bin/kernel.out > /dev/null &
processkernel=$!
sleep 2
cd ../consola
> ./log_cpu.log
make
./bin/consola.out "configs/consola.config" "pruebas/DEADLOCK_1.txt" > /dev/null &
process1=$?
sleep 1
./bin/consola.out "configs/consola.config" "pruebas/DEADLOCK_2.txt" > /dev/null & 
process2=$?
sleep 1
./bin/consola.out "configs/consola.config" "pruebas/DEADLOCK_3.txt" > /dev/null &
process3=$?
sleep 5
./bin/consola.out "configs/consola.config" "pruebas/DEADLOCK_4.txt" > /dev/null &
process4=$?
opcion="sas"
while [ $opcion != "exit" ]
do
read -p "Imprimir resultados (consola,kernel,cpu,memoria) o exit: " opcion
case $opcion in 
    memoria)
        clear && cat ../memoria/log_memoria.log
        ;;
    consola)
        clear && cat ../consola/log_consola.log
        ;;
    kernel)
        clear && cat ../kernel/log_kernel.log
        ;;
    cpu)
        clear && cat ../cpu/log_cpu.log
        ;;
    proceso1)
        clear && echo $process1
        ;;
    proceso2)
        clear && echo $process2
        ;;
    proceso3)
        clear && echo $process3
        ;;
    proceso4)
        clear && echo $process4
        ;;
    exit)
        echo "Cerrando script."
        kill -9 $process1
        kill -9 $process2
        kill -9 $process3
        kill -9 $process4
        kill -9 $processkernel
        kill -9 $processcpu
        kill -9 $processmemoria
        ;;
    *)
        echo "No es una opcion."
        ;;
esac
done



