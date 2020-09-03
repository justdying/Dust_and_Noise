#include "link_list.h"
#include "malloc.h"	
#include "usart.h"
LinkNode *usrgm3_head;
//初始化函数
void linklistInit(LinkNode **head)
{
    *head = NULL;
}
//创建新节点函数
LinkNode *CreateNode(LType value)
{
    //为新节点申请空间
    LinkNode *new_node = (LinkNode*)mymalloc(sizeof(LinkNode));
		if(NULL == new_node)
			return NULL;
    //给新节点赋值
    new_node->data = value;
    //将新节点的next指向空
    new_node->next = NULL;
    return new_node;
}
//销毁一个节点
void DestroyNode(LinkNode *node)
{
		myfree(node->data);
    myfree(node);
}
//顺序打印链表元素
void linklistPrint(LinkNode *head)
{
		LinkNode *cur;
    if(head == NULL)
    {
        //空链表无需打印
        return;
    }
    cur = head;
    //遍历链表
    while(cur != NULL)
    {
        //打印元素和其对应的地址
        printf("%p|%p\n",cur->data,cur);
        //移动cur,以达到遍历链表的目的
        cur = cur->next;
    }
    printf("\n\n");
}
//头插函数
void linklistPushFront(LinkNode **head,LType value)
{
		LinkNode *new_node;
    //非法输入
    if(head == NULL)
    {
        return;
    }
    //空链表
    if(*head == NULL)
    {
        //直接创建一个新节点完成插入
        *head = CreateNode(value);
        return;
    }
    else
    {
        //创建一个新的指针指向头结点
        new_node = *head;
        //创建一个新的头结点
        *head = CreateNode(value);
        //将新的头结点的next指向旧的头结点
        (*head)->next = new_node;
    }
    return;
}
//头删函数
void linklistPopFront(LinkNode **head)
{
		LinkNode *new_node;
    //非法输入
    if(head == NULL)
    {
        return;
    }
    //空链表没有可删除的元素
    if(*head == NULL)
    {
        return;
    }
    else
    {
        //创建一个新的指针指向第二个元素
        new_node = (*head)->next;
        //将头结点的next指向空
        (*head)->next = NULL;
        //删除该头结点
        DestroyNode(*head);
        //将第二个元素设置成新的头结点
        *head = new_node;
    }
    return;
}
//尾插函数
LinkNode *linklistPushBack(LinkNode **head,LType value)
{
		LinkNode *new_node,*cur;
    //非法输入
    if(head == NULL)
    {
        return NULL;
    }
    //空链表
    if(*head == NULL)
    {
        //直接创建一个新的节点完成元素插入
        *head = CreateNode(value);
        return NULL;
    }
    else
    {
        cur = *head;
        //遍历链表，让cur指向最后一个元素
        while(cur->next != NULL)
        {
            cur = cur->next;
        }
        //创建一个新节点
        new_node = CreateNode(value);
        //将最后一个元素的next指向新节点
        cur->next = new_node;
        return new_node;
    }
}
//尾删函数
void linklistPopBack(LinkNode **head)
{
		LinkNode *to_delete,*cur;
    //非法输入
    if(head == NULL)
    {
        return;
    }
    //空链表没有可删除的元素
    if(*head == NULL)
    {
        return;
    }
    //只有一个元素
    if((*head)->next == NULL)
    {
        //直接删除节点
        DestroyNode(*head);
        //将头结点置空
        *head = NULL;
        return;
    }
    else
    {
        cur = *head;
        //遍历链表，使cur指向倒数第二个元素
        while(cur->next->next != NULL)
        {
            cur = cur->next;
        }
        //创建指针指向最后一个元素，即我们需要删除的元素
        to_delete = cur->next;
        //将该节点销毁
        DestroyNode(to_delete);
        //将倒数第二个元素的next指向空
        cur->next = NULL;
    }
    return;
}
//判断链表是否为空链表
int linklistEmpty(LinkNode *head)
{
    //为空返回0否则返回1
    return head == NULL?0:1;
}
//求链表中元素个数，返回元素个数
uint8_t linklistSize(LinkNode *head)
{
		uint8_t count = 0;
		LinkNode *cur;
    //空链表返回0
    if(head == NULL)
    {
        return 0;
    }
    cur = head;
    //遍历链表
    for(;cur != NULL;cur = cur->next)
    {
        count++;
    }
    return count;
}

