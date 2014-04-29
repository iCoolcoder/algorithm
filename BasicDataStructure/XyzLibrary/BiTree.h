#pragma once

#ifdef XYZLIBRARY_EXPORTS
#define XYZAPI __declspec(dllexport)
#else
#define XYZAPI __declspec(dllimport)
#endif

typedef int T;
typedef int (*FunType)(T, T);

typedef struct BiNode
{
	T data;
	BiNode* left;
	BiNode* right;
}BiNode;

class XYZAPI BiTree
{
public:
	BiTree(void);
	~BiTree(void);
	BiNode* Create();
	BiNode* Find_PreOrder_Recursive(T data, FunType fun);
	BiNode* Insert(T data, FunType fp);
	BiNode* GetCommonParent_Recursive(BiNode* p, BiNode* q);
	bool IsBalanced_Recursive();
	DWORD GetHight();
	DWORD GetSize();
	BiNode* BiTreeToDoubleList();
private:
	DWORD _size;
	BiNode* _root;
	void Destroy(BiNode* root);
};

