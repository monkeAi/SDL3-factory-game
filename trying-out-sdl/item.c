#include <stdio.h>
#include <stdlib.h>
#include "SDL3/SDL.h"
#include "item.h"
#include "constants.h"
#include "buildings.h"

struct Item_data* Item_data_list[MAX_ITEMS] = { NULL };

// Create a item_data struct meant for storing stats of every different item type
struct Item_data* create_item_data(enum ItemType type, char* name_string, unsigned int max_quantity, unsigned int color, int is_buildable, enum BuildingType building_type) {

    // Allocate enough memory to the item_data pointer
    struct Item_data* item_data = (struct Item_data*)malloc(sizeof(struct Item_data));
    if (item_data == NULL) {
        fprintf(stderr, "Failed to allocate memory for item data.\n");
        exit(1);
    }

    item_data->type = type;
    item_data->name_string = name_string;
    item_data->max_quantity = max_quantity;
    item_data->color = color;
    item_data->is_buildable = is_buildable;
    item_data->building_type = building_type;

    return item_data;
}

// Item data list is a dictionary of all item types
void init_item_data_list(void) {

    // Loop through all items, initialize their parameters and add them to the list
    for (int item = 0; item < MAX_ITEMS; item++) {

        switch (item) {
        case ITEM_NONE: { Item_data_list[item] = create_item_data(item, "Item none", 0, 0x000000FF, FALSE, BUILDING_NONE); break; }

            case ITEM_IRON_ORE: { Item_data_list[item] = create_item_data(item, "Iron ore", 100, 0x85c9d8FF, FALSE, BUILDING_NONE); break; }
            case ITEM_IRON_PLATE: { Item_data_list[item] = create_item_data(item, "Iron plate", 100, 0x6de7fcFF, FALSE, BUILDING_NONE); break; }
            case ITEM_IRON_GEAR: { Item_data_list[item] = create_item_data(item, "Iron gear", 50, 0xabdfebFF, FALSE, BUILDING_NONE); break; }

            case ITEM_COPPER_ORE: { Item_data_list[item] = create_item_data(item, "Copper ore", 100, 0xf19143FF, FALSE, BUILDING_NONE); break; }
            case ITEM_COPPER_PLATE: { Item_data_list[item] = create_item_data(item, "Copper plate", 100, 0xf5984cFF, FALSE, BUILDING_NONE); break; }
            case ITEM_COPPER_WIRE: { Item_data_list[item] = create_item_data(item, "Copper wire", 100, 0xfcae3fFF, FALSE, BUILDING_NONE); break; }

            case ITEM_COAL_ORE: { Item_data_list[item] = create_item_data(item, "Coal ore", 100, 0x2e3535FF, FALSE, BUILDING_NONE); break; }

            case ITEM_STONE_ORE: { Item_data_list[item] = create_item_data(item, "Stone ore", 100, 0xd7cfcdFF, FALSE, BUILDING_NONE); break; }
            case ITEM_STONE_BRICK: { Item_data_list[item] = create_item_data(item, "Stone brick", 100, 0xded2c5FF, FALSE, BUILDING_NONE); break; }

            case ITEM_GREEN_CHIP: { Item_data_list[item] = create_item_data(item, "Green circuits", 100, 0x5ed160FF, FALSE, BUILDING_NONE); break; }

            case ITEM_CRAFTER_1: { Item_data_list[item] = create_item_data(item, "Crafter mk.1", 20, 0xFFBD33FF, TRUE, BUILDING_CRAFTER_1); break; }
            case ITEM_BURNER_MINER: { Item_data_list[item] = create_item_data(item, "Burner Miner", 20, 0xa84e32ff, TRUE, BUILDING_BURNER_MINER); break; }
            case ITEM_BURNER_SMELTER: { Item_data_list[item] = create_item_data(item, "Burner Smelter", 20, 0xedda5fff, TRUE, BUILDING_BURNER_SMELTER); break; }

            case ITEM_INSERTER: { Item_data_list[item] = create_item_data(item, "Inserter", 50, 0x325e8aff, TRUE, BUILDING_INSERTER); break; }
            case ITEM_INSERTER_LONG: { Item_data_list[item] = create_item_data(item, "Long Inserter", 50, 0xc4063cff, TRUE, BUILDING_INSERTER_LONG); break; }
            case ITEM_CONVEYOR: { Item_data_list[item] = create_item_data(item, "Conveyor belt", 200, 0xffb60aff, TRUE, BUILDING_CONVEYOR); break; }



            default: { Item_data_list[item] = NULL; break; }
        }

    }
    
}

void print_item_data_list(void) {
    
    for (int item = 0; Item_data_list[item] != NULL; item++) {
        printf("Item %s: max count: %d \n", Item_data_list[item]->name_string, Item_data_list[item]->max_quantity);
    }

}

struct Item Item_create(enum ItemType type, unsigned int quantity) {
    struct Item item;
    item.type = type;
    item.quantity = quantity;
    item.color = Item_data_list[item.type]->color;
    return item;
}

// Checks if item is at full capacity
int Item_is_full(struct Item* item) {

	if (item->quantity < Item_data_list[item->type]->max_quantity) return 0;
	else return 1;

}

// Reset item back to default
void Item_delete(struct Item* item) {
    item->type = ITEM_NONE;
    item->quantity = 0;
}

