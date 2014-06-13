#include <iostream>
#include <utility>
#include <stdlib.h>
#include <iomanip>
#include <vector>
#ifndef _BOARD_H_
#define _BOARD_H_
using namespace std;

class changed {
	public:
	int i;
	int j;
	int val;
	changed(int i, int j, int k):i(i),j(j),val(val) {
	}
};

class board {

	public:
	int data[4][4];	
	enum dir {
		UP,
		DOWN,
		RIGHT,
		LEFT
	};

	board() {
		for (int i =0; i<4; i++) {
			for (int j =0; j<4; j++) {
				data[i][j] = 0;
			}
		}
	}

	board(const board& inp) {
		for (int i =0; i<4; i++) {
			for (int j =0; j<4; j++) {
				data[i][j] = inp.data[i][j];
			}
		}
	}

	bool game_won() {
		for (int i = 0 ; i< 4; i++) {
			for (int j = 0; j< 4; j++) {
				if (data[i][j] == 2048) {
					return true;
				}
			}
		}
		return false;
	}

	bool game_lost() {
		int zeroes =0;
		int matched_neighbors = 0;
		for (int i = 0 ; i< 4; i++) {
			for (int j = 0; j< 4; j++) {
				if (data[i][j] == 0) {
					zeroes++;
				}
				if (i > 0) {
					if (data[i][j] == data[i-1][j]) {
						matched_neighbors++;
					}
				}
				if (j > 0) {
					if (data[i][j] == data[i][j-1]) {
						matched_neighbors++;
					}
				}

			}
		}
		if (zeroes || matched_neighbors) {
			return false;
		}
		
		return true;

	}
	void compress(vector<int>&o, vector<pair<int, int> >&chgd) {
again:
		for (int i = o.size()-1;i>0;i--) {
			if (o[i] == o[i-1] && o[i] != 0) {
				o[i]+=o[i];
				chgd.push_back(pair<int, int>(i,o[i]));
				int j = i-1;
				while(j>0) {
					o[j] = o[j-1];
					j--;
				}
				o[0] = 0;
				goto again;
			} else if (o[i] == 0) {
				int j = i;
				while(j>0) {
					o[j] = o[j-1];
					j--;
				}
				o[0] = 0;
				if (o[i] == 0) {
					int total = 0;
					for (int k =0; k< i; k++) {
						total+=o[k];
					}
					if (total) {
						goto again;
					}
				} else {
					goto again;
				}
			}
		}
	}

	bool move_right(vector<changed>&o) {
		int cp[4][4];
		bool shift = false;
		for (int i = 0; i<4;i++) {
			vector<int> d;
			vector<pair<int, int> > chgd;
			for (int j = 0; j<4; j++) {
				d.push_back(data[i][j]);
			}
			compress(d, chgd);
			for (int j = 0; j< 4; j++) {
				data[i][j] = d[j];
			}
			for (int j = 0; j < chgd.size(); j++) {
				o.push_back(changed(i,chgd[j].first,chgd[j].second));
			}
			shift |= (chgd.size() > 0);
		}
		return (!shift);
	}
	bool move_left(vector<changed>&o) {
		int cp[4][4];
		bool shift = false;
		for (int i = 0; i<4;i++) {
			vector<int> d;
			vector<pair<int,int> > chgd;
			for (int j = 3; j>=0; j--) {
				d.push_back(data[i][j]);
			}
			compress(d, chgd);
			for (int j = 0; j< 4; j++) {
				data[i][j] = d[3-j];
			}
			for (int j = 0; j < chgd.size(); j++) {
				o.push_back(changed(i,3-chgd[j].first, chgd[j].second));
			}
			shift |= (chgd.size() > 0);
		}
		return (!shift);
	}
	bool move_down(vector<changed>&o) {
		int cp[4][4];
		bool shift = false;
		for (int i = 0; i<4;i++) {
			vector<int> d;
			vector<pair<int, int> > chgd;
			for (int j = 0; j<4; j++) {
				d.push_back(data[j][i]);
			}
			compress(d, chgd);
			for (int j = 0; j< 4; j++) {
				data[j][i] = d[j];
			}
			for (int j = 0; j < chgd.size(); j++) {
				o.push_back(changed(chgd[j].first,i,chgd[j].second));
			}
			shift |= (chgd.size() > 0);
		}
		return (!shift);
	}
	bool move_up(vector<changed>&o) {
		int cp[4][4];
		bool shift = false;
		for (int i = 0; i<4;i++) {
			vector<int> d;
			vector<pair<int,int> > chgd;
			for (int j = 3; j>=0; j--) {
				d.push_back(data[j][i]);
			}
			compress(d, chgd);
			for (int j = 0; j< 4; j++) {
				data[j][i] = d[3-j];
			}
			for (int j = 0; j < chgd.size(); j++) {
				o.push_back(changed((3-chgd[j].first),i, chgd[j].second));
			}
			shift |= (chgd.size() > 0);
		}
		return (!shift);
	}

	void print() {
		for (int i = 0; i< 4; i++) {
			for (int j = 0; j< 4; j++) {
				cout<<setw(4)<<data[i][j]<<" ";
			}
			cout<<endl;
		}	
	}

	bool play_random(vector<changed>&o) {
		int choose;
		int chose;
		vector<pair <int, int> > l;
		for (int i = 0; i< 4; i++) {
			for (int j = 0; j< 4; j++) {
				if (data[i][j] == 0) {
					l.push_back(pair<int, int>(i,j));
				}
			}
		}	
		if (l.size()) {
			choose = (rand() %l.size());
			chose = 2 + (2*(rand()%2));
			data[l[choose].first][l[choose].second] = chose;
			o.push_back(changed(l[choose].first, l[choose].second, chose));
			return true;
		}
		return false;
	}
		
};
#endif
