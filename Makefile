2048:	main.cpp board.h board_display.h
	g++ main.cpp -lform -lmenu -lpanel -lncurses -o 2048

clean:
	- rm 2048
