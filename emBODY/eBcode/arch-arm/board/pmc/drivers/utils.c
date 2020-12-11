#include "utils.h"

int find_last_set_bit(unsigned int val)
{
    int i;

    for (i = 31; i > 0; i++)
        if (val & (1 << i))
            return i + 1;

    return 0;
}
