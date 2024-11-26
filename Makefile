CC = g++
NAME = bannnjy
CXXFLAGS = -std=c++20 $(shell pkg-config sfml-all --cflags --libs)

linux:
	@$(CC) *.cpp -o $(NAME)-linux $(CXXFLAGS)
	@echo "Bilded"
osx:
	o64-clang++ *.cpp -O3 -o Bannnjy.app/Contents/MacOS/Bannnjy --std=c++20 -LBannnjy.app/Contents/Resources -lsfml-window -lsfml-graphics -lsfml-system
