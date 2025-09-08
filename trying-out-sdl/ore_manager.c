#include "ore_manager.h"
#include <stdlib.h>
#include <stdio.h>

static struct Ore_Manager ore_Manager;
static void add_Ore(struct Ore_Manager* self, int x, int y, enum Ore_Type type, int amount);
static void remove_Ore(struct Ore_Manager* self, int x, int y);


struct Ore_Manager* get_Ore_Manager(void) {
    return &ore_Manager;
}

void init_Ore_Manager(void) {
    ore_Manager.ore_List = NULL;
    ore_Manager.add_Ore = add_Ore;
    ore_Manager.remove_Ore = remove_Ore;
}

void destroy_Ore_Manager(void) {
    struct Ore_List* curr = ore_Manager.ore_List;
    while (curr) {
        struct Ore_List* next = curr->next;
        free(curr);
        curr = next;
    }
    ore_Manager.ore_List = NULL;
}

void add_Ore(struct Ore_Manager* self, int x, int y, enum Ore_Type type, int amount) {
    struct Ore_List* new_Ore = (struct Ore_List*)malloc(sizeof(struct Ore_List));
    if (new_Ore == NULL) {
        fprintf(stderr, "Failed to allocate memory for new ore.\n");
        return;
    }
    struct Ore ore = { x, y, type, amount };

    new_Ore->ore = ore;
    new_Ore->next = self->ore_List;
    self->ore_List = new_Ore;
    printf("Added ore of type %d at (%d, %d) with amount %d\n", type, x, y, amount);
    printf("pointer %p \n", self->ore_List);
}

void remove_Ore(struct Ore_Manager* self, int x, int y) {
    struct Ore_List** curr = &self->ore_List;
    while (*curr) {
        if ((*curr)->ore.x == x && (*curr)->ore.y == y) {
            struct Ore_List* tmp = *curr;
            *curr = (*curr)->next;
            free(tmp);
            return;
        }
        curr = &(*curr)->next;
    }
}