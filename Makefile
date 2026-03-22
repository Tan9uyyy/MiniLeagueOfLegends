.PHONY: all prepare build run clean

# Commande par défaut (lancée avec "make")
all: build run

# Préparation du projet avec CMake
prepare:
	cmake -S . -B build
	ln -sf build/compile_commands.json ./compile_commands.json

# Compilation du projet
build: prepare
	cmake --build build

# Lancement du jeu
run:
	./build/bin/MiniLeagueOfLegends

# Nettoyage des dossiers générés
clean:
	rm -rf build
