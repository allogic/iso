#ifndef PCH_H
#define PCH_H

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <float.h>
#include <time.h>
#include <math.h>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include <macros.h>
#include <fwd.h>
#include <heap.h>

#include <math/constant.h>
#include <math/misc.h>
#include <math/vector2.h>
#include <math/vector3.h>
#include <math/vector4.h>
#include <math/ivector2.h>
#include <math/ivector3.h>
#include <math/ivector4.h>
#include <math/quaternion.h>
#include <math/matrix4.h>

#include <physic/plane.h>
#include <physic/ray.h>

#include <transform.h>
#include <camera.h>

#include <fsutil.h>
#include <imgutil.h>
#include <vkutil.h>

#include <pipeline.h>
#include <buffer.h>
#include <image.h>
#include <dbgui.h>
#include <swapchain.h>
#include <framebuffer.h>
#include <player.h>
#include <renderer.h>
#include <renderpass.h>
#include <window.h>

#endif // PCH_H
