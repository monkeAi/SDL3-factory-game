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