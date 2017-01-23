// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_OZONE_PLATFORM_DISPMANX_DISPMANX_WINDOW_H_
#define UI_OZONE_PLATFORM_DISPMANX_DISPMANX_WINDOW_H_

#include <bcm_host.h>

#include "base/files/file_path.h"
#include "base/macros.h"
#include "ui/events/platform/platform_event_dispatcher.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/native_widget_types.h"
#include "ui/platform_window/platform_window.h"

namespace ui {
// XXX: Copy EGL_DISPMANX_WINDOW_T from /opt/vc/include/EGL/eglplatform.h
struct DispmanxNativeWindow {
  DISPMANX_ELEMENT_HANDLE_T element;
  int width, height;
};

class PlatformWindowDelegate;
class DispmanxWindowManager;

class DispmanxWindow : public PlatformWindow,
                       public PlatformEventDispatcher {
 public:
  DispmanxWindow(PlatformWindowDelegate* delegate,
                 DispmanxWindowManager* manager,
                 const gfx::Rect& bounds);
  ~DispmanxWindow() override;

  DispmanxNativeWindow *GetNativeWindow();

  // PlatformWindow:
  gfx::Rect GetBounds() override;
  void SetBounds(const gfx::Rect& bounds) override;
  void SetTitle(const base::string16& title) override;
  void Show() override;
  void Hide() override;
  void Close() override;
  void SetCapture() override;
  void ReleaseCapture() override;
  void ToggleFullscreen() override;
  void Maximize() override;
  void Minimize() override;
  void Restore() override;
  void SetCursor(PlatformCursor cursor) override;
  void MoveCursorTo(const gfx::Point& location) override;
  void ConfineCursorToBounds(const gfx::Rect& bounds) override;
  PlatformImeController* GetPlatformImeController() override;

  // PlatformEventDispatcher:
  bool CanDispatchEvent(const PlatformEvent& event) override;
  uint32_t DispatchEvent(const PlatformEvent& event) override;

 private:
  PlatformWindowDelegate* delegate_;
  DispmanxWindowManager* manager_;
  gfx::Rect bounds_;
  gfx::AcceleratedWidget widget_;
  DispmanxNativeWindow native_window_;

  DISALLOW_COPY_AND_ASSIGN(DispmanxWindow);
};

}  // namespace ui

#endif  // UI_OZONE_PLATFORM_DISPMANX_DISPMANX_WINDOW_H_
