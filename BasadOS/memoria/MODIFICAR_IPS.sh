#!/bin/bash

new_value="$1"

# Check if the new value is provided
if [ -z "$new_value" ]; then
  echo "Escribir el nuevo valor para IP_MEMORIA."
  exit 1
fi

# Update the IP_MEMORIA value in the config file
for config_file in ./configs/*.config; do
  # Update the IP_MEMORIA value in each config file
  sed -i "s/IP_MEMORIA=.*/IP_MEMORIA=$new_value/" "$config_file"
done