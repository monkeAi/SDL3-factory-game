#pragma once

enum Ore_Type {
    IRON,
    COPPER,
    COAL
};

struct Ore {
    int x, y;
    enum Ore_Type type;
    int amount;
};

struct Ore_List {
    struct Ore ore;
    struct Ore_List* next;
};

struct Ore_Manager {
    struct Ore_List* ore_List;
    void (*add_Ore)(struct Ore_Manager* self, int x, int y, enum Ore_Type type, int amount);
    void (*remove_Ore)(struct Ore_Manager* self, int x, int y);
};

struct Ore_Manager* get_Ore_Manager(void);

void init_Ore_Manager(void);

void destroy_Ore_Manager(void);
