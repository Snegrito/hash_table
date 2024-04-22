#include <iostream>
#include "user_interface.h" 

void exceptions() {

}

int main() {
	UserInterface a;
	a.openFile("input.txt");
	a.Menu();
}
