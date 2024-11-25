#include "app.hpp"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/Window.h>
#include <SFML/Window/WindowStyle.hpp>
#include <chrono>
#include <iostream>
#include <thread>

App::App(){
	window.create(sf::VideoMode(800, 450), "Bbannjjyy");
	sidebarBackground.setSize(sf::Vector2f(800.f, 22.f));
	sidebarBackground.setFillColor(sf::Color(175, 175, 175));
	if(!font.loadFromFile("./rsrcs/BreeSerif-Regular.ttf")){
		std::cerr << "Failed to load font BreeSerif-Regular" << std::endl;
		status = -1;
	}
	font.setSmooth(false);

	hoveredNote.division = timeSig[1];

	divisionChanger.setFont(font);
	divisionChanger.setCharacterSize(CHAR_SIZE);
	divisionChanger.setString("< 1/" + std::to_string(hoveredNote.division) + " >");
	divisionChanger.setPosition(2, 2);

	fretChanger.setFont(font);
	fretChanger.setCharacterSize(CHAR_SIZE);
	fretChanger.setString("< 0 >");
	fretChanger.setPosition(20+divisionChanger.getGlobalBounds().width, 2);

	titleLabel.setFont(font);
	titleLabel.setCharacterSize(CHAR_SIZE);
	titleLabel.setString(title);
	titleLabel.setPosition(790-(titleLabel.getGlobalBounds().width), 2);

	for(int i = 0; i < 2; i++){
		timeSigLabels[i].setFont(font);
		timeSigLabels[i].setCharacterSize(CHAR_SIZE);
		timeSigLabels[i].setScale(scales[0]/3, scales[1]/3);
		timeSigLabels[i].setString(std::to_string(timeSig[i]));
		timeSigLabels[i].setFillColor(sf::Color::Black);
	}

	hoveredNote.fret = 0;
}

void App::run(){
	sf::Clock clock;

	sf::Event event;
	while(status >= 0){
		clock.restart();
		while(window.pollEvent(event)){
			switch(event.type){
				case sf::Event::Closed: quit(); break;
				case sf::Event::Resized: onResize(); break;
				case sf::Event::MouseButtonPressed: onClick(event); break;
				case sf::Event::MouseMoved: mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window)); break;
				case sf::Event::KeyPressed: onKeyPressed(event); break;
				default: break;
			}
		}

		window.clear(sf::Color::White);

		window.draw(sidebarBackground);
		window.draw(divisionChanger);
		window.draw(fretChanger);
		window.draw(titleLabel);

		sf::Vector2f dist(99, 99);

		int count = 0;
		sf::RectangleShape rect(sf::Vector2f(780*scales[0], scales[1]));
		rect.setFillColor(sf::Color::Black);
		std::vector<int> linePixelStarties;
		int lineIndex = 0;
		int lineOffset = 0;
		linePixelStarties.push_back((42*scales[1])-(10*scales[1]));
		bool firstRound = true;
		for(int y = 42*scales[1]; y < 430*scales[1]; y += 10*scales[1]){
			if(count < 5){
				if(mousePos.y > 35*scales[1] && std::abs(mousePos.y-y) < dist.y){
					dist.y = std::abs(mousePos.y-y);
					hoveredNote.lineOffset = lineOffset;
					hoveredNote.lineIndex = lineIndex;
				}
				rect.setPosition(10*scales[0], y);
				window.draw(rect);
				lineOffset++;
			}
			else if(count >= 7){
				count = 0;
				linePixelStarties.push_back(y);
				lineIndex++;
				lineOffset = 0;
				firstRound = false;
				continue;
			}
			if(firstRound){
				if(count == 1){
					timeSigLabels[0].setPosition(10*scales[0], y-12*scales[1]);
					window.draw(timeSigLabels[0]);
				}
				else if(count == 2){
					timeSigLabels[1].setPosition(10*scales[0], y-3*scales[1]);
					window.draw(timeSigLabels[1]);
				}
			}
			count++;
		}

		float x = 25*scales[0];
		lineIndex = 0;
		float beatsPassed = 0.f;

		sf::Text fred;
		fred.setFont(font);
		fred.setCharacterSize(CHAR_SIZE);
		fred.setScale(scales[0]/4, scales[1]/4);
		fred.setFillColor(sf::Color::Black);
		
		sf::RectangleShape measureBar(sf::Vector2f(2*scales[0], 50*scales[1]));
		measureBar.setFillColor(sf::Color::Black);

		sf::RectangleShape divisionIndicator(sf::Vector2f(scales[0], 18*scales[1]));
		divisionIndicator.setFillColor(sf::Color::Black);
		sf::RectangleShape dualFractyNotes(sf::Vector2f(35*scales[0], scales[1]));
		dualFractyNotes.setFillColor(sf::Color::Black);

		selectedIndex = -1;
		for(int i = 0; i < notes.size(); i++){
			float noteScale = (float)timeSig[1]/notes[i].division;

			fred.setString(std::to_string(notes[i].fret));
			fred.setPosition(x, linePixelStarties[lineIndex]+notes[i].lineOffset*10*scales[1]);
			window.draw(fred);

			divisionIndicator.setPosition(x+(fred.getGlobalBounds().width/2), linePixelStarties[lineIndex]+55*scales[1]);
			if(notes[i].division == 8){
				if(i+1 < notes.size()){
					if(notes[i+1].division == 8 && beatsPassed + noteScale < timeSig[0]){
						dualFractyNotes.setPosition(x+(fred.getGlobalBounds().width/2), linePixelStarties[lineIndex]+55*scales[1]+18*scales[1]);
						dualFractyNotes.setSize(sf::Vector2f(35*scales[0]*noteScale, scales[1]));
						window.draw(dualFractyNotes);
					}
					else if(notes[i+1].division == 16 && beatsPassed + (timeSig[1]/16.f) < timeSig[0]){
						dualFractyNotes.setPosition(x+(fred.getGlobalBounds().width/2), linePixelStarties[lineIndex]+55*scales[1]+18*scales[1]);
						dualFractyNotes.setSize(sf::Vector2f(35*scales[0]*noteScale, scales[1]));
						window.draw(dualFractyNotes);
						dualFractyNotes.setPosition(x+(fred.getGlobalBounds().width/2)+17.5*scales[0]*noteScale, linePixelStarties[lineIndex]+55*scales[1]+14*scales[1]);
						dualFractyNotes.setSize(sf::Vector2f(17.5f*scales[0]*noteScale, scales[1]));
						window.draw(dualFractyNotes);
					}
				}
				else{
					// draw lil curly
				}
			}
			if(notes[i].division == 16){
				if(i+1 < notes.size()){
					if(notes[i+1].division == 16 && beatsPassed + noteScale < timeSig[0]){
						dualFractyNotes.setPosition(x+(fred.getGlobalBounds().width/2), linePixelStarties[lineIndex]+55*scales[1]+18*scales[1]);
						dualFractyNotes.setSize(sf::Vector2f(35*scales[0]*noteScale, scales[1]));
						window.draw(dualFractyNotes);
						dualFractyNotes.setPosition(x+(fred.getGlobalBounds().width/2), linePixelStarties[lineIndex]+55*scales[1]+14*scales[1]);
						window.draw(dualFractyNotes);
					}
					else if(notes[i+1].division == 8 && beatsPassed + (timeSig[1]/8.f) < timeSig[0]){
						dualFractyNotes.setPosition(x+(fred.getGlobalBounds().width/2), linePixelStarties[lineIndex]+55*scales[1]+18*scales[1]);
						dualFractyNotes.setSize(sf::Vector2f(35*scales[0]*noteScale, scales[1]));
						window.draw(dualFractyNotes);
						dualFractyNotes.setPosition(x+(fred.getGlobalBounds().width/2), linePixelStarties[lineIndex]+55*scales[1]+14*scales[1]);
						dualFractyNotes.setSize(sf::Vector2f(17.5f*scales[0]*noteScale, scales[1]));
						window.draw(dualFractyNotes);
					}
				}
				else{
					// draw curly fry
				}
			}
			window.draw(divisionIndicator);

			if(towers.contains(i)){
				auto range = towers.equal_range(i);
				for(auto link = range.first; link != range.second; link++){
					fred.setString(std::to_string(link->second.fret));
					fred.setPosition(x, linePixelStarties[lineIndex]+link->second.lineOffset*10*scales[1]);
					window.draw(fred);
				}
			}

			if(std::abs(mousePos.x - x) < 15*scales[0] && std::abs(mousePos.x - x) < dist.x){
				dist.x = std::abs(mousePos.x - x);
				selectedIndex = i;
			}
			
			x += 35*scales[0]*noteScale;
			
			beatsPassed += noteScale;
			if(beatsPassed == timeSig[0]){
				measureBar.setPosition(x, linePixelStarties[lineIndex]);
				x += 5*scales[0];
				window.draw(measureBar);
				beatsPassed = 0.f;
			}
			else if(beatsPassed > timeSig[0]){
				measureBar.setFillColor(sf::Color::Red);
				measureBar.setPosition(x, linePixelStarties[lineIndex]);
				x += 5*scales[0];
				window.draw(measureBar);
				beatsPassed = 0.f;
			}
			
			if(x >= 760*scales[0] && i+1 < notes.size()){
				x = 25*scales[0];
				lineIndex++;
				if(lineIndex >= linePixelStarties.size()){
					// TODO add a lil scrolly wolly
					break;
				}
			}
		}

		window.display();

		if(clock.getElapsedTime().asMilliseconds() < 16){
			std::this_thread::sleep_for(std::chrono::milliseconds(16-clock.getElapsedTime().asMilliseconds()));
		}
	}
}

// TODO put all the other quitting shit here
void App::quit(){
	status = -1;
	window.close();
}

void App::onResize(){
	sf::Vector2f size = static_cast<sf::Vector2f>(window.getSize());
	scales[0] = size.x / 800;
	scales[1] = size.y / 450;
	window.setView(sf::View(sf::FloatRect(0.f, 0.f, size.x, size.y)));

	sidebarBackground.setSize(sf::Vector2f(800.f * scales[0], 22.f * scales[1]));
	
	divisionChanger.setScale(scales[0]/CHAR_SCALE, scales[1]/CHAR_SCALE);
	divisionChanger.setPosition(2*scales[0], 2*scales[1]);

	fretChanger.setScale(scales[0]/CHAR_SCALE, scales[1]/CHAR_SCALE);
	fretChanger.setPosition((20*scales[0])+divisionChanger.getGlobalBounds().width, 2*scales[1]);

	titleLabel.setScale(scales[0]/CHAR_SCALE, scales[1]/CHAR_SCALE);
	titleLabel.setPosition(window.getSize().x-(10*scales[0])-(titleLabel.getGlobalBounds().width), 2*scales[1]);

	for(int i = 0; i < 2; i++){
		timeSigLabels[i].setScale(scales[0]/3, scales[1]/3);
	}
}

void App::onClick(sf::Event event){
	mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

	if(event.mouseButton.button == sf::Mouse::Left){
		sf::FloatRect divisionTextBounds = divisionChanger.getGlobalBounds();
		sf::FloatRect fretTextBounds = fretChanger.getGlobalBounds();

		if(divisionTextBounds.contains(mousePos)){
			if(mousePos.x > divisionTextBounds.left + (divisionTextBounds.width/2)){
				changeDivision();
			}
			else{
				changeDivision(false);
			}
		}
		else if(fretTextBounds.contains(mousePos)){
			if(mousePos.x > fretTextBounds.left + (fretTextBounds.width/2)){
				changeFret();
			}
			else{
				changeFret(false);
			}
		}
		else if(titleLabel.getGlobalBounds().contains(mousePos)){
			title.clear();
			status = 1;
		}
		else if(timeSigLabels[0].getGlobalBounds().contains(mousePos)){
			status = 2;
		} 
		else if(timeSigLabels[1].getGlobalBounds().contains(mousePos)){
			status = 3;
		}
		else if(mousePos.y > 35*scales[1]){
			if(selectedIndex == -1){
				Note note;
				note.fret = hoveredNote.fret;
				note.division = hoveredNote.division;
				note.lineOffset = hoveredNote.lineOffset;
				note.lineIndex = hoveredNote.lineIndex;
				notes.push_back(note);
			}
			else if(selectedIndex >= 0 && hoveredNote.lineIndex == notes[selectedIndex].lineIndex){
				if(hoveredNote.lineOffset == notes[selectedIndex].lineOffset){
					notes[selectedIndex].fret = hoveredNote.fret;
					notes[selectedIndex].division = hoveredNote.division;

					auto range = towers.equal_range(selectedIndex);
					for(auto i = range.first; i != range.second; i++){
						i->second.division = hoveredNote.division;
					}
				}
				else{
					bool failedLinkCheck = true;
					
					auto range = towers.equal_range(selectedIndex);
					for(auto i = range.first; i != range.second; i++){
						if(hoveredNote.lineOffset == i->second.lineOffset){
							failedLinkCheck = false;
							i->second.fret = hoveredNote.fret;
							i->second.division = hoveredNote.division;
							for(auto j = range.first; j != range.second; j++){
								if(j != i){
									j->second.division = hoveredNote.division;
								}
							}
						}
						notes[selectedIndex].division = hoveredNote.division;
					}
					
					if(failedLinkCheck){
						Note note;
						note.fret = hoveredNote.fret;
						note.division = hoveredNote.division;
						note.lineOffset = hoveredNote.lineOffset;
						note.lineIndex = hoveredNote.lineIndex;
						towers.insert(std::pair<int, Note>(selectedIndex, note));
					}
				}
			}
		}
	} // endif mouse button is left click
	else if(event.mouseButton.button == sf::Mouse::Right){
		deleteNote();
	} // endif mouse button is right click
}

void App::changeDivision(bool up, bool loop){
	if(up){
		hoveredNote.division /= 2;
	}
	else{
		hoveredNote.division *= 2;
	}

	if(!loop){
		if(hoveredNote.division < 1){
			hoveredNote.division = 1;
		}
		else if(hoveredNote.division > 16){
			hoveredNote.division = 16;
		}
	}
	else{
		if(hoveredNote.division < 1){
			hoveredNote.division = 16;
		}
		else if(hoveredNote.division > 16){
			hoveredNote.division = 1;
		}
	}

	divisionChanger.setString("< 1/" + std::to_string(hoveredNote.division) + " >");
}

void App::changeFret(bool up, int specific){
	if(up){
		hoveredNote.fret++;
		if(hoveredNote.fret > 21){
			hoveredNote.fret = 21;
		}
	}
	else{
		hoveredNote.fret--;
		if(hoveredNote.fret < 0){
			hoveredNote.fret = 0;
		}
	}

	if(specific != -1){
		hoveredNote.fret = specific;
	}

	fretChanger.setString("< " + std::to_string(hoveredNote.fret) + " >");
}

void App::deleteNote(){
	if(selectedIndex < 0){
		return;
	}
	auto range = towers.equal_range(selectedIndex);

	// if hitting master node, move master node to first tower node
	if(hoveredNote.lineIndex == notes[selectedIndex].lineIndex && hoveredNote.lineOffset == notes[selectedIndex].lineOffset){
		bool foundTowerNode = false;
		for(auto i = range.first; i != range.second; i++){
			notes[selectedIndex] = i->second;
			foundTowerNode = true;
			towers.erase(i);
			break;
		}

		if(!foundTowerNode){
			notes.erase(notes.begin()+selectedIndex);
			// iterate through towers more than the selected index and shift them back one
			std::multimap<int, Note> temp;
			for(auto i = towers.begin(); i != towers.end();){
				Note val = i->second;
				int key = i->first;
				temp.insert({key > selectedIndex ? key - 1 : key, val});
				i = towers.erase(i);
			}

			towers = std::move(temp);
		}
	}
	else{
		for(auto i = range.first; i != range.second; i++){
			if(i->second.lineIndex == hoveredNote.lineIndex && i->second.lineOffset == hoveredNote.lineOffset){
				towers.erase(i);
				break;
			}
		}
	}
}

void App::onKeyPressed(sf::Event event){
	if(status == 1){
		if(event.key.code == sf::Keyboard::Backspace && title.length() > 0){
			title.pop_back();
		}
		else if(title.length() > 1 && (event.key.code == sf::Keyboard::Escape || event.key.code == sf::Keyboard::Enter)){
			status = 0;
		}
		if(title.length() < 20){
			if(event.key.code >= sf::Keyboard::A && event.key.code <= sf::Keyboard::Z){
				title.push_back(event.key.code + 97 - (event.key.shift ? 32: 0));
			}
			else if(event.key.code == sf::Keyboard::Space){
				title.push_back(' ');
			}
		}
		titleLabel.setString(title);
		titleLabel.setPosition(window.getSize().x-(10*scales[0])-(titleLabel.getGlobalBounds().width), 2*scales[1]);
	}
	else if(status == 2){
		if(event.key.code >= sf::Keyboard::Num1 && event.key.code <= sf::Keyboard::Num9){
			timeSig[0] = event.key.code - sf::Keyboard::Num0;
			timeSigLabels[0].setString(std::to_string(timeSig[0]));
			status = 0;
		}
	}
	else if(status == 3){
		if(event.key.code == sf::Keyboard::Num8 || event.key.code == sf::Keyboard::Num4 || event.key.code == sf::Keyboard::Num2){
			timeSig[1] = event.key.code - sf::Keyboard::Num0;
			timeSigLabels[1].setString(std::to_string(timeSig[1]));
			status = 0;
		}
	}
	else{
		if(event.key.code == sf::Keyboard::Tab){
			changeDivision(!event.key.shift, true);
		}
		else if(event.key.code == sf::Keyboard::Up){
			changeFret();
		}
		else if(event.key.code == sf::Keyboard::Down){
			changeFret(false);
		}
		else if(event.key.code == sf::Keyboard::Backspace){
			deleteNote();
		}
		else if(event.key.code == sf::Keyboard::D){
			// debug message
		}
	}
}
