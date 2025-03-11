#pragma once
#include "UIDockable.h"
#include "UIPropertyDescription.h"
#include "UITheme.h" // New include
#include <vector>
#include <memory>
#include <unordered_map>

namespace ui {

class UIPropertyPane : public UIDockable {
public:
    static std::unique_ptr<UIPropertyPane> create(const std::string& title = "Property Pane", const std::string& styleType = "propertyPane", int zIndex = 0);
    ~UIPropertyPane() override;

    void addObject(std::shared_ptr<IExposable> obj);
    void combineWith(const UIPropertyPane& other);
    void refresh();

    // Theme-related methods (modify UITheme instead of local state)
    static void setUseBeautification(bool useBeautification);
    static void setSeparatorColor(const glm::vec4& color);

protected:
    void render(IRenderer* renderer) override;
    bool handleInput(IMouseEvent* mouseEvent) override;

private:
    struct SectionState {
        bool isCollapsed{false};
        std::unique_ptr<UIButton> toggleButton;
        std::vector<std::unique_ptr<UIElement>> elements; // Elements under this section
    };

    UIPropertyPane(const std::string& title, const std::string& styleType, int zIndex);
    void rebuildPropertyElements();
    void renderPropertyTree(const UIPropertyDescription& description, glm::vec2& position, SectionState& state);
    void renderBeautification(IRenderer* renderer, const glm::vec2& position, float width);
    void toggleSection(SectionState& state);

    static UITheme& getTheme(); // Access the global theme (to be implemented as a singleton or passed)
    std::vector<std::shared_ptr<IExposable>> objects_;
    std::vector<std::string> objectNames_;
    std::vector<SectionState> sectionStates_;
    float separatorHeight_{2.0f};
    glm::vec2 padding_{5.0f, 5.0f};
    std::unordered_map<std::string, std::function<void(UIElement*)>> customRenderers_;
};

} // namespace ui