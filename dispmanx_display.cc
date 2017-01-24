#include "ui/ozone/platform/dispmanx/dispmanx_display.h"

#include "base/logging.h"

namespace ui {

DispmanxDisplay::DispmanxDisplay() : native_display_(0) {
}

DispmanxDisplay::~DispmanxDisplay() {
  if (native_display_)
    vc_dispmanx_display_close(native_display_);
  bcm_host_deinit();
}

void DispmanxDisplay::Initialize() {
  bcm_host_init();
  uint32_t w, h;
  graphics_get_display_size(0, &w, &h);
  size_.SetSize(w, h);
  native_display_ = vc_dispmanx_display_open(0);
  if (!native_display_) {
    LOG(ERROR) << "vc_dispmanx_display_open() failed.";
  }
}

gfx::Size DispmanxDisplay::GetSize() {
  return size_;
}

DispmanxNativeDisplay DispmanxDisplay::GetNativeDisplay() {
  return native_display_;
}

} // namespace ui
