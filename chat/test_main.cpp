#include <iostream>
#include <map>
#include <list>
#include <vector>
#include "databuf.h"

using namespace std;

int main() {
	grp_ctrl mctrl;
	grp_ctx grpid;
	vector<mem_ctx> mems;

	grpid = mctrl.create_group(0, 4, 5);

	for (int i = 0; i<5; i++) {
		mem_ctx m = mctrl.group_add_user(grpid, 0);
		if (m) {
			mems.push_back(m);
		}
	}

	char ibuf[10];
	int current = 0;

	for (int j = 0; j<1000; j++) {
	for (int i = 0; i< mems.size(); i++) {
		printf("Input %d\n", current);
		sprintf(ibuf, "%d", current);
		mctrl.update(grpid, mems[i], (void*)ibuf, strlen(ibuf)+1);
		current++;
	}
	}

	for (int i = 0 ; i < mems.size(); i++) {
		cout<<"Member "<< mems[i] << endl;
		while (1) {
			shared_ptr<data_buf> sp = mctrl.read(grpid, mems[i]);
			if (sp.get() == NULL) {
				break;
			} else {
				cout<<sp->buf<<endl;
			}
		}
	}	

	for (int i = 0; i<5; i++) {
		mctrl.group_del_user(grpid, mems[i]);
		cout<<"Deleted out "<<mems[i]<<endl;
	}


	return 0;
}
