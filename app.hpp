#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Window/Event.hpp>

// whenever you change font scaling will 9/11 itself
// just fuck the divide to be in terms of 16
// such that 64 is a divide of 4
// idfk why this shit is like that blame god for making me
const int CHAR_SIZE = 64;
const int CHAR_SCALE = 4;

struct Note {
	Note(){
		division = -1;
		fret = -1;
		lineOffset = -1;
		lineIndex = -1;
	};

	int division = -1;
	int fret = -1;
	int lineOffset = -1;
	int lineIndex = -1;
};

class App{
	public:
		App();
		void run();
		void quit();

	private:
		void onResize();
		void onClick(sf::Event event);
		void onKeyPressed(sf::Event event);

		void changeDivision(bool up = true, bool loop = false);
		void changeFret(bool up = true, int specific = -1);
		void deleteNote();

		std::string getDocumentFolder();
		void saveToFile();
		void loadFromFile(std::string path);
		void launchFileDialog();

		int status = 0;
		float scales[2] = {1.0f, 1.0f};
		sf::Vector2f mousePos;

		Note hoveredNote;
		int selectedIndex = -1;
		int timeSig[2] = {4, 4};
		std::string title = "New Banjy";
		std::vector<Note> notes;
		std::multimap<int, Note> towers;

		sf::RenderWindow window;

		sf::Font font;
		sf::Text divisionChanger;
		sf::Text fretChanger;
		sf::Text titleLabel;
		sf::Text timeSigLabels[2];
		sf::Texture eightNoteCurlyTexture;
		sf::Texture sixteenthNoteCurlyTexture;
		sf::Sprite eightNoteCurly;
		sf::Sprite sixteenthNoteCurly;

		sf::RectangleShape sidebarBackground;
};
