glslangValidator -I"." -V --target-env vulkan1.3 -o debug/line_renderer.vert.spv debug/line_renderer.vert
glslangValidator -I"." -V --target-env vulkan1.3 -o debug/line_renderer.frag.spv debug/line_renderer.frag

glslangValidator -I"." -V --target-env vulkan1.3 -o vdb/iso_renderer.task.spv vdb/iso_renderer.task
glslangValidator -I"." -V --target-env vulkan1.3 -o vdb/iso_renderer.mesh.spv vdb/iso_renderer.mesh
glslangValidator -I"." -V --target-env vulkan1.3 -o vdb/iso_renderer.frag.spv vdb/iso_renderer.frag

glslangValidator -I"." -V --target-env vulkan1.3 -o vdb/dda_trace.comp.spv vdb/dda_trace.comp
glslangValidator -I"." -V --target-env vulkan1.3 -o vdb/world_generator.comp.spv vdb/world_generator.comp