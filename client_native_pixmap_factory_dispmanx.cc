// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/ozone/platform/dispmanx/client_native_pixmap_factory_dispmanx.h"

#include "ui/ozone/common/stub_client_native_pixmap_factory.h"

namespace ui {

ClientNativePixmapFactory* CreateClientNativePixmapFactoryDispmanx() {
  return CreateStubClientNativePixmapFactory();
}

}  // namespace ui
