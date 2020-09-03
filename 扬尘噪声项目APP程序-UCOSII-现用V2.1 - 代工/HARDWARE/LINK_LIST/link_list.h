#ifndef __LINK_LIST_H
#define __LINK_LIST_H	 
#include "sys.h" 
#define LINK_LIST_LENTH   25
typedef uint8_t* LType;
typedef struct LinkNode
{
    //数据域
    LType data;
    //指针域
    struct LinkNode* next;
}LinkNode;
//初始化
void linklistInit(LinkNode **head);
//打印链表
void linklistPrint(LinkNode *head);
//节点销毁
void DestroyNode(LinkNode *node);
//头插
void linklistPushFront(LinkNode **head,LType value);
//头删
void linklistPopFront(LinkNode **head);
//尾插
LinkNode *linklistPushBack(LinkNode **head,LType value);
//尾删
void linklistPopBack(LinkNode **head);
//判断链表是否为空链表
int linklistEmpty(LinkNode *head);
//求链表中元素个数，返回元素个数
uint8_t linklistSize(LinkNode *head);


extern LinkNode *usrgm3_head;
#endif

