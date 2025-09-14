#include "OreManager.h"
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"

static struct OreManager oreManager;
static void addOre(struct OreManager* self, int x, int y, enum OreType type, int amount);
static void removeOre(struct OreManager* self, int x, int y);


struct OreManager* getOreManager(void) {
    return &oreManager;
}

void initOreManager(void) {
    oreManager.oreList = NULL;
    oreManager.addOre = addOre;
    oreManager.removeOre = removeOre;
}

void cleanOreManager(void) {
    struct OreList* curr = oreManager.oreList;
    while (curr) {
        struct OreList* next = curr->next;
        free(curr);
        curr = next;
    }
    oreManager.oreList = NULL;
}

void addOre(struct OreManager* self, int x, int y, enum OreType type, int amount) {
    struct OreList* newOre = (struct OreList*)malloc(sizeof(struct OreList));
    if (newOre == NULL) {
        fprintf(stderr, "Failed to allocate memory for new ore.\n");
        return;
    }
    struct Ore ore = { x, y, type, amount };

    newOre->ore = ore;
    newOre->next = self->oreList;
    self->oreList = newOre;
    //printf("Added ore of type %d at (%d, %d) with amount %d\n", type, x, y, amount);
    //printf("pointer %p \n", self->oreList);
}

void removeOre(struct OreManager* self, int x, int y) {
    struct OreList** curr = &self->oreList;
    while (*curr) {
        if ((*curr)->ore.x == x && (*curr)->ore.y == y) {
            struct OreList* tmp = *curr;
            *curr = (*curr)->next;
            free(tmp);
            return;
        }
        curr = &(*curr)->next;
    }
}

// Check what ore is at the given world cordinate, returns pointer to ore or NULL pointer
struct OreList* getOre(int world_x, int world_y) {

    // Convert world cordinates to tile index
    int selected_cords[2];
    selected_cords[0] = world_x;
    selected_cords[1] = world_y;

    int selected_tile_index[2];
    cordinate_to_index(selected_cords, selected_tile_index);

    struct OreManager* ore_manager = getOreManager();
    struct OreList* curr_ore = ore_manager->oreList;

    struct OreList* result_ore = NULL;

    while (curr_ore->next != NULL) {

        //printf("curr_ore pointer: %x, x: %d, y:%d\n", curr_ore, curr_ore->ore.x, curr_ore->ore.y);/*
        //printf("mouse pos: x: %d, y:%d \n", selected_cords[0], selected_cords[1]);*/

        if (curr_ore->ore.x == selected_tile_index[0] && curr_ore->ore.y == selected_tile_index[1]) {

            result_ore = curr_ore;
            break;
        }

        curr_ore = curr_ore->next;

    }

    return result_ore;
}

// Returns table of ores that are 
struct OreList** getOres(int world_x, int world_y, int width, int height) {

    // Create a table
    // Loop through all coordinates
    // get ore for each
    // return table

}