#include "item.h"


struct Item Item_create(enum ItemType type, unsigned int max_quantity, unsigned int quantity) {
    struct Item item;
    item.type = type;
    item.max_quantity = max_quantity;
    item.quantity = quantity;
    item.color = Item_get_color(type);
    return item;
}

// Checks if item is at full capacity
int Item_is_full(struct Item* item) {

	if (item->quantity < item->max_quantity) return 0;
	else return 1;

}

int Item_get_color(enum ItemType type) {
    switch (type)
    {
    case ITEM_NONE:
        return 0xFFFFFFFF;
        break;
    case ITEM_IRON_ORE:
        return 0x0b4b80FF;
        break;
    case ITEM_IRON_PLATE:
        return 0xa3cdf0FF;
        break;
    case ITEM_IRON_GEAR:
        return 0xabbcc9FF;
        break;
    case ITEM_COPPER_ORE:
        return 0x964520FF;
        break;
    case ITEM_COPPER_PLATE:
        return 0xed8b5fFF;
        break;
    default:
        return 0xFFFFFFFF;
        break;
    }
}

// Reset item back to default
void Item_delete(struct Item* item) {
    item->type = ITEM_NONE;
    item->max_quantity = 0;
    item->quantity = 0;
}

