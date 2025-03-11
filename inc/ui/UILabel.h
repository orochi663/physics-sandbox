#pragma once
#include "UIElement.h"
#include <string>

namespace ui {

class UILabel : public UIElement {
public:
    enum class Anchor { TopLeft, TopRight, BottomLeft, BottomRight, Center };

    static std::unique_ptr<UILabel> create(const std::string* text = nullptr);
    void render(IRenderer* renderer) override;

    void setText(const std::string& text);
    std::string getText() const { return text_; }
    void setAnchor(Anchor anchor) { anchor_ = anchor; markDirty(); }

private:
    UILabel(const std::string* text);
    std::string text_;
    Anchor anchor_{Anchor::Center};
};

} // namespace ui