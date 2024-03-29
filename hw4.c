#include <stdlib.h>
#include "hw4.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

struct memory_region
{
  size_t * start;
  size_t * end;
};

struct memory_region global_mem;
struct memory_region heap_mem;
struct memory_region stack_mem;

void walk_region_and_mark(void* start, void* end);

//how many ptrs into the heap we have
#define INDEX_SIZE 1000
void* heapindex[INDEX_SIZE];


//grabbing the address and size of the global memory region from proc 
void init_global_range()
{
  char file[100];
  char * line=NULL;
  size_t n=0;
  size_t read_bytes=0;
  size_t start, end;

  sprintf(file, "/proc/%d/maps", getpid());
  FILE * mapfile  = fopen(file, "r");

  if (mapfile==NULL)
  {
    perror("opening maps file failed\n");
    exit(-1);
  }

  int counter=0;
  while ((read_bytes = getline(&line, &n, mapfile)) != -1) 
  {
    if (strstr(line, "hw4")!=NULL)
    {
      ++counter;
      if (counter==3)
      {
        sscanf(line, "%lx-%lx", &start, &end);
        global_mem.start=(size_t*)start;
        // with a regular address space, our globals spill over into the heap
        global_mem.end=malloc(256);
        free(global_mem.end);
      }
    }
    else if (read_bytes > 0 && counter==3) 
    {
      if(strstr(line,"heap")==NULL) 
      {
        // with a randomized address space, our globals spill over into an unnamed segment directly following the globals
        sscanf(line, "%lx-%lx", &start, &end);
        printf("found an extra segment, ending at %zx\n",end);						
        global_mem.end=(size_t*)end;
      }
      break;
    }
  }
  fclose(mapfile);
}


//marking related operations

int is_marked(size_t* chunk) 
{
  return ((*chunk) & 0x2) > 0;
}

void mark(size_t* chunk)
{
  (*chunk)|=0x2;
}

void clear_mark(size_t* chunk) 
{
  (*chunk)&=(~0x2);
}

// chunk related operations

#define chunk_size(c)  ((*((size_t*)c))& ~(size_t)3 ) 

void* next_chunk(void* c) 
{ 
  if(chunk_size(c) == 0) 
  {
    printf("Panic, chunk is of zero size.\n");
  }
  if((c+chunk_size(c)) < sbrk(0))
    return ((void*)c+chunk_size(c));
  else 
    return 0;
}

int in_use(void *c) 
{ 
  return (next_chunk(c) && ((*(size_t*)next_chunk(c)) & 1));
}


// index related operations

#define IND_INTERVAL ((sbrk(0) - (void*)(heap_mem.start - 1)) / INDEX_SIZE)

//DID NOT IMPELEMENT
//void build_heap_index() {
  // TODO


// the actual collection code
void sweep() 
{
  // TODO

  // X is temporary
  //previous chunk start 
  size_t *X = heap_mem.start - 1; 
  //printf("A\n");
  //looping through 
    while (X != NULL && X < heap_mem.end)
    {
      //printf("B\n");
      //start by pointing to the new chunk to the pointer
      size_t *nextptr = next_chunk(X); 
      //
      if (is_marked(X))
      {
        //printf("C\n");
        //clear's the mark 
        clear_mark(X);
      }
               
      else if (in_use(X))
      {
        //printf("D\n");
        //free's chunk if it has already been used
        free(X+1); 
        //checks the block after 
        heap_mem.end = (size_t*)sbrk(0); 
      }
        //printf("E\n");
      X = nextptr; 
    }
      //printf("F\n");
}

//determine if what "looks" like a pointer actually points to a block in the heap
size_t * is_pointer(size_t * ptr) 
{
  // TODO
  //tmp = temporary pointer to look through chunks
  size_t* tmp; 
  //pointer to the previous part of the chunk
  size_t *startptr = heap_mem.start - 1; 
  if (ptr < heap_mem.start || ptr >= heap_mem.end)
    return NULL; 
  //checks the range of the chunk
  while (startptr != NULL && startptr < ptr)
  {
    //update the range within all the chunks 
    tmp = startptr; 
    //starts by looking at the next chunk as well 
    startptr = next_chunk(tmp); 
  }
  //return pointer into the final chunk
  return tmp;
}

void walk_region_and_mark(void* start, void* end) 
{
  
  // TODO
   size_t *tmpZ = (size_t*)start; 
  size_t* temp; 
  while (tmpZ != NULL && tmpZ < (size_t*) end) 
  {
    if (*tmpZ >= (size_t) heap_mem.start && *tmpZ < (size_t) heap_mem.end)
    {
      temp = is_pointer(*tmpZ);
    } 
    else
    {
      temp = NULL; 
    }
    if (temp != NULL && !is_marked(temp))
    {
      mark(temp); 
      walk_region_and_mark(temp + 1, next_chunk(temp)); 
    }
    tmpZ++; 
  }
}

// standard initialization 

void init_gc() 
{
  size_t stack_var;
  init_global_range();
  heap_mem.start=malloc(512);
  //since the heap grows down, the end is found first
  stack_mem.end=(size_t *)&stack_var;
}

void gc() 
{
  size_t stack_var;
  heap_mem.end=sbrk(0);
  //grows down, so start is a lower address
  stack_mem.start=(size_t *)&stack_var;

  // DID NOT IMPLEMENT 
  // build the index that makes determining valid ptrs easier
  // implementing this smart function for collecting garbage can get bonus;
  // if you can't figure it out, just comment out this function.
  // walk_region_and_mark and sweep are enough for this project.
  //build_heap_index();

  //walk memory regions
  walk_region_and_mark(global_mem.start,global_mem.end);
  walk_region_and_mark(stack_mem.start,stack_mem.end);
  sweep();
}

/* REFERENCES USED 
 - PDF FOR LAB 8 (ATTACHED)
 - TA XIANG
 - IN CLASS NOTES 
 - THIS WEBSITE - http://web.engr.illinois.edu/~maplant2/gc.html
 - lecture notes
 */

