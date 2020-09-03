#ifndef __LINK_LIST_H
#define __LINK_LIST_H	 
#include "sys.h" 
#define LINK_LIST_LENTH   25
typedef uint8_t* LType;
typedef struct LinkNode
{
    //������
    LType data;
    //ָ����
    struct LinkNode* next;
}LinkNode;
//��ʼ��
void linklistInit(LinkNode **head);
//��ӡ����
void linklistPrint(LinkNode *head);
//�ڵ�����
void DestroyNode(LinkNode *node);
//ͷ��
void linklistPushFront(LinkNode **head,LType value);
//ͷɾ
void linklistPopFront(LinkNode **head);
//β��
LinkNode *linklistPushBack(LinkNode **head,LType value);
//βɾ
void linklistPopBack(LinkNode **head);
//�ж������Ƿ�Ϊ������
int linklistEmpty(LinkNode *head);
//��������Ԫ�ظ���������Ԫ�ظ���
uint8_t linklistSize(LinkNode *head);


extern LinkNode *usrgm3_head;
#endif

