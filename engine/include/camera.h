#ifndef CAMERA_H
#define CAMERA_H

typedef enum frustum_plane_t {
  FRUSTUM_PLANE_LEFT = 0,
  FRUSTUM_PLANE_RIGHT,
  FRUSTUM_PLANE_BOTTOM,
  FRUSTUM_PLANE_TOP,
  FRUSTUM_PLANE_NEAR,
  FRUSTUM_PLANE_FAR,
  FRUSTUM_PLANE_COUNT,
} frustum_plane_t;
typedef enum frustum_corner_t {
  FRUSTUM_CORNER_NEAR_V0 = 0,
  FRUSTUM_CORNER_NEAR_V1,
  FRUSTUM_CORNER_NEAR_V2,
  FRUSTUM_CORNER_NEAR_V3,
  FRUSTUM_CORNER_FAR_V0,
  FRUSTUM_CORNER_FAR_V1,
  FRUSTUM_CORNER_FAR_V2,
  FRUSTUM_CORNER_FAR_V3,
  FRUSTUM_CORNER_COUNT,
} frustum_corner_t;

typedef struct camera_t {
  int8_t is_debug_enabled;
  int8_t is_orthographic;
  float fov;
  float zoom;
  float persp_near_z;
  float persp_far_z;
  float ortho_near_z;
  float ortho_far_z;
  vector3_t eye;
  vector3_t center;
  vector3_t up;
  matrix4_t view;
  matrix4_t projection;
  matrix4_t view_projection;
  matrix4_t view_projection_inv;
  plane_t frustum_plane[FRUSTUM_PLANE_COUNT];
  vector3_t frustum_corner[FRUSTUM_CORNER_COUNT];
} camera_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void camera_update(camera_t *camera, transform_t *transform);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CAMERA_H
