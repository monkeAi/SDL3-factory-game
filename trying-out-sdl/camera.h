#ifndef CAMERA_H
#define CAMERA_H

struct Camera {
    float x_offset;
    float y_offset;
    float zoom;
};

// Use a global pointer to the main camera
extern struct Camera* mainCamera;

int init_camera();
void update_camera();

#endif
