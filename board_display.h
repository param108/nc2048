#include <curses.h>
#include <form.h>
#include <panel.h>
#include <menu.h>
#include <string.h>
#include <list>
#include <map>
#include "board.h"
#ifndef __BOARD_DISPLAY_H__
#define __BOARD_DISPLAY_H__
using namespace std;

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define SINGLE_PLAYER_MENU_OPT 0
#define MULTI_PLAYER_MENU_OPT 1
#define EXIT_MENU_OPT 2

class board_display {
public:
	WINDOW *hiscore_panel, *main_panel, *game_panel;
	WINDOW *dlg_screen, *dlg_form;
	map<WINDOW*, PANEL*> window_map;
	board brd;	

	board_display() {
	}

	void reset() {
		brd.clear();
		render_board();
	}

	void end() {
		del_panel(window_map[game_panel]);
		del_panel(window_map[dlg_screen]);
		delwin(hiscore_panel);
		delwin(main_panel);
		delwin(game_panel);
		delwin(dlg_screen);
		delwin(dlg_form);
		endwin();
	}

	void init() {
		initscr();
		cbreak();
		noecho();
		raw();
	}

	void scale_board(int &h, int &l) {
		int padx = 1;
		int pady = 1;
		int numx = 4;
		int numy = 1;
		int vertline = 1;
		int horline = 1;
	
		l = (vertline + padx + numx + padx)*4 + vertline;
		h = (horline + pady + numy + pady)*4 + horline;
	}

	void render_board() {
		int h, w, brdh, brdw;
  	getmaxyx(main_panel, h, w);
		scale_board(brdh, brdw);
		
		int startx = (w - brdw)/2;
		int starty = (h - brdh)/2;
		int iterx, itery;

		for (int i = 0; i < brdw; i++) {
			for (int j = 0; j < brdh; j++) {
				iterx = startx + i; 
				itery = starty + j; 
				if (i == 0) {
					if (j == 0) {
						mvwaddch(main_panel, itery, iterx, ACS_ULCORNER);	
					} else if (j == (brdh-1)) {
						mvwaddch(main_panel, itery, iterx, ACS_LLCORNER);	
					} else if (j%4 == 0) {
						mvwaddch(main_panel, itery, iterx, ACS_LTEE);
					} else {
						mvwaddch(main_panel, itery, iterx, ACS_VLINE);
					}
				} else if (i == (brdw-1)) {
					if (j == 0) {
						mvwaddch(main_panel, itery, iterx, ACS_URCORNER);	
					} else if (j == (brdh-1)) {
						mvwaddch(main_panel, itery, iterx, ACS_LRCORNER);	
					} else if (j%4 == 0) {
						mvwaddch(main_panel, itery, iterx, ACS_RTEE);
					} else {
						mvwaddch(main_panel, itery, iterx, ACS_VLINE);
					}
				} else if (i%7 == 0) {
					if (j == 0) {
						mvwaddch(main_panel, itery, iterx, ACS_TTEE);	
					} else if (j == (brdh-1)) {
						mvwaddch(main_panel, itery, iterx, ACS_BTEE);	
					} else if (j%4 == 0) {
						mvwaddch(main_panel, itery, iterx, ACS_PLUS);
					} else {
						mvwaddch(main_panel, itery, iterx, ACS_VLINE);
					}
				} else {
					if (j == 0) {
						mvwaddch(main_panel, itery, iterx, ACS_HLINE);	
					} else if (j == (brdh-1)) {
						mvwaddch(main_panel, itery, iterx, ACS_HLINE);	
					} else if (j%4 == 0) {
						mvwaddch(main_panel, itery, iterx, ACS_HLINE);
					} else {
					}
				}
			}
		}

		for (int i = 0; i<4; i++) {
			for (int j = 0; j<4; j++) {
				char num[5];
				int val = brd.data[i][j];
				int y = 2 + i*4 + starty;
				int x = 2 + j*7 + startx;
				sprintf(num,"%d",val);
				int k;
				for (k = 0; k<((4 - strlen(num))/2);k++) {
					mvwaddch(main_panel,y, x, ' ');
					x++;
				}	
				mvwaddstr(main_panel, y, x, num);
				k += strlen(num);	
				x += strlen(num);	
				while(k<4) {
					mvwaddch(main_panel,y, x, ' ');
					k++;
				}
			}
		}
	}

	void maingame() {
  	int h, w;
  	getmaxyx(stdscr, h, w);

		game_panel = newwin(h,w,0,0);
		hiscore_panel = subwin(game_panel,h, 8,0,0);
		main_panel = subwin(game_panel,h, w-8,0,8);

		render_board();

		mvwaddstr(main_panel,1,1,"Use arrow keys to play");
		mvwaddstr(main_panel,2,1,"q to quit");
		dlg_screen = newwin(h/2,w,h/4,0);
    dlg_form = derwin(dlg_screen, h/2-8, w-4, 4, 2);

		wborder(hiscore_panel, 0, 0, 0, 0, 0, 0, 0, 0);
		wborder(main_panel, 0, 0, 0, 0, 0, 0, 0, 0);

		werase(dlg_form);
		werase(dlg_screen);
		box(dlg_screen,0,0);
		box(dlg_form,0,0);

		window_map[game_panel] = new_panel(game_panel);
		window_map[dlg_screen] = new_panel(dlg_screen);

		update_panels();
		doupdate();
	}

	void do_highlights(vector<changed> &o) {
		/* highlights */
		for (int hindex = 0; hindex < o.size(); hindex++) {
			if (brd.data[o[hindex].i][o[hindex].j] == o[hindex].val) {
				//highlight(o[hindex].i,o[hindex].j);
			}
		}
	}

	void singleplayer_cmd_loop(fstream &f) {
		bring_to_top(game_panel);
		vector<changed>o;
		int c;

		brd.play_random(o);
		render_board();
		wrefresh(main_panel);
		if (f.is_open()) {
			brd.print(f);
		}
		while((c = wgetch(game_panel)) != 'q')
		{ switch(c)
	    {	
				case KEY_DOWN:
					brd.move_down(o);
				break;
				case KEY_UP:
					brd.move_up(o);
				break;
				case KEY_RIGHT:
					brd.move_right(o);
				break;
				case KEY_LEFT:
					brd.move_left(o);
				break;
				default:
					continue;
			}
			if (f.is_open()) {
				brd.print(f);
			}

			do_highlights(o);

			o.clear();
			if (!brd.play_random(o)) {
				break;
			}

			render_board();
			if (f.is_open()) {
				brd.print(f);
			}
			o.clear();
			wrefresh(main_panel);

			if (brd.game_won()) {
				break;
			}
			if (brd.game_lost()) {
				break;
			}
		}	
	}

	void bring_to_top(WINDOW* w) {
		top_panel(window_map[w]);
		keypad(w,TRUE);
		update_panels();
		doupdate();
	}

	void reset_dlg() {
		werase(dlg_form);
		box(dlg_form,0,0);
	}


	void gametype(int &t) {
		int n_choices = 0;
		int c = 0;
		MENU* my_menu;
		const char *choices[] = {
     "Single Player",
     "Multi Player",
     "Exit",
   	};
	
		n_choices = ARRAY_SIZE(choices);
	
		ITEM** my_items = new(ITEM*[n_choices+ 1]);
		my_items[n_choices] = NULL;
		for (int i = 0; i< n_choices;i++) {
			my_items[i] = new_item(choices[i],"");
		}

		my_menu = new_menu(my_items);

		int h, w;
  	getmaxyx(dlg_form, h, w);

		int mh, ml;
		set_menu_mark(my_menu, " * ");
		scale_menu(my_menu, &mh, &ml);
		WINDOW *dlg_menu  = derwin(dlg_form, mh, ml,(h-mh)/2,(w - ml)/2); 
		set_menu_win(my_menu, dlg_form);
		set_menu_sub(my_menu, dlg_menu);


		mvwprintw(dlg_form,h-2,2,"<Enter> to choose"); 

		/* Post the menu */
		post_menu(my_menu);
		wrefresh(dlg_screen);

    bring_to_top(dlg_screen);

		keypad(dlg_screen, TRUE);

		while((c = wgetch(dlg_screen)) != '\n')
		{ switch(c)
	    {	case KEY_DOWN:
				menu_driver(my_menu, REQ_DOWN_ITEM);
				break;
				case KEY_UP:
				menu_driver(my_menu, REQ_UP_ITEM);
				break;
			}
      wrefresh(dlg_screen);
		}	

		ITEM *iter = current_item(my_menu);

		for (int i = 0; i< n_choices; i++) {
			if (my_items[i] == iter) {
				t = i;
			}
		}

	/* Unpost and free all the memory taken up */
    unpost_menu(my_menu);
    free_menu(my_menu);
    for(int i = 0; i < n_choices; ++i) {
        free_item(my_items[i]);
		}
		delwin(dlg_menu);
		refresh();
		reset_dlg();

		delete my_items;
	}

	void loginscreen(char *buf) {
		FIELD* fld[2];
		FORM* login;
		int ch;		
  	int h, w;

  	getmaxyx(stdscr, h, w);

		int len = strlen("Name:") + 1 + 5;
		int startx = (((w - 4) - len)/2)+5;
		fld[0]= new_field(1,6,(h/4)-4,startx,0, 0);
    set_field_back(fld[0], A_UNDERLINE);
		set_field_type(fld[0],TYPE_ALNUM,5);
		fld[1]= NULL;

		login = new_form(fld);

    set_form_win(login, dlg_screen);
    set_form_sub(login, dlg_form);

		keypad(dlg_form,TRUE);
	 	post_form(login);
		mvwaddstr(dlg_form,(h/4) - 4, startx - 5,"Name:");
		refresh();
		bring_to_top(dlg_screen);
		/* Loop through to get user requests */
		while((ch = wgetch(dlg_form)) != '\n')
		{				 
			switch(ch) {
			case KEY_BACKSPACE:
				form_driver(login,REQ_DEL_PREV);
				break;
			default:
    	  form_driver(login, REQ_VALIDATION);

				char *b;
				int len = 0;
				b=field_buffer(fld[0],0);
				for (int i = 0; i<6;i++) {
					if (b[i] != ' ') {
						len++;
					}
				}
				if (len < 5) {
				  form_driver(login, ch);
				}
				break;
			}
		}
    form_driver(login, REQ_VALIDATION);
		strcpy(buf,field_buffer(fld[0],0));
		/* Un post form and free the memory */
		unpost_form(login);
		free_form(login);
		free_field(fld[0]);
		refresh();
		reset_dlg();
	}

};
#endif
