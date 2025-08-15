#define FALSE 0
#define TRUE 1

#define WINDOW_WIDTH 1200				
#define WINDOW_HEIGHT 900
#define WINDOW_TITLE "Victorio the game"

#define FPS 60
#define FRAME_TARGET_TIME (1000 / FPS)

#define TILE_SIZE 32
#define MAP_WIDTH 40
#define MAP_HEIGHT 40	 


// Terrain generation

#define SEED 27
#define NOISE_FREQ 0.02
#define NOISE_AMP 20

#define TERRAIN_WATER_RANGE 40		// From 0 to 100
#define TERRAIN_SAND_RANGE 50
#define TERRAIN_GRASS_RANGE 70
#define TERRAIN_FORREST_RANGE 80
#define TERRAIN_ROCKY_RANGE 100


// Player

#define PLAYER_MAX_SPEED 300
#define PLAYER_MAX_INV_SIZE 30
#define PLAYER_START_INV_SIZE 10


// Recipes and Crafting

#define MAX_RECIPES 128
#define CRAFT_METHODS 4
#define MAX_CRAFT_QUEUE 100


// Buildings

#define MAX_BUILDINGS 200

#define CRAFTER_MAX_INV_SLOTS 5
#define CRAFTER_WIDTH 3
#define CRAFTER_HEIGHT 3


// GUI
#define MAX_GUI_WINDOWS 5
#define MAX_GUI_FLAGS 5
#define MAX_GUI_CLASS 16
#define MAX_GUI_CLASS_MATHCHES 256

#define GUI_TILE_SIZE 40
#define GUI_INVENTORY_WIDTH 10
#define GUI_INVENTORY_HEIGHT 10
#define GUI_TILE_MARGIN 1
