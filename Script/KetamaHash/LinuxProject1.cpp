#include <iostream>
#include "ketama_hash.h"
#include "server_controller.h"
#include <hash_map.h>
#include <cassert>

using namespace std;
void testcase1()
{
	// initialize server information
	vector<server_info> shards;
	server_info s_info;
	s_info.ip = 111;
	s_info.port = 80;
	s_info.name = "abc";
	s_info.weight = 100;
	shards.push_back(s_info);
	server_info s_info1;
	s_info1.ip = 222;
	s_info1.port = 80;
	s_info1.name = "def";
	s_info1.weight = 100;
	shards.push_back(s_info1);

	// create ketama hash object
	ketama_hash ketama(shards);
	string key = "server-defnode-79";
	cout << murmur_hash(key.c_str(), key.length()) << endl;
	server_info *ret = ketama.get_server_info(key);
	cout << ret->name << endl;

	// verify the result
	assert(ret->name.compare("def") == 0);
	
}

void testcase2()
{
	// we have three physical servers with the same weight named by "abc", "def", "ghi". 
	// After ketama hash, verify the mount of hash keys hitting on each server
	vector<server_info> shards;
	server_info s_info;
	s_info.ip = 111;
	s_info.port = 80;
	s_info.name = "abc";
	s_info.weight = 100;
	shards.push_back(s_info);
	server_info s_info1;
	s_info1.ip = 222;
	s_info1.port = 80;
	s_info1.name = "def";
	s_info1.weight = 100;
	shards.push_back(s_info1);
	server_info s_info2;
	s_info2.ip = 333;
	s_info2.port = 80;
	s_info2.name = "ghi";
	s_info2.weight = 100;
	shards.push_back(s_info2);
	ketama_hash ketama(shards);
	
	hash_map<const char *, int> map;
	for (int i = 0; i < 10000; i++)
	{
		stringstream str_i;
		str_i << i;
		string key(str_i.str());
		server_info *ret = ketama.get_server_info(key);
		map[(ret->name).c_str()]++;
	}

	hash_map<const char *, int>::iterator it;
	for (it = map.begin(); it != map.end(); it++)
	{
		cout << it->first << "  :  ";
		cout << it->second << endl;
	}
}

void testcase3()
{
	// initialize server information
	vector<server_info> shards;
	server_info s_info;
	s_info.ip = 111;
	s_info.port = 80;
	s_info.name = "abc";
	s_info.weight = 100;
	shards.push_back(s_info);
	server_info s_info1;
	s_info1.ip = 222;
	s_info1.port = 80;
	s_info1.name = "def";
	s_info1.weight = 100;
	shards.push_back(s_info1);
	server_info s_info2;
	s_info2.ip = 333;
	s_info2.port = 80;
	s_info2.name = "ghi";
	s_info2.weight = 100;

	// create ketama hash object
	server_controller controller(shards);
	// insert a server
	controller.insert(s_info2);
	// remove a server
	string str("abc");
	controller.remove(str);

	string key = "server-defnode-79";
	cout << murmur_hash(key.c_str(), key.length()) << endl;
	server_info *ret = controller.get_server_info(key);
	cout << ret->name << endl;

}

void testcase4()
{
	// initialize server information
	vector<server_info> shards;
	server_info s_info;
	s_info.ip = 111;
	s_info.port = 80;
	s_info.name = "abc";
	s_info.weight = 100;
	shards.push_back(s_info);
	server_info s_info1;
	s_info1.ip = 222;
	s_info1.port = 80;
	s_info1.name = "def";
	s_info1.weight = 100;
	shards.push_back(s_info1);
	server_info s_info2;
	s_info2.ip = 333;
	s_info2.port = 80;
	s_info2.name = "ghi";
	s_info2.weight = 100;

	// create ketama hash object
	ketama_hash ketama(shards);
	ketama.insert(s_info2);
	string str("abc");
	ketama.remove(str);
	string key = "server-defnode-79";
	cout << murmur_hash(key.c_str(), key.length()) << endl;
	server_info *ret = ketama.get_server_info(key);
	cout << ret->name << endl;

	// verify the result
	assert(ret->name.compare("def") == 0);

}

int main(int argc, char *argv[])
{
	testcase1();
	testcase2();
	testcase3();
	testcase4();
	return 0;
}
