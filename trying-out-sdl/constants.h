#define FALSE 0
#define TRUE 1

#define WINDOW_WIDTH 800				
#define WINDOW_HEIGHT 600
#define WINDOW_TITLE "Victorio the game"

#define FPS 60
#define FRAME_TARGET_TIME (1000 / FPS)

#define TILE_SIZE 32
#define MAP_WIDTH 40
#define MAP_HEIGHT 40	 


// Terrain generation

#define SEED 15
#define NOISE_FREQ 0.02
#define NOISE_AMP 20

#define TERRAIN_WATER_RANGE 40		// From 0 to 100
#define TERRAIN_SAND_RANGE 50
#define TERRAIN_GRASS_RANGE 70
#define TERRAIN_FORREST_RANGE 80
#define TERRAIN_ROCKY_RANGE 100


// Player

#define PLAYER_MAX_SPEED 300


// Recipes and Crafting

#define MAX_RECIPES 128
#define CRAFT_METHODS 4