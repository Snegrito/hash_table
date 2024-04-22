#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include "dictionary_map.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class UserInterface {
public:
	UserInterface() = default;
	UserInterface(const UserInterface& copy) = delete;
	UserInterface(UserInterface&& move) = delete;
	UserInterface& operator=(const UserInterface& copy) = delete;
	UserInterface& operator=(UserInterface&& move) noexcept = delete;

	~UserInterface() {
		if (fin.is_open()) {
			fin.close();
		}
	}

	bool openFile(const std::string& filename);
	
	void insertTextFromFile();
	void insertStringFromConsole();
	void insertWord(const std::string& word);

	void eraseWord();

	void showDictionary(std::ostream& out);
	void showTopTree(std::ostream& out);

	size_t ditionarySize();
	void clearDictionary();
	
	void showMenu();
	void Menu();
private:
	DictionaryMap<std::string> dict;
	std::ifstream fin;
};

inline bool UserInterface::openFile(const std::string& filename) {
	fin.open(filename);
	if (!fin.is_open()) {
		return false;
	}
	return true;
}

inline void UserInterface::insertTextFromFile() {
	if (!fin.is_open()) {
		return;
	}
	std::string inserter;
	while (!fin.eof()) {
		std::getline(fin, inserter, '\n');
		std::istringstream tempStream(inserter);
		std::string substr;
		while (tempStream >> substr) {
			dict.insert(substr);
		}
	}
}

inline void UserInterface::insertStringFromConsole() {
	if (!fin.is_open()) {
		return;
	}
	std::string inserter;
	std::getline(std::cin, inserter, '\n');
	std::getline(std::cin, inserter, '\n');
	std::istringstream tempStream(inserter);
	std::string substr;
	while (tempStream >> substr) {
		dict.insert(substr);
	}
}

inline void UserInterface::insertWord(const std::string& word) {
	if (!fin.is_open()) {
		return;
	}
	dict.insert(word);
}

inline void UserInterface::showDictionary(std::ostream& out) {
	if (!fin.is_open()) {
		return;
	}
	dict.print(out);
}

inline void UserInterface::showTopTree(std::ostream& out) {
	if (!fin.is_open()) {
		return;
	}
	auto* arr = new std::pair<std::string, size_t>[3];
	try {
		dict.popularWords(arr);
		if (dict.size() < 4) {
			for (size_t i = 0; i < dict.size(); ++i) {
				out << '(' << arr[i].first << " : " << arr[i].second << ") ";
			}
			out << '\n';
		}
		else {
			for (size_t i = 0; i < 3; ++i) {
				out << '(' << arr[i].first << " : " << arr[i].second << ") ";
			}
			out << '\n';
		}
	}
	catch (...) {
		delete[] arr;
	}
	delete[] arr;
}

inline size_t UserInterface::ditionarySize() {
	return dict.size();
}

inline void UserInterface::eraseWord() {
	std::string a;
	std::cin >> a;
	if (!dict.erase(a)) {
		std::cout << "Word is missing!\n";
	}
}

inline void UserInterface::clearDictionary() {
	dict.clear();
}

inline void UserInterface::showMenu() {
	std::cout << "Input number you want:\n";
	std::cout << "1) Read text from the file\n";
	std::cout << "2) Read string from the console\n";
	std::cout << "3) Delete word from dictionary\n";
	std::cout << "4) Show all dictionary\n";
	std::cout << "5) Show top three word\n";
	std::cout << "6) Show dictionary size\n";
	std::cout << "7) Clear dictionary\n";
	std::cout << "8) Show menu\n";
	std::cout << "9) Stop\n\n";
}

inline void UserInterface::Menu() {
	if (!fin.is_open()) {
		std::cout << "file is not open!\n";
	}
	showMenu();
	while (true) {
		char command;
		std::cin >> command;
		if (command == '1') {
			insertTextFromFile();
		}
		else if (command == '2') {
			insertStringFromConsole();
		}
		else if (command == '3') {
			eraseWord();
		}
		else if (command == '4') {
			showDictionary(std::cout);
		}
		else if (command == '5') {
			showTopTree(std::cout);
		}
		else if (command == '6') {
			std::cout << ditionarySize() << '\n';
		}
		else if (command == '7') {
			clearDictionary();
		}
		else if (command == '8') {
			showMenu();
		}
		else if (command == '9') {
			break;
		}
		else {
			std::cout << "Wrond command!\n\n";
			showMenu();
		}
	}
}

#endif
