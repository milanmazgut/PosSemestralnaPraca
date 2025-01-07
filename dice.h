#ifndef DICE_H
#define DICE_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "enums.h"



typedef struct dice{
    int capacity;                  
    int* probabilities; 
}dice; 


void dice_init(dice* dicePtr, int capacity, int* probabilities); 
void roll_dice(dice* dicePtr, animalTypesDice* type);
void dice_destroy(dice* dicePtr);                 

#endif
