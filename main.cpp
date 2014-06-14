#include <iostream>
#include "board.h"
#include "board_display.h"
#include <utility>

extern int play_text();

int main() {
	board_display bdisp;
	char buf[10];
	int t;
	bdisp.init();
	bdisp.maingame();
	bdisp.loginscreen(buf);
	bdisp.bring_to_top(bdisp.game_panel);
	bdisp.gametype(t);
	bdisp.bring_to_top(bdisp.game_panel);

	if (t == EXIT_MENU_OPT) {
		bdisp.end();
		printf("Thanks for playing! %s\n",buf);
		exit(0);
	}

	if (t == TEXT_MODE_MENU_OPT) {
		bdisp.end();
		play_text();
		printf("Thanks for playing! %s\n",buf);
		exit(0);
	}
	return 0;
}

int play_text() {
	char a;
	board brd;
	vector<changed>o;
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
