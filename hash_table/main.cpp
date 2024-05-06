#include <iostream>
#include "user_interface.h" 


int main() {
	UserInterface a;
	a.openFile("input.txt");
	a.Menu();

	ForwardList<int> b;
	b.insert_after(b.before_begin(), 1);
	b.clear();
}
