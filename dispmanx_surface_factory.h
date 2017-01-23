// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_OZONE_PLATFORM_DISPMANX_DISPMANX_SURFACE_FACTORY_H_
#define UI_OZONE_PLATFORM_DISPMANX_DISPMANX_SURFACE_FACTORY_H_

#include <vector>

#include "base/macros.h"
#include "ui/gl/gl_surface.h"
#include "ui/ozone/public/gl_ozone.h"
#include "ui/ozone/public/surface_factory_ozone.h"
#include "ui/ozone/public/surface_ozone_canvas.h"

namespace ui {

class DispmanxWindowManager;

class DispmanxSurfaceFactory : public SurfaceFactoryOzone {
 public:
  explicit DispmanxSurfaceFactory(DispmanxWindowManager* window_manager);
  ~DispmanxSurfaceFactory() override;

  // SurfaceFactoryOzone:
  std::vector<gl::GLImplementation> GetAllowedGLImplementations() override;
  GLOzone* GetGLOzone(gl::GLImplementation implementation) override;

 private:
  std::unique_ptr<GLOzone> egl_implementation_;

  DISALLOW_COPY_AND_ASSIGN(DispmanxSurfaceFactory);
};

}  // namespace ui

#endif  // UI_OZONE_PLATFORM_DISPMANX_DISPMANX_SURFACE_FACTORY_H_
