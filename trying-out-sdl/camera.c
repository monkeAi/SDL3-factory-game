#include "camera.h"
#include "constants.h"
#include <stdlib.h>
#include "player.h"

struct Camera* mainCamera = NULL;

int init_camera() {
    mainCamera = malloc(sizeof(struct Camera));
    if (mainCamera == NULL) {
        
        return 1;
    }
    mainCamera->x_offset = 0;
    mainCamera->y_offset = 0;
	mainCamera->width = WINDOW_WIDTH;
	mainCamera->height = WINDOW_HEIGHT;
	mainCamera->zoom = 100.0f;
	mainCamera->zoom_aspect_ratio = (float)WINDOW_HEIGHT / (float)WINDOW_WIDTH;

    return 0;
}

// Example: center camera on a player position
void update_camera() {

    mainCamera->x_offset = player->x_pos - mainCamera->width / 2;
	mainCamera->y_offset = player->y_pos - mainCamera->height / 2;
}