#pragma once
#include "UIDockable.h"
#include "UIPropertyDescription.h"
#include "UITheme.h"
#include "IExposable.h"
#include <vector>
#include <memory>
#include <unordered_map>
#include <string>
#include <functional>

namespace ui {

    class UIPropertyPane : public UIDockable {
    public:
        static std::unique_ptr<UIPropertyPane> create(const std::string& title = "Property Pane", const std::string& styleType = "propertyPane", int zIndex = 0);
        ~UIPropertyPane() override;

        void addObject(std::shared_ptr<IExposable> obj);
        void combineWith(const UIPropertyPane& other);
        void refresh();

        // Theme-related methods.
        static void setUseBeautification(bool useBeautification);
        static void setSeparatorColor(const glm::vec4& color);

        void onStyleUpdate() override;
        void render(IRenderer* renderer) override;

    private:
        UIPropertyPane(const std::string& title, const std::string& styleType, int zIndex);
        void rebuildPropertyElements();
        void renderPropertyTree(const UIPropertyDescription& description, glm::vec2& position);
        void renderBeautification(IRenderer* renderer, const glm::vec2& position, float width, const glm::vec4& color);
        void toggleSection();

        std::vector<std::shared_ptr<IExposable>> objects_;
        std::vector<std::string> objectNames_;
        float separatorHeight_{ 2.0f };
        glm::vec2 padding_{ 5.0f, 5.0f };
        std::unordered_map<std::string, std::function<void(UIElement*)>> customRenderers_;
    };

} // namespace ui
