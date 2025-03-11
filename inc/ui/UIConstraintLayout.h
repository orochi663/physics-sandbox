#pragma once
#include "UILayout.h"
#include <glm/glm.hpp>
#include <vector>
#include <map>
#include <set>
#include <optional>

namespace ui {

class UICanvas;

class UIConstraintLayout : public UILayout {
public:
    struct Constraint {
        enum class Anchor { Left, Right, Top, Bottom, CenterX, CenterY, Width, Height };
        enum class Type { Absolute, Proportional, MatchSize };

        Anchor anchor;
        Type type;
        UIElement* target{nullptr}; // nullptr means canvas
        float value{0.0f};         // Offset (Absolute), Percentage (Proportional, 0-1), or 1.0 for MatchSize
        int priority{0};           // Higher value takes precedence
    };

    UIConstraintLayout() = default;
    ~UIConstraintLayout() override = default;

    void addConstraint(UIElement* element, const Constraint& constraint);
    glm::vec2 arrange(UICanvas* canvas, std::vector<std::unique_ptr<UIElement>>& children) override;
    glm::vec2 calculateContentSize(const UICanvas* canvas,
                                   const std::vector<std::unique_ptr<UIElement>>& children) const override;

private:
    struct Dependency {
        UIElement* element;
        std::set<UIElement*> dependencies;
    };

    std::map<UIElement*, std::vector<Constraint>> constraints_;
    std::vector<Dependency> buildDependencyGraph(const std::vector<std::unique_ptr<UIElement>>& children);
    bool topologicalSortWithCycleDetection(std::vector<Dependency>& graph, std::vector<UIElement*>& sorted);
    void applyConstraints(UICanvas* canvas, std::vector<UIElement*>& sortedElements);
    bool validateConstraints(const std::vector<std::unique_ptr<UIElement>>& children);
};

} // namespace ui