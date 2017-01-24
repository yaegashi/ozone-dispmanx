// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/ozone/platform/dispmanx/ozone_platform_dispmanx.h"

#include "base/command_line.h"
#include "base/macros.h"
#include "base/memory/ptr_util.h"
#include "ui/base/cursor/ozone/bitmap_cursor_factory_ozone.h"
#include "ui/display/fake_display_delegate.h"
#include "ui/events/ozone/device/device_manager.h"
#include "ui/events/ozone/evdev/event_factory_evdev.h"
#include "ui/events/ozone/evdev/cursor_delegate_evdev.h"
#include "ui/events/ozone/layout/keyboard_layout_engine_manager.h"
#include "ui/events/ozone/layout/stub/stub_keyboard_layout_engine.h"
#include "ui/events/platform/platform_event_source.h"
#include "ui/ozone/common/stub_overlay_manager.h"
#include "ui/ozone/platform/dispmanx/dispmanx_display.h"
#include "ui/ozone/platform/dispmanx/dispmanx_surface_factory.h"
#include "ui/ozone/platform/dispmanx/dispmanx_window.h"
#include "ui/ozone/platform/dispmanx/dispmanx_window_manager.h"
#include "ui/ozone/public/cursor_factory_ozone.h"
#include "ui/ozone/public/gpu_platform_support_host.h"
#include "ui/ozone/public/input_controller.h"
#include "ui/ozone/public/ozone_platform.h"
#include "ui/ozone/public/ozone_switches.h"
#include "ui/ozone/public/system_input_injector.h"

namespace ui {

namespace {

class DispmanxCursorEvdev: public CursorDelegateEvdev {
 public:
  DispmanxCursorEvdev() {}
  ~DispmanxCursorEvdev() override {}
  void MoveCursor(const gfx::Vector2dF& delta) override { now_ += delta; }
  void MoveCursorTo(gfx::AcceleratedWidget widget,
      const gfx::PointF& location) override { now_ = location; }
  void MoveCursorTo(const gfx::PointF& location) override { now_ = location; }
  gfx::PointF GetLocation() override { return now_; }
  bool IsCursorVisible() override { return true; }
  gfx::Rect GetCursorConfinedBounds() override { return bounds_; }
  void InitializeOnEvdev() override { bounds_.SetRect(0, 0, 800, 600); }
 private:
  gfx::PointF now_;
  gfx::Rect bounds_;
};

// OzonePlatform for dispmanx mode
class OzonePlatformDispmanx : public OzonePlatform {
 public:
  OzonePlatformDispmanx() {}
  ~OzonePlatformDispmanx() override {}

  // OzonePlatform:
  ui::SurfaceFactoryOzone* GetSurfaceFactoryOzone() override {
    return surface_factory_.get();
  }
  OverlayManagerOzone* GetOverlayManager() override {
    return overlay_manager_.get();
  }
  CursorFactoryOzone* GetCursorFactoryOzone() override {
    return cursor_factory_ozone_.get();
  }
  InputController* GetInputController() override {
    return event_factory_evdev_->input_controller();
  }
  GpuPlatformSupportHost* GetGpuPlatformSupportHost() override {
    return gpu_platform_support_host_.get();
  }
  std::unique_ptr<SystemInputInjector> CreateSystemInputInjector() override {
    return event_factory_evdev_->CreateSystemInputInjector();
  }
  std::unique_ptr<PlatformWindow> CreatePlatformWindow(
      PlatformWindowDelegate* delegate,
      const gfx::Rect& bounds) override {
    std::unique_ptr<DispmanxWindow> window(
        new DispmanxWindow(delegate, window_manager_.get(), bounds));
    return std::move(window);
  }
  std::unique_ptr<display::NativeDisplayDelegate> CreateNativeDisplayDelegate()
      override {
    return base::MakeUnique<display::FakeDisplayDelegate>();
  }

  void InitializeUI() override {
    device_manager_ = CreateDeviceManager();
    display_.reset(new DispmanxDisplay());
    display_->Initialize();
    window_manager_.reset(new DispmanxWindowManager(display_.get()));
    window_manager_->Initialize();
    surface_factory_.reset(new DispmanxSurfaceFactory(window_manager_.get()));
    cursor_.reset(new DispmanxCursorEvdev());
    KeyboardLayoutEngineManager::SetKeyboardLayoutEngine(
        base::MakeUnique<StubKeyboardLayoutEngine>());
    overlay_manager_.reset(new StubOverlayManager());
    cursor_factory_ozone_.reset(new BitmapCursorFactoryOzone);
    gpu_platform_support_host_.reset(CreateStubGpuPlatformSupportHost());
    event_factory_evdev_.reset(new EventFactoryEvdev(
        cursor_.get(), device_manager_.get(),
        KeyboardLayoutEngineManager::GetKeyboardLayoutEngine()));
  }

  void InitializeGPU() override {
    if (!surface_factory_)
      surface_factory_.reset(new DispmanxSurfaceFactory(nullptr));
  }

 private:
  std::unique_ptr<DeviceManager> device_manager_;
  std::unique_ptr<DispmanxDisplay> display_;
  std::unique_ptr<DispmanxWindowManager> window_manager_;
  std::unique_ptr<DispmanxSurfaceFactory> surface_factory_;
  std::unique_ptr<DispmanxCursorEvdev> cursor_;
  std::unique_ptr<CursorFactoryOzone> cursor_factory_ozone_;
  std::unique_ptr<GpuPlatformSupportHost> gpu_platform_support_host_;
  std::unique_ptr<OverlayManagerOzone> overlay_manager_;
  std::unique_ptr<EventFactoryEvdev> event_factory_evdev_;

  DISALLOW_COPY_AND_ASSIGN(OzonePlatformDispmanx);
};

}  // namespace

OzonePlatform* CreateOzonePlatformDispmanx() {
  return new OzonePlatformDispmanx();
}

}  // namespace ui
