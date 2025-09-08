#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

/**
 * 代理模式 (Proxy Pattern)
 * 游戏开发中常用于：资源懒加载、访问控制、缓存等
 * 特点：为另一个对象提供代理以控制对它的访问
 */

// 抽象接口 - 游戏资源
class GameAsset {
public:
    virtual ~GameAsset() = default;
    virtual void load() = 0;
    virtual void render() = 0;
    virtual bool isLoaded() const = 0;
    virtual size_t getSize() const = 0;
    virtual std::string getName() const = 0;
};

// 真实主题 - 大型3D模型
class LargeModel : public GameAsset {
private:
    std::string modelName;
    bool loaded;
    size_t fileSize;
    std::vector<float> vertexData;  // 模拟顶点数据
    
public:
    LargeModel(const std::string& name, size_t size) 
        : modelName(name), loaded(false), fileSize(size) {}
    
    void load() override {
        if (!loaded) {
            // 模拟加载大型文件的耗时过程
            // loadFromFile(modelName);
            
            // 模拟分配大量内存
            vertexData.resize(fileSize / sizeof(float));
            
            loaded = true;
            // 输出加载信息
        }
    }
    
    void render() override {
        if (loaded) {
            // 渲染3D模型
            // renderModel(vertexData);
        }
    }
    
    bool isLoaded() const override { return loaded; }
    size_t getSize() const override { return fileSize; }
    std::string getName() const override { return modelName; }
};

// 代理 - 虚拟代理（懒加载代理）
class ModelProxy : public GameAsset {
private:
    std::unique_ptr<LargeModel> realModel;
    std::string modelName;
    size_t modelSize;
    bool loadRequested;
    
public:
    ModelProxy(const std::string& name, size_t size) 
        : modelName(name), modelSize(size), loadRequested(false) {}
    
    void load() override {
        if (!realModel) {
            realModel = std::make_unique<LargeModel>(modelName, modelSize);
        }
        if (!loadRequested) {
            realModel->load();
            loadRequested = true;
        }
    }
    
    void render() override {
        // 按需加载：只有在渲染时才真正加载模型
        if (!realModel) {
            load();
        }
        realModel->render();
    }
    
    bool isLoaded() const override {
        return realModel && realModel->isLoaded();
    }
    
    size_t getSize() const override { return modelSize; }
    std::string getName() const override { return modelName; }
};

// 保护代理 - 访问控制代理
class SecureAssetProxy : public GameAsset {
private:
    std::unique_ptr<GameAsset> realAsset;
    std::string requiredPermission;
    
    bool hasPermission(const std::string& permission) const {
        // 模拟权限检查
        return permission == requiredPermission;
    }
    
public:
    SecureAssetProxy(std::unique_ptr<GameAsset> asset, const std::string& permission)
        : realAsset(std::move(asset)), requiredPermission(permission) {}
    
    void load() override {
        if (hasPermission("admin")) {
            realAsset->load();
        } else {
            // 访问被拒绝
        }
    }
    
    void render() override {
        if (hasPermission("render")) {
            realAsset->render();
        } else {
            // 渲染默认占位符
        }
    }
    
    bool isLoaded() const override {
        return realAsset->isLoaded();
    }
    
    size_t getSize() const override {
        return realAsset->getSize();
    }
    
    std::string getName() const override {
        return realAsset->getName();
    }
};

// 智能引用代理 - 引用计数和缓存
class SmartAssetProxy : public GameAsset {
private:
    static std::unordered_map<std::string, std::weak_ptr<GameAsset>> assetCache;
    std::shared_ptr<GameAsset> realAsset;
    std::string assetName;
    size_t referenceCount;
    
public:
    SmartAssetProxy(const std::string& name, size_t size) 
        : assetName(name), referenceCount(0) {
        
        // 检查缓存中是否已存在该资源
        auto it = assetCache.find(name);
        if (it != assetCache.end()) {
            auto cachedAsset = it->second.lock();
            if (cachedAsset) {
                realAsset = cachedAsset;
                return;
            }
        }
        
        // 创建新资源并加入缓存
        realAsset = std::make_shared<LargeModel>(name, size);
        assetCache[name] = realAsset;
    }
    
    void load() override {
        referenceCount++;
        realAsset->load();
    }
    
    void render() override {
        realAsset->render();
    }
    
    bool isLoaded() const override {
        return realAsset->isLoaded();
    }
    
    size_t getSize() const override {
        return realAsset->getSize();
    }
    
    std::string getName() const override {
        return assetName;
    }
    
    ~SmartAssetProxy() {
        // 析构时减少引用计数
        referenceCount--;
        if (referenceCount == 0) {
            // 可以选择从缓存中移除
        }
    }
};

std::unordered_map<std::string, std::weak_ptr<GameAsset>> SmartAssetProxy::assetCache;

// 远程代理 - 网络资源代理
class NetworkAssetProxy : public GameAsset {
private:
    std::string assetURL;
    std::unique_ptr<GameAsset> localAsset;
    bool downloaded;
    
    void downloadAsset() {
        if (!downloaded) {
            // 模拟从网络下载资源
            // downloadFromURL(assetURL);
            
            // 下载完成后创建本地资源
            localAsset = std::make_unique<LargeModel>("downloaded_" + assetURL, 1024 * 1024);
            downloaded = true;
        }
    }
    
public:
    NetworkAssetProxy(const std::string& url) 
        : assetURL(url), downloaded(false) {}
    
    void load() override {
        downloadAsset();
        if (localAsset) {
            localAsset->load();
        }
    }
    
    void render() override {
        if (!downloaded) {
            // 显示加载中的占位符
            return;
        }
        
        if (localAsset) {
            localAsset->render();
        }
    }
    
    bool isLoaded() const override {
        return downloaded && localAsset && localAsset->isLoaded();
    }
    
    size_t getSize() const override {
        return localAsset ? localAsset->getSize() : 0;
    }
    
    std::string getName() const override {
        return assetURL;
    }
};

// 资源管理器 - 使用各种代理的客户端
class AssetManager {
private:
    std::vector<std::unique_ptr<GameAsset>> assets;
    
public:
    // 添加懒加载资源
    void addLazyAsset(const std::string& name, size_t size) {
        assets.push_back(std::make_unique<ModelProxy>(name, size));
    }
    
    // 添加受保护的资源
    void addSecureAsset(const std::string& name, size_t size, const std::string& permission) {
        auto model = std::make_unique<LargeModel>(name, size);
        auto secureProxy = std::make_unique<SecureAssetProxy>(std::move(model), permission);
        assets.push_back(std::move(secureProxy));
    }
    
    // 添加网络资源
    void addNetworkAsset(const std::string& url) {
        assets.push_back(std::make_unique<NetworkAssetProxy>(url));
    }
    
    // 预加载所有资源
    void preloadAll() {
        for (auto& asset : assets) {
            asset->load();
        }
    }
    
    // 渲染所有已加载的资源
    void renderAll() {
        for (auto& asset : assets) {
            if (asset->isLoaded()) {
                asset->render();
            }
        }
    }
    
    // 获取资源统计信息
    void printStats() const {
        size_t loadedCount = 0;
        size_t totalSize = 0;
        
        for (const auto& asset : assets) {
            if (asset->isLoaded()) {
                loadedCount++;
            }
            totalSize += asset->getSize();
        }
        
        // 输出统计信息
        // printf("总资源数: %zu\n", assets.size());
        // printf("已加载资源数: %zu\n", loadedCount);
        // printf("总大小: %zu bytes\n", totalSize);
    }
    
    // 根据名称查找资源
    GameAsset* findAsset(const std::string& name) {
        for (auto& asset : assets) {
            if (asset->getName().find(name) != std::string::npos) {
                return asset.get();
            }
        }
        return nullptr;
    }
};

// 游戏场景 - 演示代理模式的使用
class GameScene {
private:
    AssetManager assetManager;
    bool assetsLoaded;
    
public:
    GameScene() : assetsLoaded(false) {
        // 添加各种类型的资源代理
        assetManager.addLazyAsset("character_model.obj", 5 * 1024 * 1024);     // 5MB
        assetManager.addLazyAsset("environment_model.obj", 20 * 1024 * 1024);  // 20MB
        assetManager.addLazyAsset("weapon_model.obj", 1 * 1024 * 1024);        // 1MB
        
        assetManager.addSecureAsset("admin_model.obj", 10 * 1024 * 1024, "admin");
        
        assetManager.addNetworkAsset("https://game-assets.com/model1.obj");
        assetManager.addNetworkAsset("https://game-assets.com/model2.obj");
    }
    
    void loadLevel() {
        if (!assetsLoaded) {
            // 通过代理按需加载资源，而不是一次性加载所有
            assetManager.preloadAll();
            assetsLoaded = true;
        }
    }
    
    void render() {
        // 代理会确保资源在渲染时是可用的
        assetManager.renderAll();
    }
    
    void showAssetInfo() {
        assetManager.printStats();
    }
};