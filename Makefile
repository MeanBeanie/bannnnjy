CC = g++
NAME = bannnjy
CXXFLAGS = -std=c++20 $(shell pkg-config sfml-all --cflags --libs)

linux:
	@$(CC) *.cpp -o $(NAME)-linux $(CXXFLAGS)
	@echo "Bilded"
osxcross:
	export OSXCROSS_MP_INC=1
	o64-clang++ *.cpp -O3 -o Bannnjy.app/Contents/MacOS/Bannnjy --std=c++20 -LBannnjy.app/Contents/Frameworks -lsfml-window -lsfml-graphics -lsfml-system -mmacosx-version-min=12.0
