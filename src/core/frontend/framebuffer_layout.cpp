// Copyright 2018 yuzu emulator team
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include <cmath>

#include "common/assert.h"
#include "core/frontend/framebuffer_layout.h"
#include "core/settings.h"

namespace Layout {

// Finds the largest size subrectangle contained in window area that is confined to the aspect ratio
template <class T>
static Common::Rectangle<T> MaxRectangle(Common::Rectangle<T> window_area,
                                         float screen_aspect_ratio) {
    float scale = std::min(static_cast<float>(window_area.GetWidth()),
                           window_area.GetHeight() / screen_aspect_ratio);
    return Common::Rectangle<T>{0, 0, static_cast<T>(std::round(scale)),
                                static_cast<T>(std::round(scale * screen_aspect_ratio))};
}

FramebufferLayout DefaultFrameLayout(u32 width, u32 height) {
    ASSERT(width > 0);
    ASSERT(height > 0);
    // The drawing code needs at least somewhat valid values for both screens
    // so just calculate them both even if the other isn't showing.
    FramebufferLayout res{width, height};

    const float emulation_aspect_ratio{static_cast<float>(ScreenUndocked::Height) /
                                       ScreenUndocked::Width};
    const auto window_aspect_ratio = static_cast<float>(height) / width;

    const Common::Rectangle<u32> screen_window_area{0, 0, width, height};
    Common::Rectangle<u32> screen = MaxRectangle(screen_window_area, emulation_aspect_ratio);

    if (window_aspect_ratio < emulation_aspect_ratio) {
        screen = screen.TranslateX((screen_window_area.GetWidth() - screen.GetWidth()) / 2);
    } else {
        screen = screen.TranslateY((height - screen.GetHeight()) / 2);
    }

    res.screen = screen;
    return res;
}

FramebufferLayout FrameLayoutFromResolutionScale(u32 res_scale) {
    u32 width, height;

    if (Settings::values.use_docked_mode) {
        width = ScreenDocked::WidthDocked * res_scale;
        height = ScreenDocked::HeightDocked * res_scale;
    } else {
        width = ScreenUndocked::Width * res_scale;
        height = ScreenUndocked::Height * res_scale;
    }

    return DefaultFrameLayout(width, height);
}

} // namespace Layout
