#include "UIPropertyPane.h"
#include "UIFactory.h"
#include "UILabel.h"
#include "UITextField.h"
#include "UIButton.h"
#include "UIManager.h"
#include <spdlog/spdlog.h>
#include <algorithm>

// Global theme instance (for simplicity; consider a singleton or dependency injection)
namespace ui {
UITheme& UIPropertyPane::getTheme() {
    static UITheme theme;
    return theme;
}

std::unique_ptr<UIPropertyPane> UIPropertyPane::create(const std::string& title, const std::string& styleType, int zIndex) {
    auto pane = std::unique_ptr<UIPropertyPane>(new UIPropertyPane(title, styleType, zIndex));
    // Apply default style
    UIStyle style;
    style.useBeautification = true; // Default to true
    style.separatorColor = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f); // Default gray
    pane->getTheme().setStyle(styleType, style);
    return pane;
}

UIPropertyPane::UIPropertyPane(const std::string& title, const std::string& styleType, int zIndex)
    : UIDockable(title, styleType, zIndex) {
    UIManager::getInstance().addCanvas(this);
    size_ = glm::vec2(300.0f, 400.0f); // Default size with scrolling support
}

UIPropertyPane::~UIPropertyPane() {
    sectionStates_.clear();
}

void UIPropertyPane::addObject(std::shared_ptr<IExposable> obj) {
    if (obj) {
        objects_.push_back(obj);
        objectNames_.push_back(obj->getObjectName());
        rebuildPropertyElements();
    }
}

void UIPropertyPane::combineWith(const UIPropertyPane& other) {
    objects_.insert(objects_.end(), other.objects_.begin(), other.objects_.end());
    objectNames_.insert(objectNames_.end(), other.objectNames_.begin(), other.objectNames_.end());
    rebuildPropertyElements();
}

void UIPropertyPane::setUseBeautification(bool useBeautification) {
    auto style = getTheme().getStyle(styleType_);
    style.useBeautification = useBeautification;
    getTheme().setStyle(styleType_, style);
    rebuildPropertyElements();
}

void UIPropertyPane::setSeparatorColor(const glm::vec4& color) {
    auto style = getTheme().getStyle(styleType_);
    style.separatorColor = color;
    getTheme().setStyle(styleType_, style);
    rebuildPropertyElements();
}

void UIPropertyPane::refresh() {
    rebuildPropertyElements();
}

void UIPropertyPane::rebuildPropertyElements() {
    sectionStates_.clear();
    glm::vec2 currentPos(position_.x, position_.y + titleBarHeight_);

    auto style = getTheme().getStyle(styleType_);
    bool useBeautification = style.useBeautification;
    glm::vec4 separatorColor = style.separatorColor;

    for (size_t i = 0; i < objects_.size(); ++i) {
        auto obj = objects_[i];
        auto name = objectNames_[i];

        SectionState section;
        section.isCollapsed = false;

        // Header
        auto header = UIFactory::createElement("label", this);
        dynamic_cast<UILabel*>(header.get())->setText(name);
        header->setPosition(currentPos);
        header->setTextAlignment(TextAlignment::Left);
        section.elements.push_back(std::move(header));
        currentPos.y += 20.0f + padding_.y;

        // Collapsible toggle
        section.toggleButton = std::unique_ptr<UIButton>(dynamic_cast<UIButton*>(UIFactory::createElement("button", this).release()));
        section.toggleButton->setText("▼");
        section.toggleButton->setPosition(currentPos + glm::vec2(-20.0f, -15.0f));
        section.toggleButton->setSize(glm::vec2(20.0f, 20.0f));
        section.elements.push_back(std::move(section.toggleButton));

        if (!section.isCollapsed) {
            for (const auto& desc : obj->getProperties()) {
                renderPropertyTree(desc, currentPos, section);
                if (useBeautification && i < objects_.size() - 1) {
                    renderBeautification(nullptr, currentPos, size_.x, separatorColor);
                    currentPos.y += separatorHeight_ + padding_.y;
                }
            }
        }

        if (i < objects_.size() - 1) {
            renderBeautification(nullptr, currentPos, size_.x, separatorColor);
            currentPos.y += separatorHeight_ + padding_.y;
        }

        sectionStates_.push_back(std::move(section));
    }
}

void UIPropertyPane::renderPropertyTree(const UIPropertyDescription& description, glm::vec2& position, SectionState& state) {
    SectionState subSection;
    subSection.isCollapsed = false;

    // Property name
    auto nameLabel = UIFactory::createElement("label", this);
    dynamic_cast<UILabel*>(nameLabel.get())->setText(description.getName());
    nameLabel->setPosition(position);
    nameLabel->setTextAlignment(TextAlignment::Left);
    subSection.elements.push_back(std::move(nameLabel));
    position.y += 20.0f + padding_.y;

    // Collapsible toggle for composite properties
    if (!description.getSubProperties().empty()) {
        subSection.toggleButton = std::unique_ptr<UIButton>(dynamic_cast<UIButton*>(UIFactory::createElement("button", this).release()));
        subSection.toggleButton->setText("▼");
        subSection.toggleButton->setPosition(position + glm::vec2(-20.0f, -15.0f));
        subSection.toggleButton->setSize(glm::vec2(20.0f, 20.0f));
        subSection.elements.push_back(std::move(subSection.toggleButton));
    }

    if (!subSection.isCollapsed) {
        const auto& fields = description.getFields();
        if (!fields.empty()) {
            auto style = getTheme().getStyle(styleType_);
            bool useBeautification = style.useBeautification;
            glm::vec4 separatorColor = style.separatorColor;

            if (description.getLayout() == PropertyLayout::SingleLine) {
                glm::vec2 fieldPos = position;
                float fieldWidth = (size_.x - padding_.x * (fields.size() - 1)) / fields.size();

                for (const auto& field : fields) {
                    std::unique_ptr<UIElement> valueElement;
                    if (description.isReadOnly() || !field.fieldSetter) {
                        auto label = UIFactory::createElement("label", this);
                        dynamic_cast<UILabel*>(label.get())->setText(std::visit([](const auto& v) { return std::to_string(v); }, field.value));
                        label->setSize(glm::vec2(fieldWidth, 20.0f));
                        valueElement = std::move(label);
                    } else {
                        auto textField = UIFactory::createElement("textField", this);
                        dynamic_cast<UITextField*>(textField.get())->setText(std::visit([](const auto& v) { return std::to_string(v); }, field.value));
                        textField->setSize(glm::vec2(fieldWidth, 20.0f));
                        valueElement = std::move(textField);
                    }
                    valueElement->setPosition(fieldPos);
                    subSection.elements.push_back(std::move(valueElement));
                    fieldPos.x += fieldWidth + padding_.x;
                }
                position.y += 20.0f + padding_.y;
            } else {
                // Multi-line layout (e.g., matrix)
                int cols = 4; // Default columns (configurable in the future)
                int rows = (fields.size() + cols - 1) / cols;
                for (int row = 0; row < rows; ++row) {
                    glm::vec2 fieldPos = position;
                    float fieldWidth = (size_.x - padding_.x * (cols - 1)) / cols;
                    for (int col = 0; col < cols; ++col) {
                        int idx = row * cols + col;
                        if (idx >= fields.size()) break;

                        const auto& field = fields[idx];
                        std::unique_ptr<UIElement> valueElement;
                        if (description.isReadOnly() || !field.fieldSetter) {
                            auto label = UIFactory::createElement("label", this);
                            dynamic_cast<UILabel*>(label.get())->setText(std::visit([](const auto& v) { return std::to_string(v); }, field.value));
                            label->setSize(glm::vec2(fieldWidth, 20.0f));
                            valueElement = std::move(label);
                        } else {
                            auto textField = UIFactory::createElement("textField", this);
                            dynamic_cast<UITextField*>(textField.get())->setText(std::visit([](const auto& v) { return std::to_string(v); }, field.value));
                            textField->setSize(glm::vec2(fieldWidth, 20.0f));
                            valueElement = std::move(textField);
                        }
                        valueElement->setPosition(fieldPos);
                        subSection.elements.push_back(std::move(valueElement));
                        fieldPos.x += fieldWidth + padding_.x;
                    }
                    position.y += 20.0f + padding_.y;
                    if (useBeautification && row < rows - 1) {
                        renderBeautification(nullptr, position, size_.x, separatorColor);
                        position.y += separatorHeight_ + padding_.y;
                    }
                }
            }

            if (useBeautification && !description.getSubProperties().empty()) {
                renderBeautification(nullptr, position, size_.x, separatorColor);
                position.y += separatorHeight_ + padding_.y;
            }
        }

        for (const auto& subProp : description.getSubProperties()) {
            renderPropertyTree(subProp, position, subSection);
        }
    }

    state.elements.insert(state.elements.end(),
                          std::make_move_iterator(subSection.elements.begin()),
                          std::make_move_iterator(subSection.elements.end()));
}

void UIPropertyPane::renderBeautification(IRenderer* renderer, const glm::vec2& position, float width, const glm::vec4& color) {
    if (renderer) {
        renderer->drawRect(position, glm::vec2(width, separatorHeight_), color);
    }
}

void UIPropertyPane::toggleSection(SectionState& state) {
    state.isCollapsed = !state.isCollapsed;
    state.toggleButton->setText(state.isCollapsed ? "▶" : "▼");
    rebuildPropertyElements(); // Rebuild to reflect collapse state
}

void UIPropertyPane::render(IRenderer* renderer) {
    if (!renderer || !isVisible_ || !dirty_) return;

    UIDockable::render(renderer); // Base rendering

    for (const auto& section : sectionStates_) {
        for (const auto& element : section.elements) {
            if (element) {
                element->render(renderer);
            }
        }
    }
    dirty_ = false;
}

bool UIPropertyPane::handleInput(IMouseEvent* mouseEvent) {
    if (!mouseEvent || !isVisible_) return false;

    for (auto& section : sectionStates_) {
        if (section.toggleButton && section.toggleButton->handleInput(mouseEvent)) {
            toggleSection(section);
            return true;
        }
        for (auto& element : section.elements) {
            if (element && element->handleInput(mouseEvent)) {
                return true;
            }
        }
    }
    return UIDockable::handleInput(mouseEvent);
}

} // namespace ui