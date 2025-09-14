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

            case ITEM_IRON_ORE: { Item_data_list[item] = create_item_data(item, "Iron ore", 100, 0x0b4b80FF, FALSE, BUILDING_NONE); break; }
            case ITEM_IRON_PLATE: { Item_data_list[item] = create_item_data(item, "Iron plate", 100, 0xa3cdf0FF, FALSE, BUILDING_NONE); break; }
            case ITEM_IRON_GEAR: { Item_data_list[item] = create_item_data(item, "Iron gear", 50, 0xabbcc9FF, FALSE, BUILDING_NONE); break; }

            case ITEM_COPPER_ORE: { Item_data_list[item] = create_item_data(item, "Copper ore", 100, 0x964520FF, FALSE, BUILDING_NONE); break; }
            case ITEM_COPPER_PLATE: { Item_data_list[item] = create_item_data(item, "Copper plate", 100, 0xed8b5fFF, FALSE, BUILDING_NONE); break; }
            case ITEM_COPPER_WIRE: { Item_data_list[item] = create_item_data(item, "Copper wire", 100, 0x964520FF, FALSE, BUILDING_NONE); break; }

            case ITEM_COAL_ORE: { Item_data_list[item] = create_item_data(item, "Coal ore", 100, 0x666666FF, FALSE, BUILDING_NONE); break; }

            case ITEM_CRAFTER_1: { Item_data_list[item] = create_item_data(item, "Crafter mk.1", 20, 0xFFAC1CFF, TRUE, BUILDING_CRAFTER_1); break; }
            case ITEM_BURNER_MINER: { Item_data_list[item] = create_item_data(item, "Burner Miner", 20, 0x999999FF, TRUE, BUILDING_BURNER_MINER); break; }
            case ITEM_BURNER_SMELTER: { Item_data_list[item] = create_item_data(item, "Burner Smelter", 20, 0xAAAAAAFF, TRUE, BUILDING_BURNER_SMELTER); break; }

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

