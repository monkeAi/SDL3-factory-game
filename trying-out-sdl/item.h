#ifndef ITEM_H
#define ITEM_H

struct Item {
    enum ItemType type;
    unsigned int max_quantity;
    unsigned int quantity;
};


enum ItemType {
    ITEM_NONE,
    ITEM_IRON_ORE,
    ITEM_IRON_PLATE,
    ITEM_IRON_GEAR,
    ITEM_COPPER_ORE,
    ITEM_COPPER_PLATE
};

struct Item Item_create(enum ItemType type, unsigned int max_quantity, unsigned int quantity);
int Item_is_full(struct Item* item);
void Item_delete(struct Item* item);

#endif
