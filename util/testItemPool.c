#include "ItemPool.h"


typedef struct TT TT;

struct TT
{
    int abc;
    int ccc;
};

void p(TT* a)
{
    printf("num : %x, \t", a->abc);
}

ItemPool(TT, TTPool);
ItemPool(int, intPool);

void f(TTPool* pol, TT** item)
{
	newItem(pol, *item);
}


int main()
{
    TTPool pool, *pp;
    intPool pppp;
    pp = &pool;
    initItemPool(pp);


    TT* item;

    int i = 10;
    for(; i > 0; --i)
    {
        //f(pp, &item);
	    newItem(pp, item);
		//
        printf("the addr is %x\n", item);

    }

    dumpPool(pp, p);
    freeItemPool(pp);
    return 0;
}
