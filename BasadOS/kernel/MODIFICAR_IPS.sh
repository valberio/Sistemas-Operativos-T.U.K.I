#!/bin/bash

#!/bin/bash

if [ "$#" -lt 3 ]; then
  echo "Escribir el nuevo valor para IP_MEMORIA, IP_FILESYSTEM, and IP_CPU."
  exit 1
fi

new_ip_memoria="$1"
new_ip_filesystem="$2"
new_ip_cpu="$3"

for config_file in ./configs/*.config; do
  sed -i "s/^IP_MEMORIA=.*/IP_MEMORIA=$new_ip_memoria/" "$config_file"
  sed -i "s/^IP_FILESYSTEM=.*/IP_FILESYSTEM=$new_ip_filesystem/" "$config_file"
  sed -i "s/^IP_CPU=.*/IP_CPU=$new_ip_cpu/" "$config_file"
done
