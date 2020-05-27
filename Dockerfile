FROM debian:stretch

RUN apt-get update && apt-get install -y build-essential clang libopenal-dev libsndfile1-dev libfreetype6-dev libassimp-dev libsdl2-dev

RUN mkdir /NazaraEngine
WORKDIR /NazaraEngine