#ifndef choco_xcode_ArrayList_h
#define choco_xcode_ArrayList_h

#include <stdlib.h>

typedef struct ArrayList
{
    size_t  nalloc;
    size_t  nelts;
    size_t  elemsize;
    void*   elts;
} ArrayList;

void ArrayList_Init(ArrayList* al, size_t elemsize, size_t initalloc);
void ArrayList_Add(ArrayList* al, const void* elem);

#endif
