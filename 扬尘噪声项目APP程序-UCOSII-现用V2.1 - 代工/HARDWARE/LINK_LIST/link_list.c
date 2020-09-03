#include "link_list.h"
#include "malloc.h"	
#include "usart.h"
LinkNode *usrgm3_head;
//��ʼ������
void linklistInit(LinkNode **head)
{
    *head = NULL;
}
//�����½ڵ㺯��
LinkNode *CreateNode(LType value)
{
    //Ϊ�½ڵ�����ռ�
    LinkNode *new_node = (LinkNode*)mymalloc(sizeof(LinkNode));
		if(NULL == new_node)
			return NULL;
    //���½ڵ㸳ֵ
    new_node->data = value;
    //���½ڵ��nextָ���
    new_node->next = NULL;
    return new_node;
}
//����һ���ڵ�
void DestroyNode(LinkNode *node)
{
		myfree(node->data);
    myfree(node);
}
//˳���ӡ����Ԫ��
void linklistPrint(LinkNode *head)
{
		LinkNode *cur;
    if(head == NULL)
    {
        //�����������ӡ
        return;
    }
    cur = head;
    //��������
    while(cur != NULL)
    {
        //��ӡԪ�غ����Ӧ�ĵ�ַ
        printf("%p|%p\n",cur->data,cur);
        //�ƶ�cur,�Դﵽ���������Ŀ��
        cur = cur->next;
    }
    printf("\n\n");
}
//ͷ�庯��
void linklistPushFront(LinkNode **head,LType value)
{
		LinkNode *new_node;
    //�Ƿ�����
    if(head == NULL)
    {
        return;
    }
    //������
    if(*head == NULL)
    {
        //ֱ�Ӵ���һ���½ڵ���ɲ���
        *head = CreateNode(value);
        return;
    }
    else
    {
        //����һ���µ�ָ��ָ��ͷ���
        new_node = *head;
        //����һ���µ�ͷ���
        *head = CreateNode(value);
        //���µ�ͷ����nextָ��ɵ�ͷ���
        (*head)->next = new_node;
    }
    return;
}
//ͷɾ����
void linklistPopFront(LinkNode **head)
{
		LinkNode *new_node;
    //�Ƿ�����
    if(head == NULL)
    {
        return;
    }
    //������û�п�ɾ����Ԫ��
    if(*head == NULL)
    {
        return;
    }
    else
    {
        //����һ���µ�ָ��ָ��ڶ���Ԫ��
        new_node = (*head)->next;
        //��ͷ����nextָ���
        (*head)->next = NULL;
        //ɾ����ͷ���
        DestroyNode(*head);
        //���ڶ���Ԫ�����ó��µ�ͷ���
        *head = new_node;
    }
    return;
}
//β�庯��
LinkNode *linklistPushBack(LinkNode **head,LType value)
{
		LinkNode *new_node,*cur;
    //�Ƿ�����
    if(head == NULL)
    {
        return NULL;
    }
    //������
    if(*head == NULL)
    {
        //ֱ�Ӵ���һ���µĽڵ����Ԫ�ز���
        *head = CreateNode(value);
        return NULL;
    }
    else
    {
        cur = *head;
        //����������curָ�����һ��Ԫ��
        while(cur->next != NULL)
        {
            cur = cur->next;
        }
        //����һ���½ڵ�
        new_node = CreateNode(value);
        //�����һ��Ԫ�ص�nextָ���½ڵ�
        cur->next = new_node;
        return new_node;
    }
}
//βɾ����
void linklistPopBack(LinkNode **head)
{
		LinkNode *to_delete,*cur;
    //�Ƿ�����
    if(head == NULL)
    {
        return;
    }
    //������û�п�ɾ����Ԫ��
    if(*head == NULL)
    {
        return;
    }
    //ֻ��һ��Ԫ��
    if((*head)->next == NULL)
    {
        //ֱ��ɾ���ڵ�
        DestroyNode(*head);
        //��ͷ����ÿ�
        *head = NULL;
        return;
    }
    else
    {
        cur = *head;
        //��������ʹcurָ�����ڶ���Ԫ��
        while(cur->next->next != NULL)
        {
            cur = cur->next;
        }
        //����ָ��ָ�����һ��Ԫ�أ���������Ҫɾ����Ԫ��
        to_delete = cur->next;
        //���ýڵ�����
        DestroyNode(to_delete);
        //�������ڶ���Ԫ�ص�nextָ���
        cur->next = NULL;
    }
    return;
}
//�ж������Ƿ�Ϊ������
int linklistEmpty(LinkNode *head)
{
    //Ϊ�շ���0���򷵻�1
    return head == NULL?0:1;
}
//��������Ԫ�ظ���������Ԫ�ظ���
uint8_t linklistSize(LinkNode *head)
{
		uint8_t count = 0;
		LinkNode *cur;
    //��������0
    if(head == NULL)
    {
        return 0;
    }
    cur = head;
    //��������
    for(;cur != NULL;cur = cur->next)
    {
        count++;
    }
    return count;
}

