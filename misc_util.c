#include "misc_util.h"

#include <assert.h>
#include <stdlib.h>

bool safer_realloc(void **p, size_t increased_size)
{
    assert(p != NULL);
    void *tmp = realloc(*p, increased_size);
    if (tmp == NULL)
    {
        return false;
    }
    else
    {
        *p = tmp;
        return true;
    }
}

void safe_dealloc(void **p, size_t decreased_size)
{
    assert(p != NULL);
    if (decreased_size == 0)
    {
        free(*p);
        *p = NULL;
    }
    else
    {
        void *tmp = realloc(*p, decreased_size);
        *p = tmp ? tmp : *p;
    }
}
