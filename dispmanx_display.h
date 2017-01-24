#ifndef UI_OZONE_PLATFORM_DISPMANX_DISPMANX_DISPLAY_H_
#define UI_OZONE_PLATFORM_DISPMANX_DISPMANX_DISPLAY_H_

#include <bcm_host.h>

#include "base/macros.h"
#include "ui/gfx/geometry/size.h"

namespace ui {

typedef DISPMANX_DISPLAY_HANDLE_T DispmanxNativeDisplay;

class DispmanxDisplay {
 public:
  explicit DispmanxDisplay();
  ~DispmanxDisplay();

  void Initialize();
  gfx::Size GetSize();
  DispmanxNativeDisplay GetNativeDisplay();

 private:
  gfx::Size size_;
  DispmanxNativeDisplay native_display_;

  DISALLOW_COPY_AND_ASSIGN(DispmanxDisplay);
};

}  // namespace ui

#endif  // UI_OZONE_PLATFORM_DISPMANX_DISPMANX_DISPLAY_H_

