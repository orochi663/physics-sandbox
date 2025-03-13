#include "ui/UIConstraintLayout.h"
#include "ui/UICanvas.h"
#include <spdlog/spdlog.h>
#include <algorithm>
#include <queue>

namespace ui {

void UIConstraintLayout::addConstraint(UIElement* element, const Constraint& constraint) {
    if (!element) {
        spdlog::error("UIConstraintLayout: Cannot add constraint to null element");
        return;
    }
    constraints_[element].push_back(constraint);
    // Sort constraints by priority (descending)
    auto& constraintsList = constraints_[element];
    std::sort(constraintsList.begin(), constraintsList.end(),
              [](const Constraint& a, const Constraint& b) { return a.priority > b.priority; });
}

glm::vec2 UIConstraintLayout::arrange(UICanvas* canvas, std::vector<std::unique_ptr<UIElement>>& children) {
    if (children.empty()) {
        return canvas->getSize();
    }

    if (!validateConstraints(children)) {
        spdlog::warn("UIConstraintLayout: Validation failed, proceeding with partial layout");
    }

    // Build and sort dependency graph
    std::vector<Dependency> graph = buildDependencyGraph(children);
    std::vector<UIElement*> sortedElements;
    if (!topologicalSortWithCycleDetection(graph, sortedElements)) {
        spdlog::warn("UIConstraintLayout: Cyclic dependencies detected, breaking cycles");
    }

    // Apply constraints iteratively until convergence
    glm::vec2 prevContentSize(-1.0f, -1.0f);
    int iteration = 0;
    const int maxIterations = 10; // Prevent infinite loops
    while (iteration < maxIterations) {
        applyConstraints(canvas, sortedElements);
        glm::vec2 newContentSize = calculateContentSize(canvas, children);
        if (newContentSize == prevContentSize) break;
        prevContentSize = newContentSize;
        iteration++;
    }
    if (iteration >= maxIterations) {
        spdlog::warn("UIConstraintLayout: Max iterations reached, layout may not be fully resolved");
    }

    return prevContentSize;
}

glm::vec2 UIConstraintLayout::calculateContentSize(const UICanvas* canvas,
                                                   const std::vector<std::unique_ptr<UIElement>>& children) const {
    if (children.empty()) {
        return glm::vec2(canvas->getSize().x - (padding_.left + padding_.right + margin_.left + margin_.right),
                         canvas->getSize().y - (padding_.top + padding_.bottom + margin_.top + margin_.bottom));
    }

    float maxX = padding_.left + margin_.left;
    float maxY = padding_.top + margin_.top;
    for (const auto& child : children) {
        if (child) {
            glm::vec2 childPos = child->getPosition();
            glm::vec2 childSize = child->getSize();
            maxX = std::max(maxX, childPos.x + childSize.x + padding_.right + margin_.right);
            maxY = std::max(maxY, childPos.y + childSize.y + padding_.bottom + margin_.bottom);
        }
    }
    return glm::vec2(maxX, maxY);
}

std::vector<UIConstraintLayout::Dependency> UIConstraintLayout::buildDependencyGraph(
    const std::vector<std::unique_ptr<UIElement>>& children) {
    std::vector<Dependency> graph;
    for (const auto& child : children) {
        if (child) {
            Dependency dep{child.get(), {}};
            auto it = constraints_.find(child.get());
            if (it != constraints_.end()) {
                for (const auto& constraint : it->second) {
                    if (constraint.target) {
                        dep.dependencies.insert(constraint.target);
                    }
                }
            }
            graph.push_back(dep);
        }
    }
    return graph;
}

bool UIConstraintLayout::topologicalSortWithCycleDetection(std::vector<Dependency>& graph,
                                                           std::vector<UIElement*>& sorted) {
    std::map<UIElement*, int> inDegree;
    std::set<UIElement*> allElements;
    std::map<UIElement*, std::set<UIElement*>> adjList;

    for (const auto& dep : graph) {
        allElements.insert(dep.element);
        adjList[dep.element] = dep.dependencies;
        for (UIElement* dependency : dep.dependencies) {
            allElements.insert(dependency);
            inDegree[dependency]++;
        }
        inDegree[dep.element] = inDegree[dep.element]; // Ensure entry exists
    }

    std::queue<UIElement*> queue;
    for (UIElement* element : allElements) {
        if (inDegree[element] == 0) {
            queue.push(element);
        }
    }

    std::set<UIElement*> visited;
    while (!queue.empty()) {
        UIElement* current = queue.front();
        queue.pop();
        sorted.push_back(current);
        visited.insert(current);

        for (UIElement* neighbor : adjList[current]) {
            if (--inDegree[neighbor] == 0) {
                queue.push(neighbor);
            }
        }
    }

    // Check for cycles
    return sorted.size() == allElements.size();
}

void UIConstraintLayout::applyConstraints(UICanvas* canvas, std::vector<UIElement*>& sortedElements) {
    for (UIElement* element : sortedElements) {
        auto it = constraints_.find(element);
        if (it == constraints_.end()) continue;

        glm::vec2 newPos = element->getPosition();
        glm::vec2 newSize = element->getSize();
        bool positionUpdated = false, sizeUpdated = false;

        for (const auto& constraint : it->second) {
            glm::vec2 targetPos = constraint.target ? constraint.target->getPosition() : glm::vec2(0.0f, 0.0f);
            glm::vec2 targetSize = constraint.target ? constraint.target->getSize() : canvas->getSize();
            float canvasWidth = canvas->getSize().x - (padding_.left + padding_.right + margin_.left + margin_.right);
            float canvasHeight = canvas->getSize().y - (padding_.top + padding_.bottom + margin_.top + margin_.bottom);

            switch (constraint.anchor) {
                case Constraint::Anchor::Left:
                    if (!positionUpdated || constraint.priority > it->second[0].priority) {
                        newPos.x = targetPos.x + targetSize.x * (constraint.type == Constraint::Type::Proportional ? constraint.value : 0.0f)
                                 + (constraint.type == Constraint::Type::Absolute ? constraint.value : 0.0f)
                                 + padding_.left + margin_.left;
                        positionUpdated = true;
                    }
                    break;
                case Constraint::Anchor::Right:
                    if (!positionUpdated || constraint.priority > it->second[0].priority) {
                        newPos.x = targetPos.x + targetSize.x * (constraint.type == Constraint::Type::Proportional ? constraint.value : 1.0f)
                                 - element->getSize().x + (constraint.type == Constraint::Type::Absolute ? constraint.value : 0.0f)
                                 + padding_.right + margin_.right;
                        positionUpdated = true;
                    }
                    break;
                case Constraint::Anchor::Top:
                    if (!positionUpdated || constraint.priority > it->second[0].priority) {
                        newPos.y = targetPos.y + targetSize.y * (constraint.type == Constraint::Type::Proportional ? constraint.value : 0.0f)
                                 + (constraint.type == Constraint::Type::Absolute ? constraint.value : 0.0f)
                                 + padding_.top + margin_.top;
                        positionUpdated = true;
                    }
                    break;
                case Constraint::Anchor::Bottom:
                    if (!positionUpdated || constraint.priority > it->second[0].priority) {
                        newPos.y = targetPos.y + targetSize.y * (constraint.type == Constraint::Type::Proportional ? constraint.value : 1.0f)
                                 - element->getSize().y + (constraint.type == Constraint::Type::Absolute ? constraint.value : 0.0f)
                                 + padding_.bottom + margin_.bottom;
                        positionUpdated = true;
                    }
                    break;
                case Constraint::Anchor::CenterX:
                    if (!positionUpdated || constraint.priority > it->second[0].priority) {
                        newPos.x = targetPos.x + targetSize.x * (constraint.type == Constraint::Type::Proportional ? constraint.value : 0.5f)
                                 - element->getSize().x * 0.5f + (constraint.type == Constraint::Type::Absolute ? constraint.value : 0.0f)
                                 + (padding_.left + padding_.right + margin_.left + margin_.right) * 0.5f;
                        positionUpdated = true;
                    }
                    break;
                case Constraint::Anchor::CenterY:
                    if (!positionUpdated || constraint.priority > it->second[0].priority) {
                        newPos.y = targetPos.y + targetSize.y * (constraint.type == Constraint::Type::Proportional ? constraint.value : 0.5f)
                                 - element->getSize().y * 0.5f + (constraint.type == Constraint::Type::Absolute ? constraint.value : 0.0f)
                                 + (padding_.top + padding_.bottom + margin_.top + margin_.bottom) * 0.5f;
                        positionUpdated = true;
                    }
                    break;
                case Constraint::Anchor::Width:
                    if (!sizeUpdated || constraint.priority > it->second[0].priority) {
                        if (constraint.type == Constraint::Type::MatchSize && constraint.target) {
                            newSize.x = constraint.target->getSize().x;
                        } else {
                            newSize.x = canvasWidth * (constraint.type == Constraint::Type::Proportional ? constraint.value : 1.0f)
                                      + (constraint.type == Constraint::Type::Absolute ? constraint.value : 0.0f);
                        }
                        sizeUpdated = true;
                    }
                    break;
                case Constraint::Anchor::Height:
                    if (!sizeUpdated || constraint.priority > it->second[0].priority) {
                        if (constraint.type == Constraint::Type::MatchSize && constraint.target) {
                            newSize.y = constraint.target->getSize().y;
                        } else {
                            newSize.y = canvasHeight * (constraint.type == Constraint::Type::Proportional ? constraint.value : 1.0f)
                                      + (constraint.type == Constraint::Type::Absolute ? constraint.value : 0.0f);
                        }
                        sizeUpdated = true;
                    }
                    break;
            }
        }

        if (positionUpdated) element->setPosition(newPos);
        if (sizeUpdated) element->setSize(newSize);
    }
}

bool UIConstraintLayout::validateConstraints(const std::vector<std::unique_ptr<UIElement>>& children) {
    for (const auto& child : children) {
        if (!child) continue;
        auto it = constraints_.find(child.get());
        if (it != constraints_.end()) {
            for (const auto& constraint : it->second) {
                if (constraint.target && std::none_of(children.begin(), children.end(),
                                                     [constraint](const auto& c) { return c.get() == constraint.target; })) {
                    spdlog::warn("UIConstraintLayout: Invalid target for constraint on element");
                    return false;
                }
                if (constraint.type == Constraint::Type::Proportional && (constraint.value < 0.0f || constraint.value > 1.0f)) {
                    spdlog::warn("UIConstraintLayout: Proportional value out of range [0,1]");
                    return false;
                }
            }
        }
    }
    return true;
}

} // namespace ui