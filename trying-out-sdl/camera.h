#ifndef CAMERA_H
#define CAMERA_H

struct Camera {
    int x_offset;
    int y_offset;
    float zoom;
};

// Use a global pointer to the main camera
extern struct Camera* mainCamera;

int init_camera();
void update_camera();

#endif
