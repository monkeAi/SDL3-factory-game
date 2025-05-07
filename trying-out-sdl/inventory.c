#include <stdio.h>
#include <stdlib.h>

#include "inventory.h"
#include "constants.h"

// Create and initialize new inventory
struct Inventory* Inventory_create(unsigned int max_slots) {

	// Create struct and allocate memory
	struct Inventory* inv = (struct Inventory*)malloc(sizeof(struct Inventory));
	if (inv == NULL) {
		fprintf(stderr, "Failed to allocate memory for inventory.\n");
		exit(1);
	}
	inv->max_slots = max_slots;
	inv->available_slots = max_slots;
	inv->slots = (struct Item*)calloc(max_slots, sizeof(struct Item)); // Initialize all slots to ITEM_NONE

	return inv;
}


// Push new item in the first free slot, if it finds a half full slot with the same item type it fills it up first
int Inventory_push_item(struct Inventory* inv, struct Item* item) {

	int items_left = item->quantity;

	while (TRUE) {

		// Find first available item slot
		int free_index = Inventory_find_free_slot(inv, item->type);

		// Check if inventory is full
		if (free_index == -1) {
			item->quantity = items_left;
			printf("Inventory full");
			return 1; 
		}

		// 2 Cases	
			// If the slot is empty add the whole item
			// If the slot isnt empty fill it up and check again

		if (inv->slots[free_index].type == ITEM_NONE) {

			item->quantity = items_left;
			inv->slots[free_index] = *item;
			return 0;
		}

		else {

			// Amount of left space in a slot
			int space_left = inv->slots[free_index].max_quantity - inv->slots[free_index].quantity;

			// If there is more free space than required, add and return
			if (space_left >= items_left) {
				inv->slots[free_index].quantity += items_left;
				return 0;
			}
			// Else subtract the used amount and go again
			else {
				inv->slots[free_index].quantity = inv->slots[free_index].max_quantity;
				items_left -= space_left;
			}

			//printf("Space left: %d Items left: %d\n", space_left, items_left);
		}
	}

	// Return 0 if all good
	// Return 1 if inventory full
}

// Returns index of the first available slot for specified item
static int Inventory_find_free_slot(struct Inventory* inv, enum ItemType item_type) {

	// Loop through the inventory slots
	for (int i = 0; i < inv->max_slots; i++) {

		// Find free slot or with the same type and isn't full
		if (inv->slots[i].type == item_type && !Item_is_full(&inv->slots[i])) {

			// return the index of slot
			return i;
		}
		else if (inv->slots[i].type == ITEM_NONE) return i;
	}
	
	// Return -1 if no free slot was found

	return -1;
}

// Returns the slot index of last item type in inventory, returns -1 if item not found
int Inventory_search_item(struct Inventory* inv, enum ItemType item_type) {
	
	// Loop through the inventory slots backwards
	for (int i = inv->max_slots; i >= 0; i--) {

		// Find matching slot
		if (inv->slots[i].type == item_type) {

			// return the index of slot
			return i;
		}
	}

	// Return -1 if item not found
	return -1;
}

// Prints out inventory 
int Inventory_print(struct Inventory* inv) {
	for (int i = 0; i < inv->max_slots; i++) {
		printf("Slot %d: Type: %d, Quantity: %d\n", i, inv->slots[i].type, inv->slots[i].quantity);
	}

}

// Transfers quantity of item from one inventory to another
int Inventory_transfer_item(struct Inventory* from_inv, struct Inventory* to_inv, int from_slot, int quantity) {

	// Check if from_inv slot is empty
	if (from_inv->slots[from_slot].type == ITEM_NONE) return 1;

	//	If desired transfer quantity is -1 or larger than currently stored quantity -> transfer all of it
	if (quantity == -1 || from_inv->slots[from_slot].quantity < quantity) {

		if (Inventory_push_item(to_inv, &from_inv->slots[from_slot])) {
			return 2;  // to_inventory is full
		}

		// Delete item from original inventory
		Item_delete(&from_inv->slots[from_slot]);

	}
	//	If transfer amount is less than currently stored create new item with the desired quantity and send it
	else if (from_inv->slots[from_slot].quantity > quantity) {

		struct Item new_item = Item_create(from_inv->slots[from_slot].type, from_inv->slots[from_slot].max_quantity, quantity);
		
		if (Inventory_push_item(to_inv, &new_item)) {

			from_inv->slots[from_slot].quantity + new_item.quantity;	// Add back the left over quantity
			return 2;  // to_inventory is full
		}

		from_inv->slots[from_slot].quantity -= quantity;
	}

	return 0;
}

// Transfers all items out of inventory and deletes it.
void Inventory_delete();