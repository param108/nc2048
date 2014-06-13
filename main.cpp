#include <iostream>
#include "board.h"
#include "board_display.h"
#include <utility>

int main() {
	char a;
	board brd;
	vector<changed>o;
	board_display bdisp;
	brd.play_random(o);
	brd.print();
	cout<<endl;
	do {
	cin>>a;
	switch(a) {
		case 'u':
			brd.move_up(o);
			cout<<"UP"<<endl;
			break;
		case 'd':
			brd.move_down(o);
			cout<<"DOWN"<<endl;
			break;
		case 'r':
			brd.move_right(o);
			cout<<"RIGHT"<<endl;
			break;
		case 'l':
			brd.move_left(o);
			cout<<"LEFT"<<endl;
			break;
	}

	if (brd.game_won()) {
		cout<<"You Won!"<<endl;
		brd.print();
		break;
	}

	if (brd.game_lost()) {
		cout<<"You Lost!"<<endl;
		brd.print();
		break;
	}
	if (!brd.play_random(o)) {
		cout<<"You Lost!"<<endl;
		brd.print();
		break;
	}
	brd.print();
	o.clear();
	} while (a != 'q');
	return 0;
}
