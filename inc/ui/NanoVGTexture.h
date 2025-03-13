#include <nanovg.h>
#include <filesystem>
#include <optional>
#include <memory>
#include <string>
#include <glm/glm.hpp>


namespace ui {
// Concrete implementation of ITexture for NanoVG
class NanoVGTexture : public ITexture {
public:
    NanoVGTexture(const std::string& name, int imageId, int width, int height, NVGcontext* ctx)
        : name_(name), imageId_(imageId), width_(width), height_(height), ctx_(ctx) {}
    ~NanoVGTexture() override = default;

    std::string getName() const override { return name_; }
    int getWidth() const override { return width_; }
    int getHeight() const override { return height_; }
    // NanoVG does not expose raw pixel data; return nullptr.
    std::byte* getData() const override { return nullptr; }

    // Returns an NVGpaint for drawing this texture.
    NVGpaint getPaint(const glm::vec4& color = glm::vec4(1.0f)) const {
        // Create a paint pattern covering the entire image.
        return nvgImagePattern(ctx_, 0, 0, static_cast<float>(width_), static_cast<float>(height_), 0.0f, imageId_, color.a);
    }

private:
    std::string name_;
    int imageId_;
    int width_;
    int height_;
    NVGcontext* ctx_;
};

} // namespace ui