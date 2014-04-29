#include "stdafx.h"
#include "BiTree.h"
#include <stdio.h>

BiTree::BiTree(void)
{
	_root = NULL;
	_size = 0;
}

BiTree::~BiTree(void)
{
	Destroy(_root);
}

BiNode* BiTree::Create()
{
	BiNode* root = NULL;

	T ch;
	printf("please input data");
	scanf("%c", &ch);
	if(ch != '#')
	{
		root = new BiNode;
		_size++;
		root->data = ch;
		root->left = Create();
		root->right = Create();
	}

	return root;
}

BiNode* _find_PreOrder(T data, BiNode* root, FunType fun)
{
	if(root)
	{
		if(fun(data, root->data) == 0)
			return root;

		BiNode* ret;
		if((ret = _find_PreOrder(data, root->left, fun)) != NULL)
			return ret;
		if((ret = _find_PreOrder(data, root->right, fun)) != NULL)
			return ret;
	}

	return NULL;
}

BiNode* BiTree::Find_PreOrder_Recursive(T data, FunType fun)
{
	return _find_PreOrder(data, _root, fun);
}

BiNode* BiTree::Insert(T data, FunType fun)
{
	BiNode* p = _root;
	BiNode* pre = NULL;
	bool isLeftChild = true;

	while(p != NULL)
	{
		if(fun(data, p->data) == 0)
			return p;
		else if(fun(data, p->data) > 0)
		{
			pre = p;
			p = p->right;
			isLeftChild = false;
		}
		else
		{
			pre = p;
			p = p->left;
		}
	}

	p = new BiNode;
	p->data = data;
	p->left = NULL;
	p->right = NULL;

	if(pre == NULL)
	{
		_root = p;
	}
	else if (isLeftChild)
	{
		pre->left = p;
	}
	else
	{
		pre->right = p;
	}
	_size++;
	return p;
}

BiNode* _getCommonParent(BiNode* p, BiNode* q, BiNode* root, int flag)
{
	if(NULL == root)
		return NULL;

	if(p == root || q == root)
		flag++;

	BiNode* left = _getCommonParent(p, q, root->left, flag);
	BiNode* right = _getCommonParent(p, q, root->right, flag);

	if(flag == 2)
	{
		if(left != NULL)
			return left;
		else if(right != NULL)
			return right;
		else
			return root;
	}
	else
		return NULL;
}

BiNode* BiTree::GetCommonParent_Recursive(BiNode* p, BiNode* q)
{
	return _getCommonParent(p, q, _root, 0);
}

DWORD _max(DWORD a, DWORD b)
{
	return a > b ? a : b;
}

DWORD _getHight(BiNode* root)
{
	if(root == NULL)
		return 0;
	else
		return _max(_getHight(root->left), _getHight(root->right)) + 1;
}

DWORD BiTree::GetHight()
{
	return _getHight(_root);
}

bool _isBalanced(BiNode* root)
{
	bool flag = true;
	if(root == NULL)
		return flag;

	DWORD distance = _getHight(root->left) - _getHight(root->right);

	if(distance <= 1 || distance >= -1)
		flag = true;
	else
		flag = false;

	flag = flag && _isBalanced(root->left) && _isBalanced(root->right);
	return flag;
}

bool BiTree::IsBalanced_Recursive()
{
	return _isBalanced(_root);	
}


void BiTree::Destroy(BiNode* root)
{
	if(root)
	{
		Destroy(root->left);
		Destroy(root->right);
		delete root;
	}
}

DWORD BiTree::GetSize()
{
	return _size;
}

void _biTreeToDoubleList(BiNode* root, BiNode* head, BiNode* tail)
{
	if(NULL == root)
		head = tail = NULL;

	BiNode* leftHead;
	BiNode* leftTail;
	BiNode* rightHead;
	BiNode* rightTail;

	_biTreeToDoubleList(root->left, leftHead, leftTail);
	_biTreeToDoubleList(root->right, rightHead, rightTail);

	if(leftTail)
	{
		leftTail->right = root;
		head = leftHead;
	}
	else
		head = root;
	root->left = leftTail;
	root->right = rightHead;
	if(rightHead)
	{
		rightHead->left = root;
		tail = rightTail;
	}
	else
		tail = root;

}

BiNode* BiTree::BiTreeToDoubleList()
{
	BiNode* head;
	BiNode* tail;
	_biTreeToDoubleList(_root, head, tail);
	
	return head;
}