FROM debian:stretch

RUN apt-get update && apt-get install -y build-essential clang libopenal-dev libsndfile1-dev libxcb-cursor-dev libxcb-ewmh-dev libxcb-randr0-dev libxcb-icccm4-dev libxcb-keysyms1-dev libx11-dev libfreetype6-dev mesa-common-dev libgl1-mesa-dev libassimp-dev

RUN mkdir /NazaraEngine
WORKDIR /NazaraEngine