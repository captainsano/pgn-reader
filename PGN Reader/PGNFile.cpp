//
//  PGNFile.cpp
//  PGN Reader
//
//  Created by Santhosbaala RS on 11/08/13.
//  Copyright (c) 2013 64cloud. All rights reserved.
//

#include "PGNFile.h"

#include <iostream>
#include "PGNException.h"

PGNFile::PGNFile(std::string fileName) {
	inputFileStream.open(fileName);
	if (!inputFileStream.good()) {
		throw std::invalid_argument("The file does not exist or is unreadable");
	}
	
	setGamePointers();
}

void PGNFile::setGamePointers() {
	// Function should go through the file and find where the game begins/ends, store it inside a tuple
	
	enum {
		ReadingMeta,
		ReadingTextCommentary,
		ReadingMoveText,
	} readingStatus = ReadingMeta;
	
	std::ifstream::pos_type currentGameBegin = 0, currentGameEnd = 0;
	
	while (inputFileStream.good()) {
		char ch = inputFileStream.get();
		if (ch == EOF) {
			// std::cout << "\n EOF begin: " << currentGameBegin << " " << currentGameEnd << std::endl;
			break;
		}
		
		// std::cout << ch; fflush(stdout);
		// Skip whitespaces
		if (ch == '\n' ||
			ch == '\r' ||
			ch == '\t' ||
			ch == '\v' ||
			ch == ' ' ||
			ch == '\0') {
			continue;
		}
		
		switch (readingStatus) {
			case ReadingMeta: {
				switch (ch) {
					case '[': {
						currentGameBegin = inputFileStream.tellg();
						readingStatus = ReadingMoveText;
	
						// Consume characters till a blank line is reached
						std::string line;
						while (inputFileStream.good()) {
							getline(inputFileStream, line);
							
							if (line.length() == 0 || line[0] == '\r' || line[0] == '\n') {
								break;
							}
						}
						
						break;
					}
						
					default: {
						// Invalid character encountered
						throw parse_error("Invalid character encountered while searching for meta data");
						break;
					}
				}
				
				break;
			}
				
			case ReadingMoveText: {
				switch (ch) {
					// Skip text commentary
					case '{': {
						readingStatus = ReadingTextCommentary;
						break;
					}
						
					default: {
						// Look for termination markers.
						switch (ch) {
							case '*': {
								currentGameEnd = inputFileStream.tellg();
								break;
							}
								
							// See if the next few characters read 1-0 or 1/2-1/2
							case '1': {
								if (inputFileStream.peek() == '-') {
									inputFileStream.get();	// For '-'
									char next2;
									if (inputFileStream.good()) {
										next2 = inputFileStream.get();
										if (next2 == '0') {
											currentGameEnd = inputFileStream.tellg();
										} else {
											for (int x = 0; x < 2; x++) { inputFileStream.unget(); }
											break;
										}
									}
								} else if (inputFileStream.peek() == '/') {
									inputFileStream.get();	// For '/'
									char next2, next3, next4, next5, next6;
									if (inputFileStream.good()) { next2 = inputFileStream.get(); }
									else {
										inputFileStream.unget();
										break;
									}
									if (inputFileStream.good()) { next3 = inputFileStream.get(); }
									else {
										for (int x = 0; x < 2; x++) { inputFileStream.unget(); }
										break;
									}
									if (inputFileStream.good()) { next4 = inputFileStream.get(); }
									else {
										for (int x = 0; x < 3; x++) { inputFileStream.unget(); }
										break;
									}
									if (inputFileStream.good()) { next5 = inputFileStream.get(); }
									else {
										for (int x = 0; x < 4; x++) { inputFileStream.unget(); }
										break;
									}
									if (inputFileStream.good()) { next6 = inputFileStream.get(); }
									else {
										for (int x = 0; x < 5; x++) { inputFileStream.unget(); }
										break;
									}
									
									if (next2 == '2' &&
										next3 == '-' &&
										next4 == '1' &&
										next5 == '/' &&
										next6 == '2') {
										currentGameEnd = inputFileStream.tellg();
									} else {
										for (int x = 0; x < 6; x++) { inputFileStream.unget(); }
										break;
									}
								}
								
								break;
							}
								
							// See if the next few characters read 0-1
							case '0': {
								if (inputFileStream.peek() == '-') {
									inputFileStream.get();	// For '-'
									char next2;
									if (inputFileStream.good()) {
										next2 = inputFileStream.get();
										if (next2 == '1') {
											currentGameEnd = inputFileStream.tellg();
										} else {
											inputFileStream.unget();
											inputFileStream.unget();
										}
									}
								}
								
								break;
							}
						}
						
						// Push the current <begin, end> tuple
						if (currentGameEnd != 0) {
							gamePointers.push_back(std::make_tuple(currentGameBegin - static_cast<std::ios::pos_type>(1), currentGameEnd));
							currentGameBegin = 0;
							currentGameEnd = 0;
							readingStatus = ReadingMeta;
						}
					}
				}
				
				break;
			}
				
			case ReadingTextCommentary: {
				if (ch == '}') {
					readingStatus = ReadingMoveText;
				}
				break;
			}
		}
	}
	
	// Check the pointers for begin and end were properly set - Invalid state
	if (currentGameBegin != 0 && currentGameEnd == 0) {
		// std::cout << "\n" << currentGameBegin << " " << std::endl;
		throw parse_error("The game termination string was not found for a game ");
	}
}

unsigned int PGNFile::getGameCount() {
	return (unsigned int)gamePointers.size();
}

std::shared_ptr<PGNGame> PGNFile::getGame(unsigned int i) {
	if (i > gamePointers.size()) {
		throw std::out_of_range("The index i is not in range");
	}
	
	std::string gameString;
	inputFileStream.clear();
	inputFileStream.seekg(std::get<0>(gamePointers[i]));
	
	while (inputFileStream.good() && inputFileStream.tellg() != std::get<1>(gamePointers[i])) {
		gameString += inputFileStream.get();
	}
	
	std::shared_ptr<PGNGame> toReturn = std::make_shared<PGNGame>(gameString);
	return toReturn;
}