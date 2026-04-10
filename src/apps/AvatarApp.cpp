#include "AvatarApp.h"
#include "../include/config.h"

AvatarApp::AvatarApp() : m_container(nullptr), m_leftEye(nullptr), m_rightEye(nullptr), m_leftPupil(nullptr), m_rightPupil(nullptr) {
}

AvatarApp::~AvatarApp() {
    hide(); // Garbage Collection Trigger according to Best Practices
}

void AvatarApp::show(lv_obj_t* parent) {
    if (m_container) hide();
    
    // Create full screen container for the app
    m_container = lv_obj_create(parent);
    lv_obj_set_size(m_container, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_color(m_container, lv_color_hex(0x1a1a2e), 0); // Dark Blueish Background
    lv_obj_set_style_border_width(m_container, 0, 0);

    // Draw the two eyes
    drawEye(m_container, m_leftEye, m_leftPupil, -160);
    drawEye(m_container, m_rightEye, m_rightPupil, 160);
}

void AvatarApp::hide() {
    // Delete the root container. LVGL automatically deletes all children recursively.
    if (m_container != nullptr) {
        lv_obj_del(m_container);
        m_container = nullptr;
    }
}

void AvatarApp::drawEye(lv_obj_t* parent, lv_obj_t*& eye, lv_obj_t*& pupil, int x_offset) {
    // Outer Eye
    eye = lv_obj_create(parent);
    lv_obj_set_size(eye, 220, 280);
    lv_obj_align(eye, LV_ALIGN_CENTER, x_offset, 0);
    lv_obj_set_style_bg_color(eye, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_radius(eye, 110, 0); // Fully rounded edges to create an oval
    lv_obj_clear_flag(eye, LV_OBJ_FLAG_SCROLLABLE);

    // Inner Pupil
    pupil = lv_obj_create(eye);
    lv_obj_set_size(pupil, 90, 90);
    lv_obj_align(pupil, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_color(pupil, lv_color_hex(0x0f3460), 0); // Robotic blue pupil
    lv_obj_set_style_radius(pupil, LV_RADIUS_CIRCLE, 0);
}

void AvatarApp::updateGaze(float x, float y) {
    if (!m_container || !m_leftPupil || !m_rightPupil) return;

    // Safety constraint
    if (x < -1.0f) x = -1.0f;
    if (x > 1.0f) x = 1.0f;
    if (y < -1.0f) y = -1.0f;
    if (y > 1.0f) y = 1.0f;

    // Translate coordinates to pixel offsets inside the eye ball limits
    lv_coord_t px = static_cast<lv_coord_t>(x * 60.0f);
    lv_coord_t py = static_cast<lv_coord_t>(y * 90.0f);

    lv_obj_align(m_leftPupil, LV_ALIGN_CENTER, px, py);
    lv_obj_align(m_rightPupil, LV_ALIGN_CENTER, px, py);
}
