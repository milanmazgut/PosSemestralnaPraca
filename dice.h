#ifndef DICE_H
#define DICE_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef enum {
    RABBIT,
    SHEEP,
    PIG,
    COW,
    HORSE,
    FOX,
    WOLF,
    ANIMAL_COUNT 
} animalTypes;

typedef struct {
    int capacity;                  
    int probabilities[ANIMAL_COUNT]; 
} dice;


void dice_init(dice* dicePtr, int capacity, int* probabilities); 
void dice_roll(dice* dicePtr, animalTypes* type);                 

#endif