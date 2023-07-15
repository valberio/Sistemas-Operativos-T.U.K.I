#!/bin/bash

if [ "$#" -lt 1 ]; then
  echo "Escribir el nuevo valor para IP_KERNEL."
  exit 1
fi

new_ip_kernel="$1"

for config_file in ./configs/*.config; do
<<<<<<< HEAD
  sed -i "s/^IP=.*/IP=$new_ip_kernel/" "$config_file"
=======
  sed -i "s/^IP_MEMORIA=.*/IP=$new_ip_kernel/" "$config_file"
>>>>>>> 50ddf994be86765be7e1e7207e47a50abf63a94e
done
