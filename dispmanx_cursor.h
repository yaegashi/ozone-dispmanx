#ifndef UI_OZONE_PLATFORM_DISPMANX_DISPMANX_CURSOR_H_
#define UI_OZONE_PLATFORM_DISPMANX_DISPMANX_CURSOR_H_

#include "base/macros.h"
#include "ui/base/cursor/cursor.h"
#include "ui/base/cursor/ozone/bitmap_cursor_factory_ozone.h"
#include "ui/events/ozone/evdev/cursor_delegate_evdev.h"
#include "ui/ozone/platform/dispmanx/dispmanx_window_manager.h"

namespace ui {

class DispmanxCursor : public CursorDelegateEvdev {
 public:
  explicit DispmanxCursor(DispmanxWindowManager* window_manager);
  ~DispmanxCursor() override;

  void LimitLocation();
  void LoadCursor(PlatformCursor cursor);
  void FreeCursor();
  void ShowCursor();
  void HideCursor();
  void WarpCursor();

  // CursorDelegateEvdev
  void MoveCursor(const gfx::Vector2dF& delta) override;
  void MoveCursorTo(gfx::AcceleratedWidget widget,
      const gfx::PointF& location) override;
  void MoveCursorTo(const gfx::PointF& location) override;
  gfx::PointF GetLocation() override;
  bool IsCursorVisible() override;
  gfx::Rect GetCursorConfinedBounds() override;
  void InitializeOnEvdev() override;

 private:
  DispmanxWindowManager* window_manager_;
  DISPMANX_ELEMENT_HANDLE_T element_;
  DISPMANX_RESOURCE_HANDLE_T resource_;
  DispmanxNativeDisplay native_display_;
  SkBitmap bitmap_;
  gfx::Point hotspot_;
  gfx::Size display_size_;
  gfx::Rect bounds_;
  gfx::PointF location_;

  DISALLOW_COPY_AND_ASSIGN(DispmanxCursor);
};

}  // namespace ui

#endif  // UI_OZONE_PLATFORM_DISPMANX_DISPMANX_CURSOR_H_

