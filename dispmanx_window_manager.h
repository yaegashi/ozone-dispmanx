// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_OZONE_PLATFORM_DISPMANX_DISPMANX_WINDOW_MANAGER_H_
#define UI_OZONE_PLATFORM_DISPMANX_DISPMANX_WINDOW_MANAGER_H_

#include <stdint.h>

#include <memory>

#include "base/files/file_path.h"
#include "base/id_map.h"
#include "base/macros.h"
#include "base/threading/thread_checker.h"
#include "ui/gfx/native_widget_types.h"
#include "ui/ozone/public/surface_factory_ozone.h"
#include "ui/ozone/platform/dispmanx/dispmanx_display.h"

namespace ui {

class DispmanxWindow;

class DispmanxWindowManager {
 public:
  explicit DispmanxWindowManager(DispmanxDisplay *display);
  ~DispmanxWindowManager();

  void Initialize();

  DispmanxDisplay* GetDisplay();

  // Register a new window. Returns the window id.
  int32_t AddWindow(DispmanxWindow* window);

  // Remove a window.
  void RemoveWindow(int32_t window_id, DispmanxWindow* window);

  // Find a window object by id;
  DispmanxWindow* GetWindow(int32_t window_id);

 private:
  DispmanxDisplay* display_;
  IDMap<DispmanxWindow*> windows_;
  base::ThreadChecker thread_checker_;

  DISALLOW_COPY_AND_ASSIGN(DispmanxWindowManager);
};

}  // namespace ui

#endif  // UI_OZONE_PLATFORM_DISPMANX_DISPMANX_WINDOW_MANAGER_H_
