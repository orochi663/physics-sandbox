#include "UIGridLayout.h"
#include "UICanvas.h"

namespace ui {

UIGridLayout::UIGridLayout(int columns, float spacing) : columns_(columns), spacing_(spacing) {}

glm::vec2 UIGridLayout::arrange(UICanvas* canvas, std::vector<std::unique_ptr<UIElement>>& children) {
    if (columns_ <= 0 || children.empty()) {
        return canvas->getSize();
    }

    std::vector<float> columnWidths(columns_, 0.0f);
    std::vector<float> rowHeights;

    int row = 0;
    for (size_t i = 0; i < children.size(); ++i) {
        int col = i % columns_;
        if (col == 0) {
            rowHeights.push_back(0.0f);
            row = static_cast<int>(i / columns_);
        }
        if (children[i]) {
            columnWidths[col] = std::max(columnWidths[col], children[i]->getSize().x);
            rowHeights[row] = std::max(rowHeights[row], children[i]->getSize().y);
        }
    }

    row = 0;
    float xOffset = padding_.left + margin_.left;
    float yOffset = padding_.top + margin_.top;
    for (size_t i = 0; i < children.size(); ++i) {
        int col = i % columns_;
        if (col == 0 && i > 0) {
            xOffset = padding_.left + margin_.left;
            yOffset += rowHeights[row] + spacing_;
            row++;
        }
        if (children[i]) {
            children[i]->setPosition(glm::vec2(xOffset, yOffset));
            xOffset += columnWidths[col] + spacing_;
        }
    }

    return calculateContentSize(canvas, children);
}

glm::vec2 UIGridLayout::calculateContentSize(const UICanvas* canvas,
                                             const std::vector<std::unique_ptr<UIElement>>& children) const {
    if (columns_ <= 0 || children.empty()) {
        return glm::vec2(canvas->getSize().x - (padding_.left + padding_.right + margin_.left + margin_.right),
                         canvas->getSize().y - (padding_.top + padding_.bottom + margin_.top + margin_.bottom));
    }

    std::vector<float> columnWidths(columns_, 0.0f);
    std::vector<float> rowHeights;

    int row = 0;
    for (size_t i = 0; i < children.size(); ++i) {
        int col = i % columns_;
        if (col == 0) {
            rowHeights.push_back(0.0f);
            row = static_cast<int>(i / columns_);
        }
        if (children[i]) {
            columnWidths[col] = std::max(columnWidths[col], children[i]->getSize().x);
            rowHeights[row] = std::max(rowHeights[row], children[i]->getSize().y);
        }
    }

    float totalWidth = padding_.left + padding_.right + margin_.left + margin_.right;
    for (float width : columnWidths) {
        totalWidth += width + spacing_;
    }
    totalWidth = totalWidth > 0.0f ? totalWidth - spacing_ : 0.0f;

    float totalHeight = padding_.top + padding_.bottom + margin_.top + margin_.bottom;
    for (float height : rowHeights) {
        totalHeight += height + spacing_;
    }
    totalHeight = totalHeight > 0.0f ? totalHeight - spacing_ : 0.0f;

    return glm::vec2(totalWidth, totalHeight);
}

} // namespace ui