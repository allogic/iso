glslangValidator -I"." -V -g -Od --target-env vulkan1.3 -o debug/line_renderer.vert.spv debug/line_renderer.vert
glslangValidator -I"." -V -g -Od --target-env vulkan1.3 -o debug/line_renderer.frag.spv debug/line_renderer.frag

glslangValidator -I"." -V -g -Od --target-env vulkan1.3 -o vdb/chunk_renderer.vert.spv vdb/chunk_renderer.vert
glslangValidator -I"." -V -g -Od --target-env vulkan1.3 -o vdb/chunk_renderer.frag.spv vdb/chunk_renderer.frag

glslangValidator -I"." -V -g -Od --target-env vulkan1.3 -o vdb/dda_tracer.comp.spv vdb/dda_tracer.comp
glslangValidator -I"." -V -g -Od --target-env vulkan1.3 -o vdb/voxel_placer.comp.spv vdb/voxel_placer.comp
glslangValidator -I"." -V -g -Od --target-env vulkan1.3 -o vdb/world_generator.comp.spv vdb/world_generator.comp
glslangValidator -I"." -V -g -Od --target-env vulkan1.3 -o vdb/mask_generator.comp.spv vdb/mask_generator.comp
glslangValidator -I"." -V -g -Od --target-env vulkan1.3 -o vdb/greedy_mesher.comp.spv vdb/greedy_mesher.comp