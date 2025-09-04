#ifndef CAMERA_H
#define CAMERA_H

struct Camera {
    float x_offset;
    float y_offset;
	float width;
	float height;
	float zoom;
	float zoom_aspect_ratio;
};

// Use a global pointer to the main camera
extern struct Camera* mainCamera;

int init_camera();
void update_camera();

#endif
