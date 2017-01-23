// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/ozone/platform/dispmanx/dispmanx_window_manager.h"

namespace ui {

DispmanxWindowManager::DispmanxWindowManager() {
}

DispmanxWindowManager::~DispmanxWindowManager() {
  DCHECK(thread_checker_.CalledOnValidThread());
}

void DispmanxWindowManager::Initialize() {
}

int32_t DispmanxWindowManager::AddWindow(DispmanxWindow* window) {
  return windows_.Add(window);
}

void DispmanxWindowManager::RemoveWindow(int32_t window_id,
                                         DispmanxWindow* window) {
  DCHECK_EQ(window, windows_.Lookup(window_id));
  windows_.Remove(window_id);
}

DispmanxWindow* DispmanxWindowManager::GetWindow(int32_t window_id) {
  return windows_.Lookup(window_id);
}

}  // namespace ui
