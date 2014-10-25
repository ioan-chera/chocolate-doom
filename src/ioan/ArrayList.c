#include <string.h>
#include "ArrayList.h"
#include "../z_zone.h"

void ArrayList_Init(ArrayList* al, size_t elemsize, size_t initalloc)
{
    al->nelts = 0;
    al->elemsize = elemsize;
    if(!al->elts)
    {
        al->nalloc = initalloc;
        al->elts = Z_Malloc((int)(elemsize * initalloc), PU_STATIC, NULL);
    }
}

void ArrayList_Add(ArrayList* al, const void* elem)
{
    if(al->nelts == al->nalloc)
    {
        al->nalloc <<= 1;
        void* p = al->elts;
        al->elts = Z_Malloc((int)(al->elemsize * al->nalloc), PU_STATIC,
                            NULL);
        if(p)
            memcpy(al->elts, p, al->nelts * al->elemsize);
        Z_Free(p);
    }
    memcpy((char*)al->elts + al->elemsize * al->nelts++, elem, al->elemsize);
}