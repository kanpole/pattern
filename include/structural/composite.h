#pragma once
#include <vector>
#include <memory>
#include <string>
#include <algorithm>

/**
 * 组合模式 (Composite Pattern)
 * 游戏开发中常用于：游戏对象层次结构、UI组件树、场景图等
 * 特点：将对象组合成树形结构，使客户端对单个对象和组合对象的使用具有一致性
 */

// 组件接口 - 游戏对象基类
class GameObjectComponent {
protected:
    std::string name;
    float x, y;  // 相对位置
    bool visible;
    
public:
    GameObjectComponent(const std::string& objectName, float posX = 0, float posY = 0) 
        : name(objectName), x(posX), y(posY), visible(true) {}
    
    virtual ~GameObjectComponent() = default;
    
    // 组合模式的核心操作
    virtual void update(float deltaTime) = 0;
    virtual void render(float parentX = 0, float parentY = 0) = 0;
    
    // 组合对象操作（叶子节点可以提供空实现）
    virtual void addChild(std::shared_ptr<GameObjectComponent> child) {
        // 默认空实现，叶子节点不支持添加子对象
    }
    
    virtual void removeChild(std::shared_ptr<GameObjectComponent> child) {
        // 默认空实现
    }
    
    virtual std::shared_ptr<GameObjectComponent> getChild(size_t index) {
        return nullptr;  // 默认返回空
    }
    
    virtual size_t getChildCount() const { return 0; }
    
    // 基本属性访问
    const std::string& getName() const { return name; }
    void setPosition(float posX, float posY) { x = posX; y = posY; }
    float getX() const { return x; }
    float getY() const { return y; }
    void setVisible(bool vis) { visible = vis; }
    bool isVisible() const { return visible; }
};

// 叶子节点 - 精灵对象
class SpriteObject : public GameObjectComponent {
private:
    std::string texture;
    float width, height;
    float rotation;
    
public:
    SpriteObject(const std::string& name, const std::string& textureName, float w = 32, float h = 32)
        : GameObjectComponent(name), texture(textureName), width(w), height(h), rotation(0) {}
    
    void update(float deltaTime) override {
        // 精灵更新逻辑（动画、物理等）
        rotation += deltaTime * 90;  // 每秒旋转90度
    }
    
    void render(float parentX = 0, float parentY = 0) override {
        if (!visible) return;
        
        float worldX = parentX + x;
        float worldY = parentY + y;
        
        // 渲染精灵的具体实现
        // renderTexture(texture, worldX, worldY, width, height, rotation);
    }
    
    void setTexture(const std::string& newTexture) { texture = newTexture; }
    void setSize(float w, float h) { width = w; height = h; }
    float getRotation() const { return rotation; }
};

// 叶子节点 - 文本对象
class TextObject : public GameObjectComponent {
private:
    std::string text;
    std::string font;
    int fontSize;
    
public:
    TextObject(const std::string& name, const std::string& content, int size = 16)
        : GameObjectComponent(name), text(content), font("default"), fontSize(size) {}
    
    void update(float deltaTime) override {
        // 文本对象可能有闪烁、滚动等效果
        (void)deltaTime;
    }
    
    void render(float parentX = 0, float parentY = 0) override {
        if (!visible) return;
        
        float worldX = parentX + x;
        float worldY = parentY + y;
        
        // 渲染文本的具体实现
        // renderText(text, font, fontSize, worldX, worldY);
    }
    
    void setText(const std::string& newText) { text = newText; }
    const std::string& getText() const { return text; }
};

// 组合节点 - 游戏对象容器
class GameObjectGroup : public GameObjectComponent {
private:
    std::vector<std::shared_ptr<GameObjectComponent>> children;
    
public:
    GameObjectGroup(const std::string& name) : GameObjectComponent(name) {}
    
    void update(float deltaTime) override {
        // 更新所有子对象
        for (auto& child : children) {
            child->update(deltaTime);
        }
    }
    
    void render(float parentX = 0, float parentY = 0) override {
        if (!visible) return;
        
        float worldX = parentX + x;
        float worldY = parentY + y;
        
        // 渲染所有子对象
        for (auto& child : children) {
            child->render(worldX, worldY);
        }
    }
    
    // 组合对象特有的操作
    void addChild(std::shared_ptr<GameObjectComponent> child) override {
        if (child) {
            children.push_back(child);
        }
    }
    
    void removeChild(std::shared_ptr<GameObjectComponent> child) override {
        children.erase(
            std::remove(children.begin(), children.end(), child),
            children.end()
        );
    }
    
    std::shared_ptr<GameObjectComponent> getChild(size_t index) override {
        if (index < children.size()) {
            return children[index];
        }
        return nullptr;
    }
    
    size_t getChildCount() const override {
        return children.size();
    }
    
    // 查找子对象
    std::shared_ptr<GameObjectComponent> findChild(const std::string& childName) {
        for (auto& child : children) {
            if (child->getName() == childName) {
                return child;
            }
        }
        return nullptr;
    }
    
    // 清空所有子对象
    void clear() {
        children.clear();
    }
};

// 特殊组合节点 - 场景
class Scene : public GameObjectGroup {
private:
    std::string backgroundMusic;
    
public:
    Scene(const std::string& sceneName) : GameObjectGroup(sceneName) {}
    
    void update(float deltaTime) override {
        // 场景特有的更新逻辑
        GameObjectGroup::update(deltaTime);
        
        // 可以添加场景级别的逻辑，如碰撞检测、音效管理等
    }
    
    void setBackgroundMusic(const std::string& music) {
        backgroundMusic = music;
    }
    
    void loadScene() {
        // 场景加载逻辑
    }
    
    void unloadScene() {
        // 场景卸载逻辑
        clear();
    }
};

// 客户端使用示例 - 游戏管理器
class GameSceneManager {
private:
    std::shared_ptr<Scene> currentScene;
    
public:
    void createSampleScene() {
        // 创建一个示例场景
        currentScene = std::make_shared<Scene>("MainScene");
        
        // 创建玩家对象（组合）
        auto player = std::make_shared<GameObjectGroup>("Player");
        auto playerSprite = std::make_shared<SpriteObject>("PlayerSprite", "player.png");
        auto playerHealthBar = std::make_shared<SpriteObject>("HealthBar", "health_bar.png");
        
        player->addChild(playerSprite);
        player->addChild(playerHealthBar);
        player->setPosition(100, 100);
        
        // 创建UI组合
        auto ui = std::make_shared<GameObjectGroup>("UI");
        auto scoreText = std::make_shared<TextObject>("ScoreText", "Score: 0", 24);
        auto livesText = std::make_shared<TextObject>("LivesText", "Lives: 3", 18);
        
        scoreText->setPosition(10, 10);
        livesText->setPosition(10, 40);
        
        ui->addChild(scoreText);
        ui->addChild(livesText);
        
        // 添加到场景
        currentScene->addChild(player);
        currentScene->addChild(ui);
    }
    
    void updateScene(float deltaTime) {
        if (currentScene) {
            currentScene->update(deltaTime);
        }
    }
    
    void renderScene() {
        if (currentScene) {
            currentScene->render(0, 0);
        }
    }
};