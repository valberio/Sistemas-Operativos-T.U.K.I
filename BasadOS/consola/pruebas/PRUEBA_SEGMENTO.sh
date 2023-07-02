#! /usr/bin/bash
cd ../memoria
> ./log_memoria.log
make
./bin/memoria.out > /dev/null & 
processmemoria=$!
sleep 2
cd ../filesystem
> ./log_filesystem.log
make
./bin/filesystem.out > /dev/null & 
processfilesystem=$!
sleep 2
cd ../cpu
> ./log_cpu.log
make
./bin/cpu.out > /dev/null &
processcpu=$!
sleep 2
cd ../kernel
> ./log_kernel.log
make
./bin/kernel.out > /dev/null &
processkernel=$!
sleep 2
cd ../consola
> ./log_consola.log
make
./bin/consola.out "configs/consola.config" "pruebas/prueba.txt" > /dev/null &
process1=$!
sleep 1
wait $process1
opcion="sas"
while [ $opcion != "exit" ]
do
read -p "Imprimir resultados (filesystem,consola,kernel,cpu,memoria) o exit: " opcion
case $opcion in 
    filesystem)
        clear && cat ../memoria/log_filesystem.log
        ;;
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
    exit)
        echo "Cerrando script."
        kill -9 $processkernel
        kill -9 $processcpu
        kill -9 $processfilesystem
        kill -9 $processmemoria
        ;;
    *)
        echo "No es una opcion."
        ;;
esac
done
