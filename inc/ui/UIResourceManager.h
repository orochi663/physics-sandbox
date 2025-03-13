#pragma once
#include "IFont.h"
#include "IFontRenderer.h"
#include "ITexture.h"
#include "IShader.h"
#include "IIcon.h"
#include "IResourceLoader.h"  // New include for the resource loader interface
#include <mutex>
#include <unordered_map>
#include <memory>
#include <optional>
#include <string>
#include <filesystem>

namespace ui {

class UIResourceManager {
public:
    // Singleton access
    static UIResourceManager& getInstance() {
        static UIResourceManager instance;
        return instance;
    }

    // Delete copy and move to enforce Singleton
    UIResourceManager(const UIResourceManager&) = delete;
    UIResourceManager& operator=(const UIResourceManager&) = delete;
    UIResourceManager(UIResourceManager&&) = delete;
    UIResourceManager& operator=(UIResourceManager&&) = delete;

    // Set the font renderer
    void setFontRenderer(std::shared_ptr<IFontRenderer> renderer);

    // Set the resource loader for textures, shaders, and icons
    void setResourceLoader(std::unique_ptr<IResourceLoader> loader);

    // Load and retrieve resources
    [[nodiscard]] std::optional<std::shared_ptr<Font>> loadFont(const std::string& name, const std::filesystem::path& path);
    [[nodiscard]] std::optional<std::shared_ptr<ITexture>> loadTexture(const std::filesystem::path& path);
    [[nodiscard]] std::optional<std::shared_ptr<IShader>> loadShader(const std::filesystem::path& path);
    [[nodiscard]] std::optional<std::shared_ptr<IIcon>> loadIcon(const std::filesystem::path& path);

    [[nodiscard]] std::optional<std::shared_ptr<Font>> getFont(const std::string& name);
    [[nodiscard]] std::optional<std::shared_ptr<ITexture>> getTexture(const std::string& name);
    [[nodiscard]] std::optional<std::shared_ptr<IShader>> getShader(const std::string& name);
    [[nodiscard]] std::optional<std::shared_ptr<IIcon>> getIcon(const std::string& name);

private:
    UIResourceManager() = default;
    ~UIResourceManager() = default;

    std::mutex resourceMutex_;
    std::unordered_map<std::string, std::weak_ptr<Font>> fonts_;
    std::unordered_map<std::string, std::weak_ptr<ITexture>> textures_;
    std::unordered_map<std::string, std::weak_ptr<IShader>> shaders_;
    std::unordered_map<std::string, std::weak_ptr<IIcon>> icons_;

    std::shared_ptr<IFontRenderer> fontRenderer_;
    std::unique_ptr<IResourceLoader> resourceLoader_;  // New resource loader member
};

} // namespace ui
