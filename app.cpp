#include "app.hpp"
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

	division = timeSig[1];

	divisionChanger.setFont(font);
	divisionChanger.setCharacterSize(CHAR_SIZE);
	divisionChanger.setString("< 1/" + std::to_string(division) + " >");
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
		timeSigLabels[i].setString(std::to_string(timeSig[i]));
		timeSigLabels[i].setFillColor(sf::Color::Black);
	}

	cursor.setFillColor(sf::Color(200, 200, 200));
	cursor.setRadius(5.0f);
}

void App::run(){
	sf::Clock clock;

	sf::Event event;
	while(status >= 0){
		clock.restart();
		while(window.pollEvent(event)){
			switch(event.type){
				case sf::Event::Closed: quit(); break;
				case sf::Event::Resized: resizeCallback(); break;
				case sf::Event::MouseButtonPressed: onClick(event); break;
				case sf::Event::MouseMoved: mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window)); break;
				case sf::Event::KeyPressed: keyboardCallback(event); break;
				default: break;
			}
		}

		window.clear(sf::Color::White);

		// ui
		window.draw(sidebarBackground);
		window.draw(divisionChanger);
		window.draw(fretChanger);
		window.draw(titleLabel);

		bool drawCurs = true;
		sf::Vector2f cursorPos;
		int count = 0;
		int stanzaStart = 0;
		int stanza = -1;
		for(int i = 40*scales[1]; i+((5-count)*scales[1]) < 440*scales[1]; i += ((440*scales[1])-(40*scales[1]))/40){
			if(count == 0){
				stanzaStart = i;
				stanza++;
			}
			else if(count == 5){
				count++;
				continue;
			}
			else if(count == 6){
				count = 0;
				continue;
			}
			if(count == 1 && i - ((440*scales[1])-(40*scales[1]))/40 <= 40*scales[1]){
				timeSigLabels[0].setPosition(15*scales[0], i - ((440*scales[1])-(40*scales[1]))/80);
				window.draw(timeSigLabels[0]);
			}
			else if(count == 2 && i - ((440*scales[1])-(40*scales[1]))/20 <= 40*scales[1]){
				timeSigLabels[1].setPosition(15*scales[0], i - ((440*scales[1])-(40*scales[1]))/80);
				window.draw(timeSigLabels[1]);
			}

			sf::RectangleShape r(sf::Vector2f(window.getSize().x - (20*scales[1]), scales[1]));
			r.setPosition(10*scales[0], i);
			r.setFillColor(sf::Color::Black);
			if(std::abs(mousePos.y - i) < 10*scales[1]){
				cursorPos.y = i - cursor.getRadius();
				selected[0] = i / scales[1];
				selected[2] = stanzaStart / scales[1];
				selected[3] = stanza;
			}

			window.draw(r);
			count++;
		}

		float barWidth = window.getSize().x - (20*scales[1]);
		int lowest = 99;
		for(float i = 1; i < 24.f; i += (float)timeSig[1]/(float)division){
			int dist = std::abs(((barWidth*i)/24)-mousePos.x);
			if(dist < lowest){
				if(timeSig[1]/lastBeatDiv(i).div == 2 && i - lastBeatDiv(i).beat < 2){
					continue;
				}
				else if(timeSig[1]/lastBeatDiv(i).div == 4 && i - lastBeatDiv(i).beat < 4){
					continue; 
				} 
				else if(timeSig[1]/lastBeatDiv(i).div == 8 && i - lastBeatDiv(i).beat < 8){
					continue;
				}
				else if(timeSig[1]/lastBeatDiv(i).div == 16 && i - lastBeatDiv(i).beat < 16){
					continue;
				}
				lowest = dist;
				cursorPos.x = (barWidth*i)/24;
				selected[1] = i;
			}
		}
//			for(float i = 0.f; i < 32.f; i += (float)timeSig[1]/(float)division){
//				int dist = std::abs(((barWidth*i)/24)-mousePos.x);
//				if(dist < lowest){
//					lowest = dist;
//					cursorPos.x = (barWidth*i)/24;
//					selected[1] = i;
//				}
//			}

		sf::Text noteText;
		noteText.setCharacterSize(CHAR_SIZE);
		noteText.setScale(scales[0]/4, scales[1]/4);
		noteText.setFont(font);
		noteText.setFillColor(sf::Color::Black);
		bool failed = true;
		for(int i = 0; i < notes.size(); i++){
			if(selected[1] == notes[i].beat && selected[0] == notes[i].line){
				hoveringNote = i;
				failed = false;
				drawCurs = false;
			}
			noteText.setString(std::to_string(notes[i].fret));
			if(notes[i].isOver){
				noteText.setFillColor(sf::Color::Red);
			}
			else{
				noteText.setFillColor(sf::Color::Black);
			}

			noteText.setPosition((notes[i].beat*barWidth)/24, (notes[i].line*scales[1]) - noteText.getGlobalBounds().height);
			window.draw(noteText);
		}
		if(failed){
			hoveringNote = -1;
		}

		// measure line arteeste
		for(int j = 0; j < stanza; j++){
			// goes by 1/16ths to account for a x/1 time sig with 1/16th notes
			float beatsPassed = 0.f;
			for(float i = 0; i < 24; i += 0.0625){
				if(divAtBeat(i, j).div != -1){
					beatsPassed += (float)timeSig[1]/(float)divAtBeat(i, j).div;
				}
				if(beatsPassed >= timeSig[0]){
					if(beatsPassed > timeSig[0]){
						notes[divAtBeat(i).index].isOver = true;
					} 
					sf::RectangleShape rect(sf::Vector2f(scales[0], 40*scales[1]));
					rect.setFillColor(sf::Color::Black);
					if(timeSig[1]/divAtBeat(i).div <= 1){
						rect.setPosition((((i)*barWidth)/24)+(10*scales[0]), divAtBeat(i, j).stanzaStart*scales[1]);
					}
					else if(timeSig[1]/divAtBeat(i).div == 2){
						rect.setPosition((((i+1)*barWidth)/24)+(10*scales[0]), divAtBeat(i, j).stanzaStart*scales[1]);
					}
					else if(timeSig[1]/divAtBeat(i).div == 4){
						rect.setPosition((((i+3)*barWidth)/24)+(10*scales[0]), divAtBeat(i, j).stanzaStart*scales[1]);
					}
					else if(timeSig[1]/divAtBeat(i).div == 8){
						rect.setPosition((((i+7)*barWidth)/24)+(10*scales[0]), divAtBeat(i, j).stanzaStart*scales[1]);
					}
					else if(timeSig[1]/divAtBeat(i).div == 16){
						rect.setPosition((((i+15)*barWidth)/24)+(10*scales[0]), divAtBeat(i, j).stanzaStart*scales[1]);
					}
					window.draw(rect);
					beatsPassed = 0.f;
				}
			}
		}

		if(drawCurs && mousePos.y > 35*scales[1] && (mousePos.x > 10*scales[1] && mousePos.x < window.getSize().x - (20*scales[1]))){
			cursor.setPosition(cursorPos);
			window.draw(cursor);
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

void App::resizeCallback(){
	sf::Vector2f size = static_cast<sf::Vector2f>(window.getSize());
	scales[0] = size.x / 800;
	scales[1] = size.y / 450;
	window.setView(sf::View(sf::FloatRect(0.f, 0.f, size.x, size.y)));

	sidebarBackground.setSize(sf::Vector2f(800.f * scales[0], 22.f * scales[1]));
	
	divisionChanger.setScale(scales[0]/4, scales[1]/4);
	divisionChanger.setPosition(2*scales[0], 2*scales[1]);

	fretChanger.setScale(scales[0]/4, scales[1]/4);
	fretChanger.setPosition((20*scales[0])+divisionChanger.getGlobalBounds().width, 2*scales[1]);

	titleLabel.setScale(scales[0]/4, scales[1]/4);
	titleLabel.setPosition(window.getSize().x-(10*scales[0])-(titleLabel.getGlobalBounds().width), 2*scales[1]);

	for(int i = 0; i < 2; i++){
		timeSigLabels[i].setScale(scales[0]/4, scales[1]/4);
	}

	cursor.setRadius(5.f * scales[1]);
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
		else if(mousePos.y > 35*scales[1] && (mousePos.x > 10*scales[1] && mousePos.x < window.getSize().x - (20*scales[1]))){
			if(hoveringNote >= 0){
				notes[hoveringNote].fret = fret;
				bool failed = false;
				for(int i = 0; i < notes.size(); i++){
					if(i == hoveringNote){
						continue;
					}
					if(notes[i].beat == selected[1]){
						if(division != notes[i].division){
							failed = true;
						}
					}
				}
				if(!failed){
					notes[hoveringNote].division = division;
				}
			}
			else{
				Note note;
				note.fret = fret;
				note.division = division;
				note.line = selected[0];
				note.beat = selected[1];
				note.stanzaStart = selected[2];
				note.stanza = selected[3];
				// i know linear search is slower than my mile time
				// but like fuck you i dont wanna do this efficiently
				bool failed = false;
				for(int i = 0; i < notes.size(); i++){
					if(notes[i].beat == note.beat){
						if(note.division != notes[i].division){
							// difference is a sin, notes are totally racist
							failed = true;
						}
					}
				}
				if(!failed){
					notes.push_back(note);
				}
			}
		}
	}

	if(hoveringNote != -1 && event.mouseButton.button == sf::Mouse::Right){
		notes.erase(notes.begin()+hoveringNote);
	}
}

void App::changeDivision(bool up, bool loop){
	if(up){
		division /= 2;
	}
	else{
		division *= 2;
	}

	if(!loop){
		if(division < 1){
			division = 1;
		}
		else if(division > 16){
			division = 16;
		}
	}
	else{
		if(division < 1){
			division = 16;
		}
		else if(division > 16){
			division = 1;
		}
	}

	divisionChanger.setString("< 1/" + std::to_string(division) + " >");
}

void App::changeFret(bool up, int specific){
	if(up){
		fret++;
		if(fret > 21){
			fret = 21;
		}
	}
	else{
		fret--;
		if(fret < 0){
			fret = 0;
		}
	}

	if(specific != -1){
		fret = specific;
	}

	fretChanger.setString("< " + std::to_string(fret) + " >");
}

void App::keyboardCallback(sf::Event event){
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
			status = 0;
		}
	}
	else if(status == 3){
		if(event.key.code == sf::Keyboard::Num8 || event.key.code == sf::Keyboard::Num4 || event.key.code == sf::Keyboard::Num2){
			timeSig[1] = event.key.code - sf::Keyboard::Num0;
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
			if(hoveringNote != -1){
				notes.erase(notes.begin()+hoveringNote);
			}
		}
		else if(event.key.code == sf::Keyboard::D){
			// leave this at the bottom of if tower, so delete easier later
			std::cout << "DEBUG OUTPUT\n"
				<< "Notes: {\n";
			for(int i = 0; i < notes.size(); i++){
				std::cout << "Div: " << notes[i].division << "\n"
					<< "Fret: " << notes[i].fret << "\n"
					<< "Beat: " << notes[i].beat << "\n"
					<< "Line: " << notes[i].line << "\n"
					<< "Index: " << i << "\n"
					<< "------------\n";
			}
			std::cout << "Selected: {" << selected[0] << ", " << selected[1] << "}\n"
				<< "Hovered Note: " << hoveringNote << "\n"
				<< std::endl;
		}
	}
}

// prayers and so much duct tape
Data App::lastBeatDiv(float beat){
	Data data;
	data.div = -1;
	// again linear search slow, my brain is the same
	int lowest = 99;
	for(int i = 0; i < notes.size(); i++){
		// can't be in the future so must be above 0 to be behind
		int dist = beat - notes[i].beat;
		if(dist > 0 && dist < lowest){
			lowest = dist;
			data.beat = notes[i].beat;
			data.div = notes[i].division;
		}
	}
	return data;
}

Data App::divAtBeat(float beat, int stanza){
	Data data;
	data.div = -1;
	for(int i = 0; i < notes.size(); i++){
		if(notes[i].beat == beat && notes[i].stanza == stanza){
			data.div = notes[i].division;
			data.index = i;
			data.stanzaStart = notes[i].stanzaStart;
			data.stanza = notes[i].stanza;
		}
	}
	return data;
}
