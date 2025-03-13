#include "ui/UIGridLayout.h"
#include "ui/UICanvas.h"
#include <algorithm>
#include <numeric>

namespace ui {

UIGridLayout::UIGridLayout(int columns, float spacing) : columns_(columns), spacing_(spacing) {}

glm::vec2 UIGridLayout::arrange(UICanvas* canvas, std::vector<std::unique_ptr<UIElement>>& children) {
    if (columns_ <= 0 || children.empty()) {
        return canvas->getSize();
    }

    std::vector<float> columnWidths(columns_, 0.0f);
    std::vector<float> rowHeights;

    int row = 0;
    for(int i = 0; i < children.size(); i++) {
        const auto child = children[i].get();
        int col = i % columns_;
        if (col == 0) {
            rowHeights.push_back(0.0f);
            row = static_cast<int>(i / columns_);
        }
        if (child) {
            columnWidths[col] = std::max(columnWidths[col], child->getSize().x);
            rowHeights[row] = std::max(rowHeights[row], child->getSize().y);
        }
    }

    // Simplified positioning loop with ranges
    row = 0;
    float xOffset = padding_.left + margin_.left;
    float yOffset = padding_.top + margin_.top;
    for (int i = 0; i < children.size(); i++) {
        const auto child = children[i].get();
        int col = i % columns_;
        if (col == 0 && i > 0) {
            xOffset = padding_.left + margin_.left;
            yOffset += rowHeights[row] + spacing_;
            row++;
        }
        if (child) {
            child->setPosition(glm::vec2(xOffset, yOffset));
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

    // Use ranges::enumerate for cleaner index access
    int row = 0;
    for (int i = 0; i < children.size(); i++) {
        const auto child = children[i].get();
        int col = i % columns_;
        if (col == 0) {
            rowHeights.push_back(0.0f);
            row = static_cast<int>(i / columns_);
        }
        if (child) {
            columnWidths[col] = std::max(columnWidths[col], child->getSize().x);
            rowHeights[row] = std::max(rowHeights[row], child->getSize().y);
        }
    }

    // Use ranges::accumulate for summing vectors
    float totalWidth = padding_.left + padding_.right + margin_.left + margin_.right +
                      std::accumulate(columnWidths.begin(), columnWidths.end(), 0.0f) +
                      spacing_ * (columnWidths.size() - 1);
    totalWidth = totalWidth > 0.0f ? totalWidth : 0.0f;

    float totalHeight = padding_.top + padding_.bottom + margin_.top + margin_.bottom +
                       std::accumulate(rowHeights.begin(), rowHeights.end(), 0.0f) +
                       spacing_ * (rowHeights.size() - 1);
    totalHeight = totalHeight > 0.0f ? totalHeight : 0.0f;

    return glm::vec2(totalWidth, totalHeight);
}

} // namespace ui