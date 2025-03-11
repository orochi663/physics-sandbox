#pragma once
#include <cstddef> // For std::byte
#include <string>

namespace ui {

class ITexture {
public:
    virtual ~ITexture() = default;

    virtual std::string getName() const = 0;
    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;
    virtual std::byte* getData() const = 0; // Updated to std::byte*
};

} // namespace ui