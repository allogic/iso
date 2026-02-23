#ifndef VDB_H
#define VDB_H

#define CLUSTER_DIM_X (10)
#define CLUSTER_DIM_Y (3)
#define CLUSTER_DIM_Z (10)

#define CHUNK_SIZE (32)
#define CHUNK_COUNT (100)

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

int32_t vdb_chunk_position_to_index(ivector3_t chunk_position);
ivector3_t vdb_chunk_index_to_position(int32_t chunk_index);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // VDB_H
