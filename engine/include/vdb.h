#ifndef VDB_H
#define VDB_H

// TODO: Refactor all "static_vdb" and "dynamic_vdb" to just "xvdb_"

// TODO: add vkGetBufferDeviceAddress to buffer_t directly..

// TODO: Implement hardware raytracing voxel renderer!
// TODO: Implement augmented vertex block descent (AVBD) collision solver!
// TODO: Finally implement sparse textures!

#define STATIC_VDB_DIM_X (1)
#define STATIC_VDB_DIM_Y (1)
#define STATIC_VDB_DIM_Z (1)

#define STATIC_VDB_CHUNK_SIZE (32)
#define STATIC_VDB_CHUNK_COUNT (1)

#define DYNAMIC_VDB_DIM_X (4)
#define DYNAMIC_VDB_DIM_Y (4)
#define DYNAMIC_VDB_DIM_Z (4)

#define DYNAMIC_VDB_CHUNK_SIZE (8)
#define DYNAMIC_VDB_CHUNK_COUNT (64)

typedef struct static_vdb_chunk_vertex_t {
  vector4_t position;
  vector4_t color;
  vector4_t uv;
  uint32_t atlas_id;
  uint32_t reserved0;
  uint32_t reserved1;
  uint32_t reserved2;
} static_vdb_chunk_vertex_t;

STATIC_ASSERT(ALIGNOF(static_vdb_chunk_vertex_t) == 4);

typedef uint32_t static_vdb_chunk_index_t;

typedef struct static_vdb_world_generator_push_constant_t {
  ivector3_t chunk_position;
  uint32_t chunk_index;
  uint32_t stage;
  uint32_t reserved0;
  uint32_t reserved1;
  uint32_t reserved2;
} static_vdb_world_generator_push_constant_t;
typedef struct static_vdb_mask_generator_push_constant_t {
  ivector3_t chunk_position;
  uint32_t chunk_index;
} static_vdb_mask_generator_push_constant_t;
typedef struct static_vdb_mesh_generator_push_constant_t {
  ivector3_t chunk_position;
  uint32_t chunk_index;
} static_vdb_mesh_generator_push_constant_t;
typedef struct static_vdb_renderer_push_constant_t {
  ivector3_t chunk_position;
  uint32_t chunk_index;
} static_vdb_renderer_push_constant_t;

STATIC_ASSERT(ALIGNOF(static_vdb_world_generator_push_constant_t) == 4);
STATIC_ASSERT(ALIGNOF(static_vdb_mask_generator_push_constant_t) == 4);
STATIC_ASSERT(ALIGNOF(static_vdb_mesh_generator_push_constant_t) == 4);
STATIC_ASSERT(ALIGNOF(static_vdb_renderer_push_constant_t) == 4);

typedef struct static_vdb_chunk_info_t {
  uint32_t is_dirty;
  uint32_t vertex_count;
  uint32_t index_count;
} static_vdb_chunk_info_t;
typedef struct dynamic_vdb_chunk_info_t {
  uint32_t is_dirty;
} dynamic_vdb_chunk_info_t;

STATIC_ASSERT(ALIGNOF(static_vdb_chunk_info_t) == 4);
STATIC_ASSERT(ALIGNOF(dynamic_vdb_chunk_info_t) == 4);

typedef struct static_vdb_chunk_mask_t {
  uint32_t any_px_faces;
  uint32_t any_py_faces;
  uint32_t any_pz_faces;
  uint32_t any_nx_faces;
  uint32_t any_ny_faces;
  uint32_t any_nz_faces;
  uint32_t opaque_px_mask[STATIC_VDB_CHUNK_SIZE * STATIC_VDB_CHUNK_SIZE];
  uint32_t opaque_py_mask[STATIC_VDB_CHUNK_SIZE * STATIC_VDB_CHUNK_SIZE];
  uint32_t opaque_pz_mask[STATIC_VDB_CHUNK_SIZE * STATIC_VDB_CHUNK_SIZE];
  uint32_t opaque_nx_mask[STATIC_VDB_CHUNK_SIZE * STATIC_VDB_CHUNK_SIZE];
  uint32_t opaque_ny_mask[STATIC_VDB_CHUNK_SIZE * STATIC_VDB_CHUNK_SIZE];
  uint32_t opaque_nz_mask[STATIC_VDB_CHUNK_SIZE * STATIC_VDB_CHUNK_SIZE];
} static_vdb_chunk_mask_t;

STATIC_ASSERT(ALIGNOF(static_vdb_chunk_mask_t) == 4);

typedef struct static_vdb_t {
  image_t *chunk_voxel_image;
  buffer_t chunk_info_buffer;
  buffer_t chunk_mask_buffer;
  buffer_t *chunk_vertex_buffer;
  buffer_t *chunk_index_buffer;
  static_vdb_chunk_info_t *chunk_info;
  VkDescriptorImageInfo *chunk_voxel_descriptor_image_info;
  VkDescriptorBufferInfo chunk_info_descriptor_buffer_info;
  VkDescriptorBufferInfo chunk_mask_descriptor_buffer_info;
  VkDescriptorBufferInfo *chunk_vertex_descriptor_buffer_info;
  VkDescriptorBufferInfo *chunk_index_descriptor_buffer_info;
} static_vdb_t;

typedef struct dynamic_vdb_t {
  image_t *curr_chunk_voxel_image;
  image_t *next_chunk_voxel_image;
  buffer_t chunk_info_buffer;
  buffer_t aabb_buffer;
  buffer_t instance_buffer;
  buffer_t blas_buffer;
  buffer_t tlas_buffer;
  buffer_t blas_scratch_buffer;
  buffer_t tlas_scratch_buffer;
  dynamic_vdb_chunk_info_t *chunk_info;
  VkDescriptorImageInfo *curr_chunk_voxel_descriptor_image_info;
  VkDescriptorImageInfo *next_chunk_voxel_descriptor_image_info;
  VkDescriptorBufferInfo chunk_info_descriptor_buffer_info;
  // TODO: make BLAS's dynamic..
  uint32_t geometry_count;
  uint32_t primitive_count;
  VkAccelerationStructureGeometryKHR blas_geometry;
  VkAccelerationStructureBuildGeometryInfoKHR blas_build_geometry_info; // TODO: make queries in a separate function..
  VkAccelerationStructureBuildSizesInfoKHR blas_build_sizes_info;
  VkAccelerationStructureKHR blas;
  VkWriteDescriptorSetAccelerationStructureKHR blas_write_descriptor_set;
  // TODO: make TLAS's dynamic..
  uint32_t instance_count;
  VkAccelerationStructureGeometryKHR tlas_geometry;
  VkAccelerationStructureBuildGeometryInfoKHR tlas_build_geometry_info;
  VkAccelerationStructureBuildSizesInfoKHR tlas_build_sizes_info; // TODO: make queries in a separate function..
  VkAccelerationStructureKHR tlas;
  VkWriteDescriptorSetAccelerationStructureKHR tlas_write_descriptor_set;
} dynamic_vdb_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern static_vdb_t g_svdb;

extern dynamic_vdb_t g_dvdb;

void static_vdb_create(void);
void static_vdb_build(void);
void static_vdb_draw(void);
void static_vdb_debug(void);
void static_vdb_destroy(void);

uint32_t static_vdb_chunk_position_to_index(ivector3_t chunk_position);
ivector3_t static_vdb_chunk_index_to_position(uint32_t chunk_index);

void dynamic_vdb_create(void);
void dynamic_vdb_draw(void);
void dynamic_vdb_debug(void);
void dynamic_vdb_destroy(void);

uint32_t dynamic_vdb_chunk_position_to_index(ivector3_t chunk_position);
ivector3_t dynamic_vdb_chunk_index_to_position(uint32_t chunk_index);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // VDB_H
