#ifndef ENUM_H  
#define ENUM_H

typedef enum {
    RABBIT,
    SHEEP,
    PIG,
    COW,
    HORSE,
    FOX,
    WOLF,
    ANIMAL_COUNT 
} animalTypesDice;

typedef enum {
    RABBIT_,
    SHEEP_,
    PIG_,
    COW_,
    HORSE_,
    SMALL_DOG,
    BIG_DOG,
    ANIMAL_COUNT_SHOP
} animalTypesShop;

const char *animalNames[] = {
    [RABBIT] = "Rabbit",
    [SHEEP] = "Sheep",
    [PIG] = "Pig",
    [COW] = "Cow",
    [HORSE] = "Horse",
    [SMALL_DOG] = "Small Dog",
    [BIG_DOG] = "Big Dog",
    [FOX] = "Fox",
    [WOLF] = "Wolf"
};

#endif