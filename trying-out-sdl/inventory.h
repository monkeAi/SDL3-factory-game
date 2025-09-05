#ifndef INVENTORY_H
#define INVENTORY_H

#include "item.h"

struct Inventory {
    unsigned int max_slots;
    unsigned int available_slots;
    struct Item *slots;
};


// Function Protoypes
struct Inventory* Inventory_create(unsigned int max_slots, unsigned int available_slots);
int Inventory_find_free_slot(struct Inventory* inv, enum ItemType item_type);
int Inventory_push_item(struct Inventory* inv, struct Item* item);
int Inventory_print(struct Inventory* inv);
int Inventory_search_item(struct Inventory* inv, enum ItemType item_type);
int Inventory_transfer_item(struct Inventory* from_inv, struct Inventory* to_inv, int from_slot, unsigned int quantity);
void Inventory_free(struct Inventory* inv);
int Inventory_enough_space(struct Inventory* inv, enum ItemType item_type, int required_amount);

#endif