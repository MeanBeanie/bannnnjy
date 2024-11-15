#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Window/Event.hpp>
#include <vector>

// whenever you change font scaling will 9/11 itself
// just fuck the divide to be in terms of 16
// such that 64 is a divide of 4
// idfk why this shit is like that blame god for making me
const int CHAR_SIZE = 64;

struct Note {
	Note(){};

	int division;
	int fret;
	float beat;
	int line;
};

struct Data {
	Data(){};

	int div;
	int beat;
};

class App{
	public:
		App();
		void run();
		void quit();

	private:
		void resizeCallback();
		void onClick(sf::Event event);
		void keyboardCallback(sf::Event event);

		void changeDivision(bool up = true, bool loop = false);
		void changeFret(bool up = true, int specific = -1);

		// my soul died then was reborn because of this
		Data lastBeatDiv(int beat);

		int status = 0;
		float scales[2] = {1.0f, 1.0f};
		sf::Vector2f mousePos;
		float selected[2] = {-1, 0};
		int hoveringNote = -1;

		int division = 2;
		int fret = 0;
		int timeSig[2] = {4, 4};
		std::string title = "New Banjy";
		std::vector<Note> notes;

		sf::RenderWindow window;

		sf::Font font;
		sf::Text divisionChanger;
		sf::Text fretChanger;
		sf::Text titleLabel;
		sf::Text timeSigLabels[2];

		sf::RectangleShape sidebarBackground;
		sf::CircleShape cursor;
};
