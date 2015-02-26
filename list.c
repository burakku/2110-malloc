#include <stdlib.h>
#include "./list.h"
#include "my_malloc.h"

static NODE* create_node(int* data)
{
	//Check for null input
    if(data == NULL)
    {
      return NULL;
    }
    NODE* new_node;
    new_node = (NODE*)my_malloc(sizeof(NODE));
    if(new_node == NULL)
    {
      return NULL;
    }
    //initialize NODE
    new_node -> data = *data;
    new_node -> prev = NULL;
    new_node -> next = NULL;
    return new_node;
}

LIST* create_list(void)
{
    LIST* new_list;
    new_list = (LIST*)my_malloc(sizeof(LIST));
    if(new_list == NULL)
    {
      return NULL;
    }
    //intitialize LIST
    new_list -> head = NULL;
    new_list -> tail = NULL;
    new_list -> size = 0;
    return new_list;
}

/**
 * pushFront
 * Takes in ptr to a list and data to add
 * Should add new node containing the data to the head of the list, and increment size
 */
void pushFront(LIST *list, int data)
{
	//Check for null input
	if(list == NULL)
    {
      return;
    }

    NODE* new_head;
    new_head = create_node(&data);
    if(new_head == NULL)
    {
      return;
    }
    new_head -> next = list -> head;
    list -> head = new_head;

    if(list -> tail == NULL)
    {
      list -> tail = new_head;
    }
    else
    {
      new_head -> next -> prev = new_head;
    }
    (list -> size)++;
}

/**
 * pushFront
 * Takes in ptr to a list and data to add
 * Should add new node containing the data to the tail of the list, and increment size
 */
void pushBack(LIST *list, int data)
{
	//Check for null input
	if(list == NULL)
    {
      return;
    }

    NODE* new_tail;
    new_tail = create_node(&data);
    if(new_tail == NULL)
    {
      return;
    }
    new_tail -> prev = list -> tail;
    list -> tail = new_tail;

    if(list -> head == NULL)
    {
      list -> head = new_tail;
    }
    else
    {
      new_tail -> prev -> next = new_tail;
    }
    (list -> size)++;
}

/**
 * popFront
 * Takes in ptr to a list
 * Remove and free node at the front of the list, and decrement size
 * Return the value of that node
 * Return 0 if the size of the list is 0
 */
int popFront(LIST *list)
{
	if(list == NULL)
  	{
    	return 0;
  	}
  	NODE* temp;
  	temp = list -> head;
  	if(temp == NULL)
  	{
  	  return 0;
  	}
  	list -> head = temp -> next;
  	int value = temp -> data;

  	if(list -> head == NULL)
  	{
    	list -> tail = NULL;
  	}
  	else
  	{
  	  list -> head -> prev = NULL;
  	}
    //my_free(&(temp -> data));
  	my_free(temp);
  	(list -> size)--;
	return value;
}

/**
 * popBack
 * Takes in ptr to a list
 * Remove and free node at the back of the list, and decrement size
 * Return the value of that node
 * Return 0 if the size of the list is 0
 */
int popBack(LIST *list)
{
	if(list == NULL)
  	{
    	return 0;
  	}
  	NODE* temp;
  	temp = list -> tail;
  	if(temp == NULL)
  	{
  	  return 0;
  	}
  	list -> tail = temp -> prev;
  	int value = temp -> data;

  	if(list -> tail == NULL)
  	{
    	list -> head = NULL;
  	}
  	else
  	{
  	  list -> tail -> next = NULL;
  	}
    //my_free(&(temp -> data));
  	my_free(temp);
  	(list -> size)--;
	return value;
}


