#include "ui/UIPropertyPane.h"
#include "ui/UIFactory.h"
#include "ui/UILabel.h"
#include "ui/UITextField.h"
#include "ui/UIButton.h"
#include "ui/UIManager.h"
#include <spdlog/spdlog.h>
#include <algorithm>

namespace ui {

    std::unique_ptr<UIPropertyPane> UIPropertyPane::create(const std::string& title, const std::string& styleType, int zIndex) {
        auto pane = std::unique_ptr<UIPropertyPane>(new UIPropertyPane(title, styleType, zIndex));
        UIManager::getInstance().addCanvas(std::move(pane));
        return pane;
    }

    UIPropertyPane::UIPropertyPane(const std::string& title, const std::string& styleType, int zIndex)
        : UIDockable(title, styleType, zIndex)
    {
        size_ = glm::vec2(300.0f, 400.0f);
    }

    UIPropertyPane::~UIPropertyPane() {
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

    void UIPropertyPane::refresh() {
        rebuildPropertyElements();
    }

    void UIPropertyPane::setUseBeautification(bool useBeautification) {
        auto style = getEffectiveTheme()->getStyle(styleType_);
        style.useBeautification = useBeautification;
        rebuildPropertyElements();
    }

    void UIPropertyPane::setSeparatorColor(const glm::vec4& color) {
        auto style = getEffectiveTheme()->getStyle(styleType_);
        style.separatorColor = color;
        rebuildPropertyElements();
    }

    void UIPropertyPane::rebuildPropertyElements() {
        auto& children = getMutableChildren();
        children.clear();
        glm::vec2 currentPos = position_ + glm::vec2(0.0f, titleBarHeight_);
        for (size_t i = 0; i < objects_.size(); ++i) {
            auto obj = objects_[i];
            auto name = objectNames_[i];
            auto header = UIFactory::createElement("label", this);
            if (header) {
                dynamic_cast<UILabel*>(header.get())->setText(name);
                header->setPosition(currentPos);
                header->setTextAlignment(TextAlignment::Left);
                children.push_back(std::move(header));
                currentPos.y += 20.0f + padding_.y;
            }
        }
    }

    void UIPropertyPane::onStyleUpdate() {
        markDirty();
    }

    void UIPropertyPane::render(IRenderer* renderer) {
        if (!renderer || !isDirty() || !isVisible()) return;
        UIDockable::render(renderer);
        for (auto& child : getMutableChildren()) {
            if (child) child->render(renderer);
        }
        markDirty();
    }

    void UIPropertyPane::renderPropertyTree(const UIPropertyDescription& description, glm::vec2& position) {
        // Stub: implement recursive property tree rendering.
    }

    void UIPropertyPane::renderBeautification(IRenderer* renderer, const glm::vec2& position, float width, const glm::vec4& color) {
        if (renderer) {
            renderer->drawRect(position, glm::vec2(width, separatorHeight_), color);
        }
    }

    void UIPropertyPane::toggleSection() {
        // Stub: implement collapse/expand functionality.
    }

} // namespace ui
