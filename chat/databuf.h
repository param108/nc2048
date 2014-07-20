#include <iostream>
#include <memory>
#include <map>
#include <list>
#include <string.h>

using namespace std; 

typedef unsigned int uint32;
typedef void * grp_ctx;
typedef void * mem_ctx;

class data_buf {
	public:
	mem_ctx from;
	char buf[256];
	int len;
	~data_buf() {
#ifdef DEBUG_DELETE
		cout<<"Deleting data:"<<buf<<endl;
#endif
	}
};

class mem_data {
	public:
	mem_ctx me;
	list<shared_ptr<data_buf> > data;

	mem_data() {
	}

	~mem_data() {
#ifdef DEBUG_DELETE
		cout<<"Deleting member "<< me << endl;
#endif
		data.clear();
	}
};

class grp_data {
	public:
	map<mem_ctx, shared_ptr<mem_data> > mems;	
	int maxusers;
	int timeout;

	grp_data() {
	}
	
	~grp_data() {
		/* should call all destructors */
		mems.clear();
	}
};

class grp_ctrl
{
	private:
	map<grp_ctx, shared_ptr<grp_data> > grps;	

	void distribute_data(grp_ctx hdl, mem_ctx mem, data_buf *dbf) {
		shared_ptr<data_buf> sp(dbf);
		shared_ptr<grp_data> grp = grps[hdl];
		map<mem_ctx, shared_ptr<mem_data> >::iterator iter= grp->mems.begin();

		while (iter != grp->mems.end()) {
			iter->second->data.push_back(sp);
			iter++;
		}

	}

	public:

	grp_ctrl() {
	}

	~grp_ctrl() {
#ifdef DEBUG_DELETE
		cout<<"Deleting grp"<<endl;
#endif
		grps.clear();
	}

	shared_ptr<data_buf> read(grp_ctx hdl, mem_ctx mem)
	{
		if (grps.find(hdl) == grps.end()) {
			cerr<<__FUNCTION__<<": failed to find grp "<< hdl<< endl;
			return(shared_ptr<data_buf>(NULL));
		}

		shared_ptr<grp_data> grp = grps[hdl];

		if (grp->mems.find(mem) == grp->mems.end()) {
			cerr<<__FUNCTION__<<": failed to find member "<< mem<< endl;
			return(shared_ptr<data_buf>(NULL));
		}

		if (grp->mems[mem]->data.size() > 0) {
			shared_ptr<data_buf> sp = grp->mems[mem]->data.front();
			grp->mems[mem]->data.pop_front();
			return sp;
		}

		return (shared_ptr<data_buf>(NULL));
	}

	void update(grp_ctx hdl, mem_ctx mem, void *buf, int len) 
	{
		if (grps.find(hdl) == grps.end()) {
			cerr<<__FUNCTION__<<": failed to find grp "<< hdl<< endl;
			return;
		}

		shared_ptr<grp_data> grp = grps[hdl];

		if (grp->mems.find(mem) == grp->mems.end()) {
			cerr<<__FUNCTION__<<": failed to find member "<< mem<< endl;
			return;
		}


		while (len > 0) {
			data_buf *dbf = new data_buf();
			dbf->from = mem;

			if (len <=256) {
				dbf->len = len;
				len = 0;
			} else {
				dbf->len = 256;
				len = len - 256;
			}

			memcpy(dbf->buf, buf, dbf->len);

			distribute_data(grp.get(), mem, dbf);
		}
	}

	grp_ctx create_group(uint32 flags, uint32 maxusers, uint32 timeout)
	{
		grp_data * gp = new grp_data();
		gp->timeout = timeout;
		gp->maxusers = maxusers;
		grps[(grp_ctx)gp] = shared_ptr<grp_data>(gp);
		return (grp_ctx) gp;
	}


	mem_ctx group_add_user(grp_ctx hdl, uint32 flags)
	{
		if (grps.find(hdl) == grps.end()) {
			cerr<<"Failed to find hdl "<<hdl<<endl;
			return (void*)NULL;
		}

		shared_ptr<grp_data> grp = grps[hdl];
		if (grp->mems.size() == grp->maxusers) {
			cerr<<"Too many users for group"<<hdl<<endl;
			return (void*)NULL;
		}	

		mem_data *new_mem = new mem_data();
		new_mem->me = (void*) new_mem;
		shared_ptr<mem_data> sp(new_mem);
		grp->mems[new_mem->me] = sp; 
		return new_mem;
	}
	
	mem_ctx group_del_user(grp_ctx hdl, mem_ctx user) {
		if (grps.find(hdl) == grps.end()) {
			cerr<<"Failed to find hdl "<<hdl<<endl;
			return (void*)NULL;
		}
		shared_ptr<grp_data> grp = grps[hdl];
		
		map<mem_ctx, shared_ptr<mem_data> >::iterator iter = grp->mems.find(user);
		if (iter != grp->mems.end()) {
			grp->mems.erase(iter);
			return user;
		}	
		cerr<<"Failed to find member "<<user<<endl;
		return NULL;	
	}

	grp_ctx delete_group(grp_ctx hdl) {
		map<grp_ctx, shared_ptr<grp_data> >::iterator iter = grps.find(hdl);
		if (iter != grps.end()) {
			grps.erase(iter);
			return hdl;
		}
		return (grp_ctx)NULL;
	}

};
