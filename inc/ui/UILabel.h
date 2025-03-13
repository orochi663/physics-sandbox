#pragma once
#include "ui/UIElement.h"
#include <string>
#include <memory>

namespace ui {

    class UILabel : public UIElement {
    public:
        enum class Anchor { TopLeft, TopRight, BottomLeft, BottomRight, Center };

        // Factory method: create a UILabel with an optional initial text.
        static std::unique_ptr<UILabel> create(const std::string& text = "");
        void render(IRenderer* renderer) override;

        void setText(const std::string& text);
        std::string getText() const;

        // Concrete implementation for onStyleUpdate.
        void onStyleUpdate() override;

        // Set the text anchor.
        void setAnchor(Anchor anchor) { anchor_ = anchor; markDirty(); }

    private:
        // Constructor now takes an optional text string.
        UILabel(const std::string& text = "");
        std::string text_;
        Anchor anchor_{ Anchor::Center };
    };

} // namespace ui
