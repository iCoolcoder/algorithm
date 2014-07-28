// Convert.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include<stdio.h>
#include<malloc.h>

#define CONTAINING_RECORD(address, type, field) (\
	(type *)((char*)(address) -(unsigned long)(&((type *)0)->field)))

typedef struct _LIST_ENTRY
{
	struct _LIST_ENTRY *Flink;
	struct _LIST_ENTRY *Blink;

}LIST_ENTRY,*PLIST_ENTRY;



void InitializeListHead( PLIST_ENTRY ListHead)
{
    ListHead->Flink = ListHead->Blink = ListHead;
}


void InsertHeadList( PLIST_ENTRY ListHead, PLIST_ENTRY Entry)
{
    PLIST_ENTRY Flink;

    Flink = ListHead->Flink;
    Entry->Flink = Flink;
    Entry->Blink = ListHead;
    Flink->Blink = Entry;
    ListHead->Flink = Entry;
}


typedef struct newstruct
{
	int num1;
	int num2;
	char alpha;
	LIST_ENTRY list_entry;
	float exp;

}MYSTRUCT,*PMYSTRUCT;


PLIST_ENTRY MyListHead;

int main()
{
    //LIST_ENTRY : Head
	MyListHead=(PLIST_ENTRY)malloc(sizeof(LIST_ENTRY));
	InitializeListHead(MyListHead);

	//First node
	MYSTRUCT *myNode=(MYSTRUCT*)malloc(sizeof(MYSTRUCT));
	
	MYSTRUCT Node;
	
	Node.num1=10;
	Node.num2=20;
	Node.alpha='c';
	Node.exp=5.5;
	
	myNode->num1=10;
	myNode->num2=20;
	myNode->alpha='c';
	myNode->exp=5.5;

	
	InsertHeadList(MyListHead,&(myNode->list_entry));

	//Second node
	myNode=(MYSTRUCT*)malloc(sizeof(MYSTRUCT));
	
	
	myNode->num1=100;
	myNode->num2=200;
	myNode->alpha='h';
	myNode->exp=10.08;

	InsertHeadList(MyListHead,&(myNode->list_entry));

	PLIST_ENTRY temp=0;
	temp=MyListHead;
	while(MyListHead!=temp->Flink)
	{
		temp=temp->Flink;
		int num1=CONTAINING_RECORD(temp,MYSTRUCT,list_entry)->num1;
		printf("num1=%d\n",num1);
	}

	__asm int 3
	return 0;
}

  
