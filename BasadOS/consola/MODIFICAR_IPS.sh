#!/bin/bash

if [ "$#" -lt 1 ]; then
  echo "Escribir el nuevo valor para IP_KERNEL."
  exit 1
fi

new_ip_kernel="$1"

for config_file in ./configs/*.config; do
  sed -i "s/^IP_MEMORIA=.*/IP=$new_ip_kernel/" "$config_file"
done
