#!/bin/bash

# Utilisation de find pour rechercher récursivement les fichiers avec plus de deux points consécutifs dans leur nom
find . -type f -regex '.*\.\..*\.md$' | while read file; do
  echo "Suppression de $file"
  rm "$file"
done
