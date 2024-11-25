CC = g++
NAME = bannnjy
CXXFLAGS = -std=c++20 $(shell pkg-config sfml-all --cflags --libs)

all:
	@$(CC) *.cpp -o $(NAME) $(CXXFLAGS)
	@echo "Bilded"
debug:
	@$(CC) *.cpp -o $(NAME)-debug $(CXXFLAGS) -g
	@echo "Bilded"
