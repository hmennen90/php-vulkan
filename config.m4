dnl config.m4 for extension vulkan

PHP_ARG_WITH([vulkan],
  [for Vulkan support],
  [AS_HELP_STRING([--with-vulkan],
    [Include Vulkan support])])

if test "$PHP_VULKAN" != "no"; then

  dnl Check for Vulkan headers
  SEARCH_PATH="/opt/homebrew /usr/local /usr"
  SEARCH_FOR="include/vulkan/vulkan.h"

  AC_MSG_CHECKING([for Vulkan SDK])

  for i in $PHP_VULKAN $SEARCH_PATH; do
    if test -r "$i/$SEARCH_FOR"; then
      VULKAN_DIR=$i
      AC_MSG_RESULT([found in $VULKAN_DIR])
      break
    fi
  done

  if test -z "$VULKAN_DIR"; then
    dnl Try pkg-config as fallback
    PKG_CHECK_MODULES([VULKAN], [vulkan], [
      VULKAN_INCLUDES=$VULKAN_CFLAGS
      VULKAN_LIBS=$VULKAN_LIBS
      AC_MSG_RESULT([found via pkg-config])
    ], [
      AC_MSG_ERROR([Vulkan SDK not found. Install vulkan-sdk or specify --with-vulkan=DIR])
    ])
  else
    VULKAN_INCLUDES="-I$VULKAN_DIR/include"
    VULKAN_LIBS="-L$VULKAN_DIR/lib -lvulkan"

    dnl On macOS with Homebrew, vulkan-headers may be in a separate prefix
    if test -d "$VULKAN_DIR/opt/vulkan-headers/include"; then
      VULKAN_INCLUDES="-I$VULKAN_DIR/opt/vulkan-headers/include"
    fi
  fi

  PHP_EVAL_INCLINE($VULKAN_INCLUDES)
  PHP_EVAL_LIBLINE($VULKAN_LIBS, VULKAN_SHARED_LIBADD)
  PHP_SUBST(VULKAN_SHARED_LIBADD)

  PHP_NEW_EXTENSION(vulkan, [
    src/vulkan.c \
    src/vk_instance.c \
    src/vk_physical_device.c \
    src/vk_device.c \
    src/vk_queue.c \
    src/vk_buffer.c \
    src/vk_device_memory.c \
    src/vk_command_pool.c \
    src/vk_command_buffer.c \
    src/vk_shader_module.c \
    src/vk_descriptor_set_layout.c \
    src/vk_descriptor_pool.c \
    src/vk_descriptor_set.c \
    src/vk_pipeline_layout.c \
    src/vk_pipeline.c \
    src/vk_render_pass.c \
    src/vk_framebuffer.c \
    src/vk_image.c \
    src/vk_image_view.c \
    src/vk_sampler.c \
    src/vk_fence.c \
    src/vk_semaphore.c \
    src/vk_swapchain.c \
    src/vk_surface.c \
    src/vk_enums.c
  ], $ext_shared)
fi
