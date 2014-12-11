/************************************************
*  zhangcan, 2014-12-10
*  zhangcan@tudou.com
*  copyright:youku.com
*  ********************************************/

#include "ketama_hash.h"

ketama_hash::ketama_hash(vector<server_info> &shards)
{
	init(shards);
}

ketama_hash::~ketama_hash()
{
}

void ketama_hash::setup_nodes()
{
	sort(_shards.begin(), _shards.end());

	vector<server_info>::iterator it;
	for (size_t i = 0; i < _shards.size(); i++) {
		for (int j = 0; j < _shards[i].weight; j++) {
			stringstream str_j;
			str_j << j;
			virtual_node node("server-" + _shards[i].name + "node-" + str_j.str(), i);
			_nodes.push_back(node);
		}
	}
	sort(_nodes.begin(), _nodes.end());
}

void ketama_hash::init(vector<server_info> &shards) 
{
	_shards.insert(_shards.end(), shards.begin(), shards.end());
	setup_nodes();
}

void ketama_hash::insert(server_info &s_info)
{
	server_info tmp = s_info;
	_shards.push_back(s_info);
	setup_nodes();
}

void ketama_hash::remove(string &name)
{
	vector<server_info>::iterator it;
	for (it = _shards.begin(); it != _shards.end(); it++)
	{
		if (it->name.compare(name) == 0)
			_shards.erase(it);
	}

	setup_nodes();
}

inline int ketama_hash::compare(const string &a, const string &b) 
{
	uint32_t a_hash = murmur_hash(a.c_str(), a.length());
	uint32_t b_hash = murmur_hash(b.c_str(), b.length());

	if (a_hash == b_hash) {
		return 0;
	} else if (a_hash < b_hash) {
		return -1;
	} else {
		return 1;
	}
}

virtual_node *ketama_hash::bsearch(string &key) 
{
	// search in the circle of the virtual nodes, return the closest node
	int virtual_nodes_num = _nodes.size();
	int i = 0;
	int j = virtual_nodes_num - 1;
	int mid = -1;
	while (true) {
		mid = (i + j) / 2;
		if (compare(key, _nodes[mid].name) == 0) {
			return &_nodes[mid];
		} else if (compare(key, _nodes[mid].name) < 0) {
			if (mid - 1 < 0)
				return &_nodes[0];
			else
				j = mid - 1;
		} else if (mid + 1 < virtual_nodes_num && compare(key, _nodes[mid + 1].name) <= 0) {
			return &_nodes[mid + 1];
		} else if (mid + 1 < virtual_nodes_num && compare(key, _nodes[mid + 1].name) > 0) {
			i = mid + 1;
		} else {
			return &_nodes[0];
		}
	}
}

server_info *ketama_hash::get_server_info(string &key)
{
	virtual_node * v_node = bsearch(key);
	return &_shards[v_node->s_info_idx];
}

uint32_t murmur_hash(const char *key, size_t len) 
{
	uint32_t  h, k;
	h = 0 ^ len;
	const unsigned char * data = (const unsigned char *)key;
	while (len >= 4) {
		k = data[0];
		k |= data[1] << 8;
		k |= data[2] << 16;
		k |= data[3] << 24;

		k *= 0x5bd1e995;
		k ^= k >> 24;
		k *= 0x5bd1e995;

		h *= 0x5bd1e995;
		h ^= k;

		data += 4;
		len -= 4;
	}

	switch (len) {
	case 3:
		h ^= data[2] << 16;
	case 2:
		h ^= data[1] << 8;
	case 1:
		h ^= data[0];
		h *= 0x5bd1e995;
	}

	h ^= h >> 13;
	h *= 0x5bd1e995;
	h ^= h >> 15;

	return h;
}
