glslangValidator -I"." -V --target-env vulkan1.3 -o debug/line_renderer.vert.spv debug/line_renderer.vert
glslangValidator -I"." -V --target-env vulkan1.3 -o debug/line_renderer.frag.spv debug/line_renderer.frag

glslangValidator -I"." -V --target-env vulkan1.3 -o vdb/geom_renderer.task.spv vdb/geom_renderer.task
glslangValidator -I"." -V --target-env vulkan1.3 -o vdb/geom_renderer.mesh.spv vdb/geom_renderer.mesh
glslangValidator -I"." -V --target-env vulkan1.3 -o vdb/geom_renderer.frag.spv vdb/geom_renderer.frag

glslangValidator -I"." -V --target-env vulkan1.3 -o vdb/world_generator.comp.spv vdb/world_generator.comp