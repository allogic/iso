#include <pch.h>

static void vdb_create_terrain_layer_buffer(void);
static void vdb_create_cluster_info_buffer(void);
static void vdb_create_chunk_info_buffer(void);
static void vdb_create_chunk_mask_buffer(void);
static void vdb_create_chunk_index_buffer(void);

vdb_t g_vdb = {0};

void vdb_create(void) {
  g_vdb.rsort_key = (uint32_t *)HEAP_ALLOC(sizeof(uint32_t) * VDB_CHUNK_COUNT, 0, 0);
  g_vdb.rsort_idx = (uint32_t *)HEAP_ALLOC(sizeof(uint32_t) * VDB_CHUNK_COUNT, 0, 0);
  g_vdb.rsort_tmp = (uint32_t *)HEAP_ALLOC(sizeof(uint32_t) * VDB_CHUNK_COUNT, 0, 0);

  vdb_create_terrain_layer_buffer();
  vdb_create_cluster_info_buffer();
  vdb_create_chunk_info_buffer();
  vdb_create_chunk_mask_buffer();
  vdb_create_chunk_index_buffer();
}
void vdb_sort(transform_t *transform) {
  vector3_t world_position = transform->world_position;

  for (uint32_t z = 0; z < VDB_CLUSTER_DIM_Z; z++) {
    for (uint32_t y = 0; y < VDB_CLUSTER_DIM_Y; y++) {
      for (uint32_t x = 0; x < VDB_CLUSTER_DIM_X; x++) {

        uint32_t i = (x) + (y * VDB_CLUSTER_DIM_X) + (z * VDB_CLUSTER_DIM_X * VDB_CLUSTER_DIM_Y);

        float wx = (x + 0.5F) * VDB_CHUNK_SIZE;
        float wy = (y + 0.5F) * VDB_CHUNK_SIZE;
        float wz = (z + 0.5F) * VDB_CHUNK_SIZE;

        float dx = wx - world_position.x;
        float dy = wy - world_position.y;
        float dz = wz - world_position.z;
        float d_sq = dx * dx + dy * dy + dz * dz;

        g_vdb.rsort_key[i] = (uint32_t)d_sq;
      }
    }
  }

  for (uint32_t i = 0; i < VDB_CHUNK_COUNT; i++) {
    g_vdb.rsort_idx[i] = i;
  }

  for (int32_t pass = 0; pass < 4; pass++) {

    int32_t shift = pass * 8;

    uint32_t count[256] = {0};

    for (uint32_t i = 0; i < VDB_CHUNK_COUNT; i++) {
      uint8_t digit = (g_vdb.rsort_key[g_vdb.rsort_idx[i]] >> shift) & 0xFF;
      count[digit]++;
    }

    for (int32_t i = 1; i < 256; i++) {
      count[i] += count[i - 1];
    }

    for (int32_t i = (VDB_CHUNK_COUNT - 1); i >= 0; i--) {
      uint8_t digit = (g_vdb.rsort_key[g_vdb.rsort_idx[i]] >> shift) & 0xFF;
      g_vdb.rsort_tmp[--count[digit]] = g_vdb.rsort_idx[i];
    }

    uint32_t *t = g_vdb.rsort_idx;
    g_vdb.rsort_idx = g_vdb.rsort_tmp;
    g_vdb.rsort_tmp = t;
  }

  memcpy(g_vdb.chunk_index_buffer.mapped_memory, g_vdb.rsort_idx, sizeof(uint32_t) * VDB_CHUNK_COUNT);
}
void vdb_destroy(void) {
  buffer_destroy(&g_vdb.terrain_layer_buffer);
  buffer_destroy(&g_vdb.cluster_info_buffer);
  buffer_destroy(&g_vdb.chunk_info_buffer);
  buffer_destroy(&g_vdb.chunk_mask_buffer);
  buffer_destroy(&g_vdb.chunk_index_buffer);

  HEAP_FREE(g_vdb.rsort_key);
  HEAP_FREE(g_vdb.rsort_idx);
  HEAP_FREE(g_vdb.rsort_tmp);
}

int32_t vdb_chunk_position_to_index(ivector3_t chunk_position) {
  return (chunk_position.x) +
         (chunk_position.y * VDB_CLUSTER_DIM_X) +
         (chunk_position.z * VDB_CLUSTER_DIM_X * VDB_CLUSTER_DIM_Y);
}
ivector3_t vdb_chunk_index_to_position(int32_t chunk_index) {
  return (ivector3_t){
    chunk_index % VDB_CLUSTER_DIM_X,
    (chunk_index / VDB_CLUSTER_DIM_X) % VDB_CLUSTER_DIM_Y,
    chunk_index / (VDB_CLUSTER_DIM_X * VDB_CLUSTER_DIM_Y),
  };
}

static void vdb_create_terrain_layer_buffer(void) {
  g_vdb.terrain_layer_buffer = buffer_create_uniform(0, sizeof(vdb_terrain_layer_t) * VDB_TERRAIN_LAYER_COUNT);
}
static void vdb_create_cluster_info_buffer(void) {
  vdb_cluster_info_t vdb_cluster_info = {
    .cluster_dim = {
      .x = VDB_CLUSTER_DIM_X,
      .y = VDB_CLUSTER_DIM_Y,
      .z = VDB_CLUSTER_DIM_Z,
    },
  };

  g_vdb.cluster_info_buffer = buffer_create_uniform(&vdb_cluster_info, sizeof(vdb_cluster_info_t));
}
static void vdb_create_chunk_info_buffer(void) {
  vdb_chunk_info_t *chunk_info = (vdb_chunk_info_t *)HEAP_ALLOC(sizeof(vdb_chunk_info_t) * VDB_CHUNK_COUNT, 1, 0);

  int32_t chunk_index = 0;
  int32_t chunk_count = VDB_CHUNK_COUNT;

  while (chunk_index < chunk_count) {

    ivector3_t chunk_position = vdb_chunk_index_to_position(chunk_index);

    vector3_t aabb_min = {
      (float)chunk_position.x * VDB_CHUNK_SIZE,
      (float)chunk_position.y * VDB_CHUNK_SIZE,
      (float)chunk_position.z * VDB_CHUNK_SIZE,
    };
    vector3_t aabb_max = {
      aabb_min.x + VDB_CHUNK_SIZE,
      aabb_min.y + VDB_CHUNK_SIZE,
      aabb_min.z + VDB_CHUNK_SIZE,
    };

    chunk_info[chunk_index].chunk_position = chunk_position;
    chunk_info[chunk_index].lod = 0;
    chunk_info[chunk_index].aabb_min = aabb_min;
    chunk_info[chunk_index].aabb_max = aabb_max;

    chunk_index++;
  }

  g_vdb.chunk_info_buffer = buffer_create_storage(chunk_info, sizeof(vdb_chunk_info_t) * VDB_CHUNK_COUNT);

  HEAP_FREE(chunk_info);
}
static void vdb_create_chunk_mask_buffer(void) {
  g_vdb.chunk_mask_buffer = buffer_create_storage(0, sizeof(vdb_chunk_mask_t) * VDB_CHUNK_COUNT);
}
static void vdb_create_chunk_index_buffer(void) {
  g_vdb.chunk_index_buffer = buffer_create_storage_coherent(0, sizeof(uint32_t) * VDB_CHUNK_COUNT);
}
