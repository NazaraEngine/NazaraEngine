#!/bin/bash

# Fonction pour ajouter le header à chaque fichier .md
process_file() {
  local file="$1"

  # Récupère le nom du fichier sans extension
  filename=$(basename "$file" .md)

  # Création du nouveau header
  header="---
title: ${filename//::/ }
description: Nothing
---"

  # Ajoute le header au début du fichier en sauvegardant le reste du contenu
  content=$(cat "$file")
  echo -e "$header\n\n$content" > "$file"

  echo "Header ajouté à $file"
}

# Utilisation de find pour parcourir les fichiers .md récursivement
find . -type f -name "*.md" | while read file; do
  process_file "$file"
done
