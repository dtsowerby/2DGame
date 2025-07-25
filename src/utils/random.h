#pragma once

#include <stdlib.h>
#include <time.h>

void init_random()
{
    srand((unsigned int)time(NULL));
}

float random_range(float min, float max)
{
    return min + (rand() / (RAND_MAX + 1.0f)) * (max - min);
}
