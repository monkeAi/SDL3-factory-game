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

    return 0;
}

// Example: center camera on a player position
void update_camera() {

    mainCamera->x_offset = player->x_pos;
    mainCamera->y_offset = player->y_pos;
}