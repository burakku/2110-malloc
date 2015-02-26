#include "my_malloc.h"

/* You *MUST* use this macro when calling my_sbrk to allocate the 
 * appropriate size. Failure to do so may result in an incorrect
 * grading!
 */
#define SBRK_SIZE 2048

/* If you want to use debugging printouts, it is HIGHLY recommended
 * to use this macro or something similar. If you produce output from
 * your code then you will receive a 20 point deduction. You have been
 * warned.
 */
#ifdef DEBUG
#define DEBUG_PRINT(x) printf x
#else
#define DEBUG_PRINT(x)
#endif


/* make sure this always points to the beginning of your current
 * heap space! if it does not, then the grader will not be able
 * to run correctly and you will receive 0 credit. remember that
 * only the _first_ call to my_malloc() returns the beginning of
 * the heap. sequential calls will return a pointer to the newly
 * added space!
 * Technically this should be declared static because we do not
 * want any program outside of this file to be able to access it
 * however, DO NOT CHANGE the way this variable is declared or
 * it will break the autograder.
 */
void* heap;

/* our freelist structure - this is where the current freelist of
 * blocks will be maintained. failure to maintain the list inside
 * of this structure will result in no credit, as the grader will
 * expect it to be maintained here.
 * Technically this should be declared static for the same reasons
 * as above, but DO NOT CHANGE the way this structure is declared
 * or it will break the autograder.
 */
metadata_t* freelist[8];
/**** SIZES FOR THE FREE LIST ****
 * freelist[0] -> 16
 * freelist[1] -> 32
 * freelist[2] -> 64
 * freelist[3] -> 128
 * freelist[4] -> 256
 * freelist[5] -> 512
 * freelist[6] -> 1024
 * freelist[7] -> 2048
 */

static int getIndex(size_t size)
{
  if (size > 1024)
    return 7;
  else if (size > 512)
    return 6;
  else if (size > 256)
    return 5;
  else if (size > 128)
    return 4;
  else if (size > 64)
    return 3;
  else if (size > 32)
    return 2;
  else if (size > 16)
    return 1;
  else
    return 0;
}

static int getSize(int i)
{
  if(i == 0)
    return 16;
  else if(i == 1)
    return 32;
  else if(i == 2)
    return 64;
  else if(i == 3)
    return 128;
  else if(i == 4)
    return 256;
  else if(i == 5)
    return 512;
  else if(i == 6)
    return 1024;
  else
    return 2048;
}

void checklist()
{
	for(int i = 0; i < 8; i++)
	{
		metadata_t* p = freelist[i];
		while(p)
		{
			printf("I'm in freelist %d.\n", i);
      printf("p is %d.\n", (int)((char*)p - (char*)heap));
      printf("p next is %d.\n", (int)((char*)(p -> next) - (char*)heap));
      printf("p size is %d.\n", p -> size);
			p = p -> next;
		}
	}
}

void* my_malloc(size_t size)
{
  if(size == 0)
  {
  	return NULL;
  }
  //total adresses needed
  size_t total = size + sizeof(metadata_t);

  //Exam if the reqested size from malloc exceed maximun
  if(total > SBRK_SIZE)
  {
    printf("Too greedy!!!!!!!!!!.\n");
  	ERRNO = SINGLE_REQUEST_TOO_LARGE;
  	return NULL;
  }

  //initialize heap
  if(!heap)
  {
  	heap = my_sbrk(SBRK_SIZE);
  	//check if heap can be initialized
  	if(heap == (void*)-1)
  	{
  		ERRNO = OUT_OF_MEMORY;
  		return NULL;
  	}
  	//intitialize freelist to NULL
  	for(int i = 0; i < 8; i++)
  	{
  		freelist[i] = NULL;
  	}
    //initialize freelist[7] first
  	metadata_t* block7 = (metadata_t*) heap;
  	block7 -> in_use = 0;
  	block7 -> size = SBRK_SIZE;
  	block7 -> next = NULL;
  	block7 -> prev = NULL;
  	freelist[7] = block7;

  /////////////////////////////////////////
  printf("Heap initialized to %d.\n", (int)heap);

  }
  //if heap initialized, do the following
  metadata_t* value;
  //obtain requested size index
  int i = getIndex(total);
  if(i == 7)
  {
  	value = freelist[7];
  	//no enough space for 2048
  	if(value == NULL)
  	{
      value = (metadata_t*)my_sbrk(SBRK_SIZE);
      printf("\n\n\n\nPointer moved to %d.\n\n\n\n", (int)value);
      if(value == (void*)-1)
      {
        ERRNO = OUT_OF_MEMORY;
        return NULL;
      }
      value -> next = NULL;
      value -> prev = NULL;
      value -> size = SBRK_SIZE;
      value -> in_use = 0;
      freelist[7] = value;
  	}
  	//distribute block of 2048
  	freelist[7] = NULL;
  	value -> in_use = 1;
  	value -> next = NULL;
  	ERRNO = NO_ERROR;
  	return ((char*)value) + sizeof(metadata_t);
  }
  else
  {
  	value = freelist[i];
  	//no space for i size
  	if(value == NULL)
  	{
  		//check bigger block
  		char* ptr = (char*)my_malloc(getSize(i + 1) - sizeof(metadata_t));
  		//no bigger block available
  		if(ptr == NULL)
  		{
        if(i == 0)
        {
          ERRNO = OUT_OF_MEMORY;
        }
  			return NULL;
  		}
      value = (metadata_t*)(ptr - sizeof(metadata_t));
  		//got bigger block, and split into half size
  		metadata_t* half = (metadata_t*)((char*)value + (value -> size) / 2);
  		half -> size = (value -> size) / 2;
  		half -> in_use = 0;
  		half -> next = NULL;
  		half -> prev = NULL;
  		freelist[i] = half;
  		//distribute block of i size
  		value -> size = (value -> size) / 2;
  		value -> next = NULL;
  		value -> prev = NULL;
  		ERRNO = NO_ERROR;
      
    ////////////////////////////
    //printf("The value returned is %d.\n", (int)((char*)value - (char*)heap));




      if(getIndex(value -> size) != i)
      {
        printf("I'm so wrong here!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!.\n");
      }
  		return ((char*)value) + sizeof(metadata_t);
    }
  	//distribute block from freelist
  	freelist[i] = value -> next;
  	value -> in_use = 1;
  	value -> next = NULL;
  	if(freelist[i])
  	{
  		freelist[i] -> prev = NULL;
  	}
  	ERRNO = NO_ERROR;


    ////////////////////////////
    //printf("The value returned from list is %d.\n", (int)((char*)value - (char*)heap));




      if(getIndex(value -> size) != i)
      {
        printf("I'm so wrong here!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!.\n");
      }
  	return ((char*)value) + sizeof(metadata_t);
  }
}

void* my_calloc(size_t num, size_t size)
{
  if(num == 0 || size == 0)
  {
  	 return NULL;
  }
  size_t total = num * size;
  void* ptr = my_malloc(total);
  if(ptr == NULL)
  {
  	return NULL;
  }

  for(int i = 0; i < total; i++)
  {
    ((char**)ptr)[i] = (char*)NULL;
  }
  ERRNO = NO_ERROR;
  return ptr;
}

void my_free(void* ptr)
{
  if(ptr == NULL)
  {
  	return;
  }
  char* cptr = (char*)ptr;
  metadata_t* p = (metadata_t*)(cptr - sizeof(metadata_t));

  if((char*)p - (char*)heap == 6144)
  {
    printf("!!!!!!!!!!!!!!!!!!first get 6144 size is: %d.\n", (int)p -> size);
  }

  //printf("This guy wanna be free! %d.\n", (int)((char*)p - (char*)heap));
  //printf("Tell me the size I will let you be free: %d.\n", (int)p -> size);
  if(p -> in_use == 0)
  {
  	ERRNO = DOUBLE_FREE_DETECTED;
  	return;
  }
  //get the size index of the block which will be freed
  int i = getIndex(p -> size);

  /////printf("i in free is %d.\n", i);
  //loop the freelist to merge
  while(i < 8)
  {
	  //check if we will free a block of size 2048 
	  if(i == 7)
	  {
      if(freelist[i] == p)
      {
        //printf("I don't bother you no more.\n");
        p -> in_use = 0;
        ERRNO = NO_ERROR;
        return;
      }
	  	p -> in_use = 0;
      p -> next = freelist[7];
	  	freelist[7] = p;
      if(p -> next != NULL)
      {
        p -> next -> prev = p;
      }
	  	ERRNO = NO_ERROR;
	  	return;
	  }
	  metadata_t* block = freelist[i];
    if(p == block)
    {
      block = block -> next;
    }
    p -> in_use = 0;

	  int match = 0;
    //check if any block is p's buddy
	  while(block != NULL && !match)
	  {
      //printf("compare p is %d, block is %d. Match is %d.\n", (int)((char*)p - (char*)heap), (int)((char*)block - (char*)heap), match);
      //printf("Block next is %d.\n", (int)((char*)(block -> next) - (char*)heap));
      //printf("Block size is %d.\n", block -> size);
      //printf("Faul number: %d\n",((int)((char*)p - (char*)heap)^ (int)((char*)block - (char*)heap)^(0x1 << (4 + i))) ==0);

      //use bitwise check if two blocks are buddy
	  	if(((((char*)p - (char*)heap) ^ ((char*)block - (char*)heap) ^ (0x1 << (4 + i))) == 0) && !(block -> in_use))
	  	{
        match = 1;
        //printf("compare p is %d, block is %d. Match is %d.\n", (int)((char*)p - (char*)heap), (int)((char*)block - (char*)heap), match);
        // printf("Block next is %d.\n", (int)((char*)(block -> next) - (char*)heap));
        // printf("Block size is %d.\n", block -> size);
        //printf("Found buddy.\n");
	  	}
	  	if(!match)
	  	{
	  		block = block -> next;
	  	}
	  }
	  //p has no buddy
	  if(!match)
	  {
      if(freelist[i] == p)
      {
        //printf("I don't bother you no more.\n");
        ERRNO = NO_ERROR;
        return;
      }
	  	//add p to the list
	  	p -> next = freelist[i];
	  	if(freelist[i])
	  	{
	  		p -> next -> prev = p;
	  	}
	  	freelist[i] = p;

      //printf("free p %d next is %d.\n", (int)((char*)p - (char*)heap),(int)((char*)(p -> next) - (char*)heap));
	  	ERRNO = NO_ERROR;
	  	return;
	  }
	  //p has buddy, do:
	  //when p has prev and next
	  if(block -> next && block -> prev)
	  {
	  	block -> prev -> next = block -> next;
	  	block -> next -> prev = block -> prev;
	  	block -> next = NULL;
	  	block -> prev = NULL;
	  }
	  //when p at the head of list
	  else if(block -> next)
	  {
	  	block -> next -> prev = NULL;
	  	freelist[i] = block -> next;
	  	block ->next = NULL;
	  }
	  //when p at the tail of list
	  else if(block -> prev)
	  {
	  	block -> prev -> next = NULL;
	  	block -> prev = NULL;
	  }
	  //when p is the only one in list
	  else
	  {
	  	freelist[i] = NULL;
	  }
	  //pick the address in the front
	  if(block < p)
	  {
	  	p = block;
	  }
	  //merge two blocks
	  p -> size *= 2;
	  p -> next = freelist[i + 1];
	  freelist[i + 1] = p;
	  if(p -> next)
	  {
	  	p -> next -> prev = p;
	  }
    //printf("Merged p size is %d.\n", p -> size);
	  i++;
    printf("%d merged to %d.\n", (int)((char*)p - (char*)heap), i);

    // if((char*)p - (char*)heap == 6144 && i == 5)
    // {

    // }


    for(int j = 0; j < 8; j++)
    {
      metadata_t* c = freelist[j];
      while(c)
      {
        if(getIndex(c -> size) != j){
        printf("I'm so wrong here!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!.\n");}


      printf("I'm in freelist %d.\n", j);
      printf("p is %d.\n", (int)((char*)c - (char*)heap));
      printf("p next is %d.\n", (int)((char*)(c -> next) - (char*)heap));
      printf("p size is %d.\n", c -> size);

      }
    }
	}
}

void* my_memmove(void* dest, const void* src, size_t num_bytes)
{
  if(dest == NULL || src == NULL || num_bytes == 0)
  {
  	return NULL;
  }
  if(dest == src)
  {
  	return (void*)dest;
  }

  if(dest > src)
  {
  	//if dest > src, copy from tail to head
  	for(int i = num_bytes - 1; i > 0; i--)
  	{
  		((char*)dest)[i] = ((char*)src)[i];
  	}
  }
  else
  {
  	//if dest < src, copy from head to tail
  	for(int i = 0; i < num_bytes; i++)
  	{
  		((char*)dest)[i] = ((char*)src)[i];
  	}
  }
  return (void*)dest;
}
