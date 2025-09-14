#pragma once
#ifndef OREMANAGER_H
#define OREMANAGER_H

enum OreType {
    ORE_IRON,
    ORE_COPPER,
    ORE_COAL
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
struct OreList* getOre(int world_x, int world_y);
int hasOre(struct OreList** Ores);
int getOres(int world_x, int world_y, int width, int height, struct OreList** Ores);


#endif