#include <pch.h>

int32_t vdb_chunk_position_to_index(ivector3_t chunk_position) {
  return (chunk_position.x) +
         (chunk_position.y * CLUSTER_DIM_X) +
         (chunk_position.z * CLUSTER_DIM_X * CLUSTER_DIM_Y);
}
ivector3_t vdb_chunk_index_to_position(int32_t chunk_index) {
  return (ivector3_t){
    chunk_index % CLUSTER_DIM_X,
    (chunk_index / CLUSTER_DIM_X) % CLUSTER_DIM_Y,
    chunk_index / (CLUSTER_DIM_X * CLUSTER_DIM_Y),
  };
}
