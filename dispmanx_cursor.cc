#include "ui/ozone/platform/dispmanx/dispmanx_cursor.h"

#include "ui/gfx/geometry/point_conversions.h"

namespace ui {

DispmanxCursor::DispmanxCursor(
    DispmanxWindowManager *window_manager) : window_manager_(window_manager),
      element_(DISPMANX_NO_HANDLE), resource_(DISPMANX_NO_HANDLE) {
  native_display_ = window_manager_->GetDisplay()->GetNativeDisplay();
  display_size_ = window_manager_->GetDisplay()->GetSize();
  bounds_.set_origin(gfx::Point(0, 0));
  bounds_.set_size(display_size_);
  location_ = gfx::PointF(bounds_.CenterPoint());
}

DispmanxCursor::~DispmanxCursor() {
  FreeCursor();
}

void DispmanxCursor::MoveCursor(const gfx::Vector2dF& delta) {
  location_ += delta;
  LimitLocation();
  WarpCursor();
}

void DispmanxCursor::MoveCursorTo(gfx::AcceleratedWidget widget,
    const gfx::PointF& location) {
  location_ = location;
  LimitLocation();
  WarpCursor();
}

void DispmanxCursor::MoveCursorTo(const gfx::PointF& location) {
  location_ = location;
  LimitLocation();
  WarpCursor();
}

gfx::PointF DispmanxCursor::GetLocation() {
  return location_;
}

bool DispmanxCursor::IsCursorVisible() {
  return true;
}

gfx::Rect DispmanxCursor::GetCursorConfinedBounds() {
  return bounds_;
}

void DispmanxCursor::InitializeOnEvdev() {
}

void DispmanxCursor::LimitLocation() {
  if (location_.x() < bounds_.x())
    location_.set_x(bounds_.x());
  if (location_.x() >= bounds_.right())
    location_.set_x(bounds_.right());
  if (location_.y() < bounds_.y())
    location_.set_y(bounds_.y());
  if (location_.y() >= bounds_.bottom())
    location_.set_y(bounds_.bottom());
}

void DispmanxCursor::LoadCursor(PlatformCursor cursor) {
  scoped_refptr<BitmapCursorOzone> bitmap =
    BitmapCursorFactoryOzone::GetBitmapCursor(cursor);
  if (!bitmap_.tryAllocPixels(
        SkImageInfo::MakeN32(32, bitmap->bitmap().height(),
          kUnpremul_SkAlphaType)))
    return;
  bitmap->bitmap().copyTo(&bitmap_);
  hotspot_ = bitmap->hotspot();
  if (resource_ != DISPMANX_NO_HANDLE)
    FreeCursor();
  int ret;
  uint32_t dummy;
  VC_RECT_T rect;
  int w = bitmap_.width(), h = bitmap_.height(), p = bitmap_.rowBytes();
  vc_dispmanx_rect_set(&rect, 0, 0, w, h);
  resource_ = vc_dispmanx_resource_create(VC_IMAGE_ARGB8888,
      w | (p << 16), h | (h << 16), &dummy);
  if (resource_ == DISPMANX_NO_HANDLE)
    LOG(ERROR) << "vc_dispmanx_resource_create() failed.";
  ret = vc_dispmanx_resource_write_data(resource_,
      VC_IMAGE_ARGB8888, p, bitmap_.getPixels(), &rect);
  if (ret != DISPMANX_SUCCESS)
    LOG(ERROR) << "vc_dispmanx_resource_write_data() failed.";
}

void DispmanxCursor::FreeCursor() {
  HideCursor();
  if (resource_ == DISPMANX_NO_HANDLE)
    return;
  vc_dispmanx_resource_delete(resource_);
  resource_ = DISPMANX_NO_HANDLE;
}

void DispmanxCursor::ShowCursor() {
  if (element_ != DISPMANX_NO_HANDLE)
    return;
  if (resource_ == DISPMANX_NO_HANDLE)
    return;
  int ret;
  int w = bitmap_.width(), h = bitmap_.height();
  gfx::Point p = gfx::ToFlooredPoint(location_) - hotspot_.OffsetFromOrigin();
  VC_DISPMANX_ALPHA_T alpha = { DISPMANX_FLAGS_ALPHA_FROM_SOURCE, 255, 0 };
  VC_RECT_T srect, drect;
  vc_dispmanx_rect_set(&srect, 0, 0, w << 16, h << 16);
  vc_dispmanx_rect_set(&drect, p.x(), p.y(), w, h);
  DISPMANX_UPDATE_HANDLE_T update = vc_dispmanx_update_start(1);
  element_ = vc_dispmanx_element_add(update, native_display_, 1,
      &drect, resource_, &srect, DISPMANX_PROTECTION_NONE, &alpha,
      DISPMANX_NO_HANDLE, DISPMANX_NO_ROTATE);
  if (element_ == DISPMANX_NO_HANDLE)
    LOG(ERROR) << "vc_dispmanx_element_add() failed.";
  ret = vc_dispmanx_update_submit_sync(update);
  if (ret != DISPMANX_SUCCESS)
    LOG(ERROR) << "vc_dispmanx_update_submit_sync() failed.";
}

void DispmanxCursor::HideCursor() {
  if (element_ == DISPMANX_NO_HANDLE)
    return;
  DISPMANX_UPDATE_HANDLE_T update = vc_dispmanx_update_start(1);
  vc_dispmanx_element_remove(update, element_);
  vc_dispmanx_update_submit_sync(update);
  element_ = DISPMANX_NO_HANDLE;
}

void DispmanxCursor::WarpCursor() {
  if (element_ == DISPMANX_NO_HANDLE)
    return;
  int ret;
  int w = bitmap_.width(), h = bitmap_.height();
  gfx::Point p = gfx::ToFlooredPoint(location_) - hotspot_.OffsetFromOrigin();
  VC_RECT_T srect, drect;
  vc_dispmanx_rect_set(&srect, 0, 0, w << 16, h << 16);
  vc_dispmanx_rect_set(&drect, p.x(), p.y(), w, h);
  DISPMANX_UPDATE_HANDLE_T update = vc_dispmanx_update_start(1);
  ret = vc_dispmanx_element_change_attributes(update, element_, 0, 0, 0,
      &drect, &srect, DISPMANX_NO_HANDLE, DISPMANX_NO_ROTATE);
  if (ret != DISPMANX_SUCCESS)
    LOG(ERROR) << "vc_dispmanx_element_change_attributes() failed.";
  ret = vc_dispmanx_update_submit(update, 0, nullptr);
  if (ret != DISPMANX_SUCCESS)
    LOG(ERROR) << "vc_dispmanx_update_submit() failed.";
}

} // namespace ui
