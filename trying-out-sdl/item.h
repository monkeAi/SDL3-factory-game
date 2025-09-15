#ifndef ITEM_H
#define ITEM_H
#include "constants.h"

struct Item {
    enum ItemType type;
    unsigned int quantity;
    unsigned int color;
};

enum ItemType {
    ITEM_NONE,

    ITEM_IRON_ORE,
    ITEM_IRON_PLATE,
    ITEM_IRON_GEAR,

    ITEM_COPPER_ORE,
    ITEM_COPPER_PLATE,
    ITEM_COPPER_WIRE,

    ITEM_COAL_ORE,

    ITEM_STONE_ORE,
    ITEM_STONE_BRICK,

    ITEM_GREEN_CHIP,
    
    ITEM_CRAFTER_1,
    ITEM_BURNER_MINER,
    ITEM_BURNER_SMELTER,

    ITEM_INSERTER,
    ITEM_CONVEYOR
};

struct Item_data {
    enum ItemType type;
    char* name_string;
    unsigned int max_quantity;
    unsigned int color;
    int is_buildable;
    enum BuildingType building_type;
};

extern struct Item_data* Item_data_list[MAX_ITEMS];

struct Item_data* create_item_data(enum ItemType type, char* name_string, unsigned int max_quantity, unsigned int color, int is_buildable, enum BuildingType building_type);
void init_item_data_list(void);
void print_item_data_list(void);


struct Item Item_create(enum ItemType type, unsigned int quantity);
int Item_is_full(struct Item* item);
void Item_delete(struct Item* item);
int Item_get_color(enum ItemType type);

#endif
