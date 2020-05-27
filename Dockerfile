FROM debian:buster

RUN apt-get update && apt-get install -y build-essential clang libopenal-dev libsndfile1-dev libfreetype6-dev libassimp-dev libsdl2-dev libxcb-keysyms1-dev libx11-dev libfreetype6-dev mesa-common-dev libgl1-mesa-dev

RUN mkdir /NazaraEngine
WORKDIR /NazaraEngine