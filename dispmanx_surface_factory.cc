// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <bcm_host.h>

#include "ui/ozone/platform/dispmanx/dispmanx_surface_factory.h"

#include "third_party/khronos/EGL/eglplatform.h"
#include "third_party/khronos/EGL/egl.h"
#include "third_party/khronos/EGL/eglext.h"
#include "ui/gl/egl_util.h"
#include "ui/gl/gl_surface_egl.h"
#include "ui/ozone/common/egl_util.h"
#include "ui/ozone/common/gl_ozone_egl.h"
#include "ui/ozone/platform/dispmanx/dispmanx_window.h"
#include "ui/ozone/platform/dispmanx/dispmanx_window_manager.h"

namespace ui {

namespace {

const char kDefaultEglSoname[] = "libbrcmEGL.so";
const char kDefaultGlesSoname[] = "libbrcmGLESv2.so";

class GLOzoneEGLDispmanx : public GLOzoneEGL {
 public:
  explicit GLOzoneEGLDispmanx(DispmanxWindowManager* window_manager) :
    window_manager_(window_manager),
    display_handle_(0), display_width_(0), display_height_(0) {}
  ~GLOzoneEGLDispmanx() override {}

  bool InitializeGLOneOffPlatform() override {
    bcm_host_init();
    graphics_get_display_size(0, &display_width_, &display_height_);
    display_handle_ = vc_dispmanx_display_open(0);
    if (!display_handle_) {
      LOG(ERROR) << "vc_dispmanx_display_open() failed.";
      return false;
    }
    return GLOzoneEGL::InitializeGLOneOffPlatform();
  }

  void ShutdownGL() override {
    GLOzoneEGL::ShutdownGL();
    vc_dispmanx_display_close(display_handle_);
    bcm_host_deinit();
  }

  scoped_refptr<gl::GLSurface> CreateViewGLSurface(
      gfx::AcceleratedWidget widget) override {
    DispmanxWindow *window = window_manager_->GetWindow(widget);
    DispmanxNativeWindow *native_window = window->GetNativeWindow();
    VC_RECT_T srect, drect;
    srect.x = 0;
    srect.y = 0;
    srect.width = display_width_ << 16;
    srect.height = display_height_ << 16;
    drect.x = 0;
    drect.y = 0;
    drect.width = display_width_;
    drect.height = display_height_;
    DISPMANX_UPDATE_HANDLE_T update = vc_dispmanx_update_start(0);
    native_window->element =
      vc_dispmanx_element_add(update, display_handle_, 0, &drect, 0, &srect, 
          DISPMANX_PROTECTION_NONE, 0, 0, DISPMANX_NO_ROTATE);
    vc_dispmanx_update_submit_sync(update);
    native_window->width = display_width_;
    native_window->height = display_height_;
    return gl::InitializeGLSurface(
        new gl::NativeViewGLSurfaceEGL(
          reinterpret_cast<EGLNativeWindowType>(native_window)));
  }

  scoped_refptr<gl::GLSurface> CreateOffscreenGLSurface(
      const gfx::Size& size) override {
    return gl::InitializeGLSurface(new gl::PbufferGLSurfaceEGL(size));
  }

 protected:
  intptr_t GetNativeDisplay() override {
    return reinterpret_cast<intptr_t>(EGL_DEFAULT_DISPLAY);
  }

  bool LoadGLES2Bindings() override {
    return LoadEGLGLES2Bindings(kDefaultEglSoname, kDefaultGlesSoname);
  }

 private:
  DispmanxWindowManager* window_manager_;
  DISPMANX_DISPLAY_HANDLE_T display_handle_;
  uint32_t display_width_, display_height_;

  DISALLOW_COPY_AND_ASSIGN(GLOzoneEGLDispmanx);
};

}  // namespace

DispmanxSurfaceFactory::DispmanxSurfaceFactory(DispmanxWindowManager* window_manager) {
  egl_implementation_.reset(new GLOzoneEGLDispmanx(window_manager));
}

DispmanxSurfaceFactory::~DispmanxSurfaceFactory() {}

std::vector<gl::GLImplementation>
DispmanxSurfaceFactory::GetAllowedGLImplementations() {
  std::vector<gl::GLImplementation> impls;
  impls.push_back(gl::kGLImplementationEGLGLES2);
  impls.push_back(gl::kGLImplementationOSMesaGL);
  return impls;
}

GLOzone* DispmanxSurfaceFactory::GetGLOzone(gl::GLImplementation implementation) {
  switch (implementation) {
    case gl::kGLImplementationEGLGLES2:
      return egl_implementation_.get();
    default:
      return nullptr;
  }
}

}  // namespace ui
