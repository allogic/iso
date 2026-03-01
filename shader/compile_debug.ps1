glslangValidator -I"." -V -g -Od --target-env vulkan1.3 -o debug/line_renderer.vert.spv debug/line_renderer.vert
glslangValidator -I"." -V -g -Od --target-env vulkan1.3 -o debug/line_renderer.frag.spv debug/line_renderer.frag

glslangValidator -I"." -V -g -Od --target-env vulkan1.3 -o dvdb/renderer.rchit.spv dvdb/renderer.rchit
glslangValidator -I"." -V -g -Od --target-env vulkan1.3 -o dvdb/renderer.rgen.spv dvdb/renderer.rgen
glslangValidator -I"." -V -g -Od --target-env vulkan1.3 -o dvdb/renderer.rint.spv dvdb/renderer.rint
glslangValidator -I"." -V -g -Od --target-env vulkan1.3 -o dvdb/renderer.rmiss.spv dvdb/renderer.rmiss
glslangValidator -I"." -V -g -Od --target-env vulkan1.3 -o dvdb/voxel_simulator.comp.spv dvdb/voxel_simulator.comp

glslangValidator -I"." -V -g -Od --target-env vulkan1.3 -o svdb/renderer.vert.spv svdb/renderer.vert
glslangValidator -I"." -V -g -Od --target-env vulkan1.3 -o svdb/renderer.frag.spv svdb/renderer.frag
glslangValidator -I"." -V -g -Od --target-env vulkan1.3 -o svdb/voxel_selector.comp.spv svdb/voxel_selector.comp
glslangValidator -I"." -V -g -Od --target-env vulkan1.3 -o svdb/voxel_placer.comp.spv svdb/voxel_placer.comp
glslangValidator -I"." -V -g -Od --target-env vulkan1.3 -o svdb/world_generator.comp.spv svdb/world_generator.comp
glslangValidator -I"." -V -g -Od --target-env vulkan1.3 -o svdb/mask_generator.comp.spv svdb/mask_generator.comp
glslangValidator -I"." -V -g -Od --target-env vulkan1.3 -o svdb/mesh_generator.comp.spv svdb/mesh_generator.comp