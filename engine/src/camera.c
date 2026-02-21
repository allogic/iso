#include <pch.h>

void camera_update(camera_t *camera, transform_t *transform) {
  float aspect_ratio = (float)g_window.window_width / (float)g_window.window_height;

  camera->eye = transform->world_position;
  camera->center = vector3_add(transform->world_position, transform_local_front(transform));
  camera->up = vector3_down();
  camera->view = matrix4_look_at(camera->eye, camera->center, camera->up);

  if (camera->is_orthographic) {

    float half_width = camera->zoom * aspect_ratio;
    float half_height = camera->zoom;

    camera->projection = matrix4_ortho(-half_width, half_width, -half_height, half_height, camera->ortho_near_z, camera->ortho_far_z);

  } else {

    camera->projection = matrix4_persp(camera->fov, aspect_ratio, camera->persp_near_z, camera->persp_far_z);
  }

  camera->view_projection = matrix4_mul(camera->view, camera->projection);
  camera->view_projection_inv = matrix4_inverse(camera->view_projection);

  // TODO: do this continuously..
  if (window_is_keyboard_key_held(KEYBOARD_KEY_SPACE)) {

    vector4_t c0 = {camera->view_projection.m00, camera->view_projection.m10, camera->view_projection.m20, camera->view_projection.m30};
    vector4_t c1 = {camera->view_projection.m01, camera->view_projection.m11, camera->view_projection.m21, camera->view_projection.m31};
    vector4_t c2 = {camera->view_projection.m02, camera->view_projection.m12, camera->view_projection.m22, camera->view_projection.m32};
    vector4_t c3 = {camera->view_projection.m03, camera->view_projection.m13, camera->view_projection.m23, camera->view_projection.m33};

    vector4_t lp = vector4_add(c3, c0);
    vector4_t rp = vector4_sub(c3, c0);
    vector4_t bp = vector4_add(c3, c1);
    vector4_t tp = vector4_sub(c3, c1);
    vector4_t np = vector4_add(c3, c2);
    vector4_t fp = vector4_sub(c3, c2);

    float lp_length_inv = 1.0F / sqrtf(lp.x * lp.x + lp.y * lp.y + lp.z * lp.z);
    float rp_length_inv = 1.0F / sqrtf(rp.x * rp.x + rp.y * rp.y + rp.z * rp.z);
    float bp_length_inv = 1.0F / sqrtf(bp.x * bp.x + bp.y * bp.y + bp.z * bp.z);
    float tp_length_inv = 1.0F / sqrtf(tp.x * tp.x + tp.y * tp.y + tp.z * tp.z);
    float np_length_inv = 1.0F / sqrtf(np.x * np.x + np.y * np.y + np.z * np.z);
    float fp_length_inv = 1.0F / sqrtf(fp.x * fp.x + fp.y * fp.y + fp.z * fp.z);

    lp = vector4_muls(lp, lp_length_inv);
    rp = vector4_muls(rp, rp_length_inv);
    bp = vector4_muls(bp, bp_length_inv);
    tp = vector4_muls(tp, tp_length_inv);
    np = vector4_muls(np, np_length_inv);
    fp = vector4_muls(fp, fp_length_inv);

    camera->frustum_plane[FRUSTUM_PLANE_LEFT] = (plane_t){lp.x, lp.y, lp.z, lp.w};
    camera->frustum_plane[FRUSTUM_PLANE_RIGHT] = (plane_t){rp.x, rp.y, rp.z, rp.w};
    camera->frustum_plane[FRUSTUM_PLANE_BOTTOM] = (plane_t){bp.x, bp.y, bp.z, bp.w};
    camera->frustum_plane[FRUSTUM_PLANE_TOP] = (plane_t){tp.x, tp.y, tp.z, tp.w};
    camera->frustum_plane[FRUSTUM_PLANE_NEAR] = (plane_t){np.x, np.y, np.z, np.w};
    camera->frustum_plane[FRUSTUM_PLANE_FAR] = (plane_t){fp.x, fp.y, fp.z, fp.w};

    plane_intersect_plane3(camera->frustum_plane[FRUSTUM_PLANE_NEAR], camera->frustum_plane[FRUSTUM_PLANE_LEFT], camera->frustum_plane[FRUSTUM_PLANE_TOP], &camera->frustum_corner[FRUSTUM_CORNER_NEAR_V0]);
    plane_intersect_plane3(camera->frustum_plane[FRUSTUM_PLANE_NEAR], camera->frustum_plane[FRUSTUM_PLANE_RIGHT], camera->frustum_plane[FRUSTUM_PLANE_TOP], &camera->frustum_corner[FRUSTUM_CORNER_NEAR_V1]);
    plane_intersect_plane3(camera->frustum_plane[FRUSTUM_PLANE_NEAR], camera->frustum_plane[FRUSTUM_PLANE_RIGHT], camera->frustum_plane[FRUSTUM_PLANE_BOTTOM], &camera->frustum_corner[FRUSTUM_CORNER_NEAR_V2]);
    plane_intersect_plane3(camera->frustum_plane[FRUSTUM_PLANE_NEAR], camera->frustum_plane[FRUSTUM_PLANE_LEFT], camera->frustum_plane[FRUSTUM_PLANE_BOTTOM], &camera->frustum_corner[FRUSTUM_CORNER_NEAR_V3]);
    plane_intersect_plane3(camera->frustum_plane[FRUSTUM_PLANE_FAR], camera->frustum_plane[FRUSTUM_PLANE_LEFT], camera->frustum_plane[FRUSTUM_PLANE_TOP], &camera->frustum_corner[FRUSTUM_CORNER_FAR_V0]);
    plane_intersect_plane3(camera->frustum_plane[FRUSTUM_PLANE_FAR], camera->frustum_plane[FRUSTUM_PLANE_RIGHT], camera->frustum_plane[FRUSTUM_PLANE_TOP], &camera->frustum_corner[FRUSTUM_CORNER_FAR_V1]);
    plane_intersect_plane3(camera->frustum_plane[FRUSTUM_PLANE_FAR], camera->frustum_plane[FRUSTUM_PLANE_RIGHT], camera->frustum_plane[FRUSTUM_PLANE_BOTTOM], &camera->frustum_corner[FRUSTUM_CORNER_FAR_V2]);
    plane_intersect_plane3(camera->frustum_plane[FRUSTUM_PLANE_FAR], camera->frustum_plane[FRUSTUM_PLANE_LEFT], camera->frustum_plane[FRUSTUM_PLANE_BOTTOM], &camera->frustum_corner[FRUSTUM_CORNER_FAR_V3]);
  }

  if (camera->is_debug_enabled) {

    vector4_t color = {1.0F, 1.0F, 1.0F, 1.0F};

    renderer_draw_debug_line(camera->frustum_corner[FRUSTUM_CORNER_NEAR_V0], camera->frustum_corner[FRUSTUM_CORNER_NEAR_V1], color);
    renderer_draw_debug_line(camera->frustum_corner[FRUSTUM_CORNER_NEAR_V1], camera->frustum_corner[FRUSTUM_CORNER_NEAR_V2], color);
    renderer_draw_debug_line(camera->frustum_corner[FRUSTUM_CORNER_NEAR_V2], camera->frustum_corner[FRUSTUM_CORNER_NEAR_V3], color);
    renderer_draw_debug_line(camera->frustum_corner[FRUSTUM_CORNER_NEAR_V3], camera->frustum_corner[FRUSTUM_CORNER_NEAR_V0], color);

    // renderer_draw_debug_line(camera->frustum_corner[FRUSTUM_CORNER_FAR_V0], camera->frustum_corner[FRUSTUM_CORNER_FAR_V1], color);
    // renderer_draw_debug_line(camera->frustum_corner[FRUSTUM_CORNER_FAR_V1], camera->frustum_corner[FRUSTUM_CORNER_FAR_V2], color);
    // renderer_draw_debug_line(camera->frustum_corner[FRUSTUM_CORNER_FAR_V2], camera->frustum_corner[FRUSTUM_CORNER_FAR_V3], color);
    // renderer_draw_debug_line(camera->frustum_corner[FRUSTUM_CORNER_FAR_V3], camera->frustum_corner[FRUSTUM_CORNER_FAR_V0], color);

    renderer_draw_debug_line(camera->frustum_corner[FRUSTUM_CORNER_NEAR_V0], camera->frustum_corner[FRUSTUM_CORNER_FAR_V0], color);
    renderer_draw_debug_line(camera->frustum_corner[FRUSTUM_CORNER_NEAR_V1], camera->frustum_corner[FRUSTUM_CORNER_FAR_V1], color);
    renderer_draw_debug_line(camera->frustum_corner[FRUSTUM_CORNER_NEAR_V2], camera->frustum_corner[FRUSTUM_CORNER_FAR_V2], color);
    renderer_draw_debug_line(camera->frustum_corner[FRUSTUM_CORNER_NEAR_V3], camera->frustum_corner[FRUSTUM_CORNER_FAR_V3], color);
  }
}
