#include "item.h"


struct Item Item_create(enum ItemType type, unsigned int max_quantity, unsigned int quantity) {
    struct Item item;
    item.type = type;
    item.max_quantity = max_quantity;
    item.quantity = quantity;
    return item;
}

// Checks if item is at full capacity
int Item_is_full(struct Item* item) {

	if (item->quantity < item->max_quantity) return 0;
	else return 1;

}

// Reset item back to default
void Item_delete(struct Item* item) {
    item->type = ITEM_NONE;
    item->max_quantity = 0;
    item->quantity = 0;
}

