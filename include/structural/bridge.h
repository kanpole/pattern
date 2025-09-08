#pragma once
#include <memory>
#include <string>

/**
 * 桥接模式 (Bridge Pattern)
 * 游戏开发中常用于：分离游戏逻辑与渲染系统、平台相关代码等
 * 特点：将抽象与实现分离，使它们可以独立变化
 */

// 实现接口 - 渲染器实现
class RenderImplementation {
public:
    virtual ~RenderImplementation() = default;
    virtual void renderSprite(float x, float y, const std::string& texture) = 0;
    virtual void renderText(float x, float y, const std::string& text) = 0;
    virtual void clearScreen() = 0;
    virtual void present() = 0;
};

// 具体实现 - OpenGL渲染器
class OpenGLRenderer : public RenderImplementation {
public:
    void renderSprite(float x, float y, const std::string& texture) override {
        // OpenGL精灵渲染实现
        // glBindTexture, glDrawArrays等OpenGL调用
    }
    
    void renderText(float x, float y, const std::string& text) override {
        // OpenGL文本渲染实现
    }
    
    void clearScreen() override {
        // glClear(GL_COLOR_BUFFER_BIT);
    }
    
    void present() override {
        // 交换缓冲区
    }
};

// 具体实现 - DirectX渲染器
class DirectXRenderer : public RenderImplementation {
public:
    void renderSprite(float x, float y, const std::string& texture) override {
        // DirectX精灵渲染实现
        // D3D11相关调用
    }
    
    void renderText(float x, float y, const std::string& text) override {
        // DirectX文本渲染实现
    }
    
    void clearScreen() override {
        // D3D11清屏
    }
    
    void present() override {
        // 呈现帧
    }
};

// 抽象层 - 游戏渲染器
class GameRenderer {
protected:
    std::unique_ptr<RenderImplementation> implementation;
    
public:
    GameRenderer(std::unique_ptr<RenderImplementation> impl) 
        : implementation(std::move(impl)) {}
    
    virtual ~GameRenderer() = default;
    
    // 基本渲染操作
    virtual void beginFrame() {
        implementation->clearScreen();
    }
    
    virtual void endFrame() {
        implementation->present();
    }
    
    // 可以添加更高级的渲染方法
    virtual void render() = 0;
};

// 精化抽象 - 2D游戏渲染器
class Game2DRenderer : public GameRenderer {
private:
    float cameraX, cameraY;
    
public:
    Game2DRenderer(std::unique_ptr<RenderImplementation> impl) 
        : GameRenderer(std::move(impl)), cameraX(0), cameraY(0) {}
    
    void render() override {
        beginFrame();
        // 2D特有的渲染逻辑
        renderBackground();
        renderSprites();
        renderUI();
        endFrame();
    }
    
    void renderSprite(float x, float y, const std::string& texture) {
        // 应用摄像机变换
        float screenX = x - cameraX;
        float screenY = y - cameraY;
        implementation->renderSprite(screenX, screenY, texture);
    }
    
    void setCamera(float x, float y) {
        cameraX = x;
        cameraY = y;
    }
    
private:
    void renderBackground() {
        implementation->renderSprite(0, 0, "background.png");
    }
    
    void renderSprites() {
        // 渲染游戏中的精灵
    }
    
    void renderUI() {
        implementation->renderText(10, 10, "Score: 1000");
    }
};

// 精化抽象 - 3D游戏渲染器
class Game3DRenderer : public GameRenderer {
private:
    float cameraX, cameraY, cameraZ;
    float pitch, yaw, roll;
    
public:
    Game3DRenderer(std::unique_ptr<RenderImplementation> impl) 
        : GameRenderer(std::move(impl)), 
          cameraX(0), cameraY(0), cameraZ(0),
          pitch(0), yaw(0), roll(0) {}
    
    void render() override {
        beginFrame();
        // 3D特有的渲染逻辑
        setupProjection();
        renderScene();
        renderHUD();
        endFrame();
    }
    
    void setCamera(float x, float y, float z, float p, float y_angle, float r) {
        cameraX = x; cameraY = y; cameraZ = z;
        pitch = p; yaw = y_angle; roll = r;
    }
    
private:
    void setupProjection() {
        // 设置3D投影矩阵
    }
    
    void renderScene() {
        // 渲染3D场景
    }
    
    void renderHUD() {
        implementation->renderText(10, 10, "Health: 100");
    }
};

// 游戏引擎 - 使用桥接模式的客户端
class GameEngine {
private:
    std::unique_ptr<GameRenderer> renderer;
    
public:
    GameEngine(std::unique_ptr<GameRenderer> gameRenderer) 
        : renderer(std::move(gameRenderer)) {}
    
    void initialize() {
        // 引擎初始化
    }
    
    void gameLoop() {
        // 游戏主循环
        while (true) {
            update();
            renderer->render();
            // 处理输入等
        }
    }
    
    void setRenderer(std::unique_ptr<GameRenderer> newRenderer) {
        renderer = std::move(newRenderer);
    }
    
private:
    void update() {
        // 游戏逻辑更新
    }
};