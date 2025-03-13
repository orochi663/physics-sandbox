#pragma once
#include "UIElement.h"
#include <memory>

namespace ui {

class ITexture;

class UIImage : public UIElement {
public:
    static std::unique_ptr<UIImage> create(ITexture* texture = nullptr);
    void render(IRenderer* renderer) override;

    void onStyleUpdate() override;

private:
    UIImage(ITexture* texture);
    std::unique_ptr<ITexture> texture_;
};

} // namespace ui