#ifndef RENDERPASS_H
#define RENDERPASS_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern VkRenderPass g_renderpass_main;

void renderpass_create_main(void);

void renderpass_destroy_main(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // RENDERPASS_H
