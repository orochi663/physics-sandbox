#pragma once
#include "IRenderer.h"
#include "IInputEvent.h"
#include "UIEventBus.h"
#include "UIStyle.h"
#include <glm/glm.hpp>
#include <memory>
#include <optional>
#include <vector>
#include <functional>

namespace ui {

class UIElement {
public:
    enum class TextAlignment { None, Left, Right, Center, CenterTop, CenterBottom };

    virtual ~UIElement() = default; // Made virtual for proper destruction in derived classes

    // Position and size
    void setPosition(const glm::vec2& pos);
    glm::vec2 getPosition() const { return position_; }
    void setSize(const glm::vec2& size);
    glm::vec2 getSize() const { return size_; }

    // Hierarchy (Composite Pattern)
    void addChild(std::unique_ptr<UIElement> child);
    void removeChild(const UIElement* child);
    virtual const std::vector<std::unique_ptr<UIElement>>& getChildren() const { return children_; }
    std::optional<UIElement*> getParent() const { return parent_; }

    // Rendering and input
    virtual void render(IRenderer* renderer) = 0;
    virtual bool handleInput(IMouseEvent* mouseEvent);
    virtual bool handleInput(IKeyboardEvent* keyboardEvent);
    virtual bool handleInput(ITextInputEvent* textEvent);

    // Focus management
    void setFocus(bool focused);
    bool hasFocus() const { return focused_; }
    virtual bool hitTest(const glm::vec2& point) const;

    // Focus priority
    void setFocusPriority(int priority) { focusPriority_ = priority; }
    int getFocusPriority() const { return focusPriority_; }

    // Style
    void setStyleType(const std::string& type) { styleType_ = type; }
    std::optional<std::string> getId() const { return id_; }
    void setId(const std::string& id) { id_ = id; }
    virtual void onStyleUpdate() = 0;
    bool isDirty() const { return dirty_; }
    void markDirty() { dirty_ = true; }

    // State for state-based styles
    bool isHovered() const { return isHovered_; }
    bool isPressed() const { return isPressed_; }

    // Text alignment
    void setTextAlignment(TextAlignment alignment) { textAlignment_ = alignment; markDirty(); }
    TextAlignment getTextAlignment() const { return textAlignment_; }

    // Event processor
    void registerEventHandler(const std::string& eventType, std::function<void(UIElement*, EventType)> handler);
    void unregisterEventHandler(const std::string& eventType);
    void publishEvent(const std::string& eventType, EventType data);

protected:
    UIElement(); // Protected constructor for Factory Pattern
    glm::vec2 position_{0.0f, 0.0f};
    glm::vec2 size_{100.0f, 100.0f};
    std::optional<UIElement*> parent_;
    std::vector<std::unique_ptr<UIElement>> children_;
    std::string styleType_;
    std::optional<std::string> id_;
    bool focused_{false};
    int focusPriority_{0};
    bool dirty_{true};
    bool isHovered_{false};
    bool isPressed_{false};
    TextAlignment textAlignment_{TextAlignment::None};

private:
    // Private to enforce Factory usage
};

} // namespace ui