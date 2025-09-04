#pragma once

enum OreType {
    IRON,
    COPPER,
    COAL
};

struct Ore {
    int x, y;
    enum OreType type;
    int amount;
};

struct OreList {
    struct Ore ore;
    struct OreList* next;
};

struct OreManager {
    struct OreList* oreList;
    void (*addOre)(struct OreManager* self, int x, int y, enum OreType type, int amount);
    void (*removeOre)(struct OreManager* self, int x, int y);
};

struct OreManager* getOreManager(void);

void initOreManager(void);

void cleanOreManager(void);
