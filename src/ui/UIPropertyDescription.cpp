#include "UIPropertyDescription.h"

namespace ui {

void UIPropertyDescription::addField(const std::string& label, FieldType type, const std::variant<float, std::string>& value,
                                     std::function<void(const std::variant&)> setter) {
    fields_.push_back({label, type, value, setter});
}

void UIPropertyDescription::addSubProperty(const UIPropertyDescription& subProperty) {
    subProperties_.push_back(subProperty);
}

} // namespace ui