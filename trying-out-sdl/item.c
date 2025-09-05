#include <stdio.h>
#include <stdlib.h>
#include "SDL3/SDL.h"
#include "item.h"

struct Item_data* Item_data_list[MAX_ITEMS] = { NULL };

// Create a item_data struct meant for storing stats of every different item type
struct Item_data* create_item_data(enum ItemType type, char* name_string, unsigned int max_quantity, unsigned int color) {

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

    return item_data;
}

// Item data list is a dictionary of all item types
void init_item_data_list(void) {

    // Loop through all items and add them to the list
    for (int item = 0; item < MAX_ITEMS; item++) {

        switch (item) {
            case ITEM_NONE: { Item_data_list[item] = create_item_data(item, "Item none", 0, 0x000000FF); break; }
            case ITEM_IRON_ORE: { Item_data_list[item] = create_item_data(item, "Iron ore", 100, 0x0b4b80FF); break; }
            case ITEM_IRON_PLATE: { Item_data_list[item] = create_item_data(item, "Iron plate", 100, 0xa3cdf0FF); break; }
            case ITEM_IRON_GEAR: { Item_data_list[item] = create_item_data(item, "Iron gear", 50, 0xabbcc9FF); break; }
            case ITEM_COPPER_ORE: { Item_data_list[item] = create_item_data(item, "Copper ore", 100, 0x964520FF); break; }
            case ITEM_COPPER_PLATE: { Item_data_list[item] = create_item_data(item, "Copper plate", 100, 0xed8b5fFF); break; }
            case ITEM_COPPER_WIRE: { Item_data_list[item] = create_item_data(item, "Copper wire", 100, 0x964520FF); break; }
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

