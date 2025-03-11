#pragma once
#include <string>
#include <vector>
#include <variant>
#include <functional>
#include <glm/glm.hpp>
#include <memory>

namespace ui {

class UIElement;

enum class PropertyLayout {
    SingleLine,
    MultiLine
};

enum class FieldType {
    Float,
    String,
    Custom
};

struct PropertyField {
    std::string label;
    FieldType type;
    std::variant<float, std::string> value;
    std::function<void(const std::variant&)> fieldSetter;
};

class UIPropertyDescription {
public:
    UIPropertyDescription(const std::string& name, PropertyLayout layout = PropertyLayout::SingleLine)
        : name_(name), layout_(layout), isReadOnly_(false), useBeautification_(false) {}

    std::string getName() const { return name_; }
    PropertyLayout getLayout() const { return layout_; }
    bool isReadOnly() const { return isReadOnly_; }
    const std::vector<PropertyField>& getFields() const { return fields_; }
    const std::vector<UIPropertyDescription>& getSubProperties() const { return subProperties_; }
    bool useBeautification() const { return useBeautification_; }
    glm::vec4 getSeparatorColor() const { return separatorColor_; }

    void setReadOnly(bool readOnly) { isReadOnly_ = readOnly; }
    void setUseBeautification(bool useBeautification) { useBeautification_ = useBeautification; }
    void setSeparatorColor(const glm::vec4& color) { separatorColor_ = color; }
    void addField(const std::string& label, FieldType type, const std::variant<float, std::string>& value,
                  std::function<void(const std::variant&)> setter = nullptr);
    void addSubProperty(const UIPropertyDescription& subProperty);

    std::function<void(const std::vector<std::variant>&)> getSetter() const { return setter_; }
    void setSetter(std::function<void(const std::vector<std::variant>&)> setter) { setter_ = setter; }

private:
    std::string name_;
    PropertyLayout layout_;
    bool isReadOnly_;
    std::vector<PropertyField> fields_;
    std::vector<UIPropertyDescription> subProperties_;
    std::function<void(const std::vector<std::variant>&)> setter_;
    bool useBeautification_;
    glm::vec4 separatorColor_{0.5f, 0.5f, 0.5f, 1.0f}; // Default gray
};

} // namespace ui