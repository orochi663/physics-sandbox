#pragma once
#include "IFont.h"
#include "IFontRenderer.h"
#include "ITexture.h"
#include "IShader.h"
#include "IIcon.h"
#include <mutex>
#include <unordered_map>
#include <memory>
#include <optional>
#include <string>

namespace ui {

class UIResourceManager {
public:
    // Singleton access
    static ResourceManager& getInstance() {
        static ResourceManager instance;
        return instance;
    }

    // Delete copy and move to enforce Singleton
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    ResourceManager(ResourceManager&&) = delete;
    ResourceManager& operator=(ResourceManager&&) = delete;

    // Set the font renderer
    void setFontRenderer(std::shared_ptr<IFontRenderer> renderer);

    // Load and retrieve resources
	[[nodiscard]] std::optional<std::shared_ptr<Font>> loadFont(const std::string& name, const std::filesystem::path& path);
	[[nodiscard]] std::optional<std::shared_ptr<Texture>> loadTexture(const std::filesystem::path& path);
	[[nodiscard]] std::optional<std::shared_ptr<Shader>> loadShader(const std::filesystem::path& path);
	[[nodiscard]] std::optional<std::shared_ptr<Icon>> loadIcon(const std::filesystem::path& path);
	[[nodiscard]] std::optional<std::shared_ptr<Font>> getFont(const std::string& name);
	[[nodiscard]] std::optional<std::shared_ptr<Texture>> getTexture(const std::string& name);
	[[nodiscard]] std::optional<std::shared_ptr<Shader>> getShader(const std::string& name);
	[[nodiscard]] std::optional<std::shared_ptr<Icon>> getIcon(const std::string& name);

private:
    ResourceManager() = default;
    ~ResourceManager() = default;

    std::mutex resourceMutex_;
    std::unordered_map<std::string, std::weak_ptr<Font>> fonts_;
    std::unordered_map<std::string, std::weak_ptr<Texture>> textures_;
    std::unordered_map<std::string, std::weak_ptr<Shader>> shaders_;
    std::unordered_map<std::string, std::weak_ptr<Icon>> icons_;
    std::shared_ptr<IFontRenderer> fontRenderer_;
};

} // namespace ui