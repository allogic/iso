glslangValidator -I"." -V -g -Od --target-env vulkan1.3 -o debug/line_renderer.vert.spv debug/line_renderer.vert
glslangValidator -I"." -V -g -Od --target-env vulkan1.3 -o debug/line_renderer.frag.spv debug/line_renderer.frag

glslangValidator -I"." -V -g -Od --target-env vulkan1.3 -o nox/default.vert.spv nox/default.vert
glslangValidator -I"." -V -g -Od --target-env vulkan1.3 -o nox/default.frag.spv nox/default.frag

glslangValidator -I"." -V -g -Od --target-env vulkan1.3 -o vdb/pre_depth.vert.spv vdb/pre_depth.vert
glslangValidator -I"." -V -g -Od --target-env vulkan1.3 -o vdb/pre_depth.frag.spv vdb/pre_depth.frag

glslangValidator -I"." -V -g -Od --target-env vulkan1.3 -o vdb/geom_renderer.task.spv vdb/geom_renderer.task
glslangValidator -I"." -V -g -Od --target-env vulkan1.3 -o vdb/geom_renderer.mesh.spv vdb/geom_renderer.mesh
glslangValidator -I"." -V -g -Od --target-env vulkan1.3 -o vdb/geom_renderer.frag.spv vdb/geom_renderer.frag

glslangValidator -I"." -V -g -Od --target-env vulkan1.3 -o vdb/frustum_culling.comp.spv vdb/frustum_culling.comp
glslangValidator -I"." -V -g -Od --target-env vulkan1.3 -o vdb/mask_generator.comp.spv vdb/mask_generator.comp
glslangValidator -I"." -V -g -Od --target-env vulkan1.3 -o vdb/lod_generator.comp.spv vdb/lod_generator.comp