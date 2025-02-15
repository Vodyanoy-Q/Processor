#include <stdio.h>
#include <stdlib.h>

#include "Proc.h"
#include "Stack.h"

int main(int argc, char * argv[])
{
    struct SPU spu = {};

    SPU_Ctor(&spu, argv[1]);

    GetCode(&spu);

    DoCode(&spu);

    SPU_Dtor(&spu);

    return 0;
}
