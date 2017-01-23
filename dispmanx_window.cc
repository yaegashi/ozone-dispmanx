// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/ozone/platform/dispmanx/dispmanx_window.h"

#include <string>

#include "ui/events/event.h"
#include "ui/events/ozone/events_ozone.h"
#include "ui/events/platform/platform_event_source.h"
#include "ui/ozone/platform/dispmanx/dispmanx_window_manager.h"
#include "ui/platform_window/platform_window_delegate.h"

namespace ui {

DispmanxWindow::DispmanxWindow(PlatformWindowDelegate* delegate,
                               DispmanxWindowManager* manager,
                               const gfx::Rect& bounds)
    : delegate_(delegate), manager_(manager), bounds_(bounds) {
  widget_ = manager_->AddWindow(this);
  delegate_->OnAcceleratedWidgetAvailable(widget_, 1.f);
  PlatformEventSource::GetInstance()->AddPlatformEventDispatcher(this);
}

DispmanxWindow::~DispmanxWindow() {
  PlatformEventSource::GetInstance()->RemovePlatformEventDispatcher(this);
  manager_->RemoveWindow(widget_, this);
}

DispmanxNativeWindow *DispmanxWindow::GetNativeWindow() {
  return &native_window_;
}

gfx::Rect DispmanxWindow::GetBounds() {
  return bounds_;
}

void DispmanxWindow::SetBounds(const gfx::Rect& bounds) {
  bounds_ = bounds;
  delegate_->OnBoundsChanged(bounds);
}

void DispmanxWindow::SetTitle(const base::string16& title) {}

void DispmanxWindow::Show() {}

void DispmanxWindow::Hide() {}

void DispmanxWindow::Close() {}

void DispmanxWindow::SetCapture() {}

void DispmanxWindow::ReleaseCapture() {}

void DispmanxWindow::ToggleFullscreen() {}

void DispmanxWindow::Maximize() {}

void DispmanxWindow::Minimize() {}

void DispmanxWindow::Restore() {}

void DispmanxWindow::SetCursor(PlatformCursor cursor) {}

void DispmanxWindow::MoveCursorTo(const gfx::Point& location) {}

void DispmanxWindow::ConfineCursorToBounds(const gfx::Rect& bounds) {}

PlatformImeController* DispmanxWindow::GetPlatformImeController() {
  return nullptr;
}

bool DispmanxWindow::CanDispatchEvent(const PlatformEvent& event) {
  return true;
}

uint32_t DispmanxWindow::DispatchEvent(const PlatformEvent& native_event) {
  Event* event = static_cast<Event*>(native_event);
  if (event->IsLocatedEvent()) {
    LocatedEvent* located_event = event->AsLocatedEvent();
    gfx::PointF location = located_event->location_f();
    location -= gfx::Vector2dF(bounds_.OffsetFromOrigin());
    located_event->set_location_f(location);
    located_event->set_root_location_f(location);
  }
  DispatchEventFromNativeUiEvent(
      native_event, base::Bind(&PlatformWindowDelegate::DispatchEvent,
                               base::Unretained(delegate_)));
  return POST_DISPATCH_PERFORM_DEFAULT;
}

}  // namespace ui
