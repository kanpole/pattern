#pragma once
#include <memory>
#include <string>

/**
 * 抽象工厂模式 (Abstract Factory Pattern)
 * 游戏开发中常用于：创建不同平台的UI组件、不同风格的游戏资源等
 * 特点：提供创建一系列相关或相互依赖对象的接口，无需指定具体类
 */

// UI组件抽象类
class Button {
public:
    virtual ~Button() = default;
    virtual void render() = 0;
    virtual std::string getStyle() const = 0;
};

class Window {
public:
    virtual ~Window() = default;
    virtual void show() = 0;
    virtual std::string getTheme() const = 0;
};

// Windows风格的具体组件
class WindowsButton : public Button {
public:
    void render() override {
        // Windows风格按钮渲染逻辑
    }
    std::string getStyle() const override { return "Windows"; }
};

class WindowsWindow : public Window {
public:
    void show() override {
        // Windows风格窗口显示逻辑
    }
    std::string getTheme() const override { return "Windows"; }
};

// Mac风格的具体组件
class MacButton : public Button {
public:
    void render() override {
        // Mac风格按钮渲染逻辑
    }
    std::string getStyle() const override { return "Mac"; }
};

class MacWindow : public Window {
public:
    void show() override {
        // Mac风格窗口显示逻辑
    }
    std::string getTheme() const override { return "Mac"; }
};

// 抽象工厂接口
class UIFactory {
public:
    virtual ~UIFactory() = default;
    virtual std::unique_ptr<Button> createButton() = 0;
    virtual std::unique_ptr<Window> createWindow() = 0;
};

// 具体工厂 - Windows工厂
class WindowsUIFactory : public UIFactory {
public:
    std::unique_ptr<Button> createButton() override {
        return std::make_unique<WindowsButton>();
    }
    
    std::unique_ptr<Window> createWindow() override {
        return std::make_unique<WindowsWindow>();
    }
};

// 具体工厂 - Mac工厂
class MacUIFactory : public UIFactory {
public:
    std::unique_ptr<Button> createButton() override {
        return std::make_unique<MacButton>();
    }
    
    std::unique_ptr<Window> createWindow() override {
        return std::make_unique<MacWindow>();
    }
};

// 客户端类：游戏UI管理器
class GameUI {
private:
    std::unique_ptr<UIFactory> factory;
    std::unique_ptr<Button> button;
    std::unique_ptr<Window> window;
    
public:
    GameUI(std::unique_ptr<UIFactory> uiFactory) : factory(std::move(uiFactory)) {
        button = factory->createButton();
        window = factory->createWindow();
    }
    
    void renderUI() {
        button->render();
        window->show();
    }
};