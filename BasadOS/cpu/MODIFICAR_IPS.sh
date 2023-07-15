#!/bin/bash

if [ "$#" -lt 1 ]; then
  echo "Escribir el nuevo valor para IP_MEMORIA."
  exit 1
fi

new_ip_memoria="$1"

for config_file in ./configs/*.config; do
  sed -i "s/^IP_MEMORIA=.*/IP_MEMORIA=$new_ip_memoria/" "$config_file"
done
