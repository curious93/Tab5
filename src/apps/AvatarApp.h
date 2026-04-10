#pragma once

#include <lvgl.h>

class AvatarApp {
public:
    AvatarApp();
    ~AvatarApp();

    // Lifecylce Methods
    void show(lv_obj_t* parent);
    void hide();

    // Update Avatar View. X and Y range is -1.0f to 1.0f
    void updateGaze(float x, float y);

private:
    lv_obj_t* m_container;
    lv_obj_t* m_leftEye;
    lv_obj_t* m_rightEye;
    lv_obj_t* m_leftPupil;
    lv_obj_t* m_rightPupil;

    void drawEye(lv_obj_t* parent, lv_obj_t*& eye, lv_obj_t*& pupil, int x_offset);
};
