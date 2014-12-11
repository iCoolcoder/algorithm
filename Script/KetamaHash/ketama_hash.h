/************************************************
*  zhangcan, 2014-12-10
*  zhangcan@tudou.com
*  copyright:youku.com
*  ********************************************/

#ifndef _KETAMA_HASH_H_
#define _KETAMA_HASH_H_

#include <list>
#include <vector>
#include <string>
#include <sstream>
#include <stdint.h>
#include <algorithm>

using namespace std;

#define NODE_NUM	100

uint32_t murmur_hash(const char *data, size_t len);

struct server_info{
	string       name;
	uint32_t     ip;
	uint16_t     port;
	uint32_t     weight;

	inline bool operator<(const server_info &b) const
	{
		if (name.compare(b.name) < 0)
			return true;
		return false;
	}
	
};

struct virtual_node {
	string       name;
	size_t       s_info_idx;
	virtual_node(string name, size_t idx) : name(name), s_info_idx(idx)
	{
		
	}
	inline bool operator<(const virtual_node &b) const
	{
		uint32_t a_hash = murmur_hash(name.c_str(), name.length());
		uint32_t b_hash = murmur_hash(b.name.c_str(), b.name.length());

		if (a_hash < b_hash)
			return true;
		return false;
	}
};

class ketama_hash {
public:
	ketama_hash(vector<server_info> &shards);
	~ketama_hash();
	void insert(server_info &s_info);
	void remove(string &name);
	server_info *get_server_info(string &key);
private:
	virtual_node *bsearch(string &key);
	void init(vector<server_info> &shards);
	void setup_nodes();
	inline int compare(const string &a, const string &b);
private:
	vector<server_info> _shards;
	vector<virtual_node> _nodes;
};

#endif /* _KETAMA_HASH_H_ */
