CC = g++
NAME = bannnjy
CXXFLAGS = -std=c++20 $(shell pkg-config sfml-all --cflags --libs)

all:
	@mkdir temp
	@cp *.* temp
	@$(CC) temp/*.cpp -o $(NAME) $(CXXFLAGS)
	@rm -r temp
	@echo "Bilded"
debug:
	mkdir temp
	cp * temp
	cd temp
	@$(CC) temp/*.cpp -o $(NAME)-debug $(CXXFLAGS) -g
	@rm -r temp
	@echo "Bilded"
