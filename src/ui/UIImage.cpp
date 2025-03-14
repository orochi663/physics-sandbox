#include "ui/UIImage.h"
#include "ui/UICanvas.h"
#include "ui/UITheme.h"
#include "ui/UIStyle.h"
#include "ui/IRenderer.h"

namespace ui {

    std::unique_ptr<UIImage> UIImage::create(ITexture* texture) {
        return std::unique_ptr<UIImage>(new UIImage(texture));
    }

    UIImage::UIImage(ITexture* texture) {
        styleType_ = "image";
        texture_ = (texture) ? std::unique_ptr<ITexture>(texture) : nullptr;
        registerEventHandler("styleUpdate", [this](UIElement*, EventType) { onStyleUpdate(); });
    }

    void UIImage::render(IRenderer* renderer) {
        if (!renderer || !dirty_) return;

        const UITheme* theme = nullptr;
        if (auto* canvas = dynamic_cast<UICanvas*>(parent_.value_or(nullptr))) {
            theme = canvas->getEffectiveTheme();
        }
        if (!theme) return;

        // Dereference the shared_ptr to get a copy of the style.
        UIStyle style = *theme->getStyle(styleType_);
        std::unordered_map<std::string, bool> states = { {"hovered", isHovered_}, {"pressed", isPressed_} };
        UIStyle effectiveStyle = style.computeEffectiveStyle(states);

        if (texture_) {
            renderer->drawTexture(position_, size_, texture_.get());
        }
        else {
            renderer->drawRect(position_, size_, effectiveStyle.backgroundColor);
        }

        dirty_ = false;
    }

    // New implementation for the pure virtual onStyleUpdate from UIElement.
    void UIImage::onStyleUpdate() {
        markDirty();
    }

} // namespace ui
