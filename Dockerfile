FROM debian:stretch

RUN apt-get update && apt-get -y install && \
	apt-get install -y clang && \
	apt-get install -y libopenal-dev libsndfile1-dev && \
	apt-get install -y libxcb-cursor-dev libxcb-ewmh-dev libxcb-randr0-dev libxcb-icccm4-dev libxcb-keysyms1-dev libx11-dev libfreetype6-dev && \
	apt-get install -y mesa-common-dev libgl1-mesa-dev && \
	apt-get install -y libassimp-dev

RUN mkdir /NazaraEngine
WORKDIR /NazaraEngine