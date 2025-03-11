#pragma once
#include <string>
#include <vector>
#include <memory>
#include <variant>
#include <functional>

namespace ui {

class UIPropertyDescription;

class IExposable {
public:
    virtual ~IExposable() = default;
    virtual std::string getObjectName() const = 0;
    virtual std::vector<UIPropertyDescription> getProperties() const = 0;
};

} // namespace ui