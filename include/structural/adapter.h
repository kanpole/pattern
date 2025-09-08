#pragma once
#include <string>
#include <memory>

/**
 * 适配器模式 (Adapter Pattern)
 * 游戏开发中常用于：适配不同的输入设备、第三方库接口等
 * 特点：将一个类的接口转换成客户端期望的另一个接口
 */

// 目标接口 - 游戏输入接口
class GameInput {
public:
    virtual ~GameInput() = default;
    virtual void moveUp() = 0;
    virtual void moveDown() = 0;
    virtual void moveLeft() = 0;
    virtual void moveRight() = 0;
    virtual void action() = 0;
};

// 需要适配的类 - 键盘输入（已存在的接口）
class KeyboardInput {
public:
    void pressW() { /* W键按下逻辑 */ }
    void pressS() { /* S键按下逻辑 */ }
    void pressA() { /* A键按下逻辑 */ }
    void pressD() { /* D键按下逻辑 */ }
    void pressSpace() { /* 空格键按下逻辑 */ }
    
    std::string getLastKey() const { return "示例键"; }
};

// 需要适配的类 - 手柄输入（第三方库接口）
class GamepadInput {
public:
    void leftStickUp() { /* 左摇杆向上 */ }
    void leftStickDown() { /* 左摇杆向下 */ }
    void leftStickLeft() { /* 左摇杆向左 */ }
    void leftStickRight() { /* 左摇杆向右 */ }
    void buttonA() { /* A按钮 */ }
    
    float getLeftStickX() const { return 0.0f; }
    float getLeftStickY() const { return 0.0f; }
    bool isButtonPressed(int button) const { return false; }
};

// 适配器 - 键盘输入适配器
class KeyboardAdapter : public GameInput {
private:
    std::unique_ptr<KeyboardInput> keyboard;
    
public:
    KeyboardAdapter(std::unique_ptr<KeyboardInput> kb) : keyboard(std::move(kb)) {}
    
    void moveUp() override {
        keyboard->pressW();
    }
    
    void moveDown() override {
        keyboard->pressS();
    }
    
    void moveLeft() override {
        keyboard->pressA();
    }
    
    void moveRight() override {
        keyboard->pressD();
    }
    
    void action() override {
        keyboard->pressSpace();
    }
};

// 适配器 - 手柄输入适配器
class GamepadAdapter : public GameInput {
private:
    std::unique_ptr<GamepadInput> gamepad;
    
public:
    GamepadAdapter(std::unique_ptr<GamepadInput> gp) : gamepad(std::move(gp)) {}
    
    void moveUp() override {
        gamepad->leftStickUp();
    }
    
    void moveDown() override {
        gamepad->leftStickDown();
    }
    
    void moveLeft() override {
        gamepad->leftStickLeft();
    }
    
    void moveRight() override {
        gamepad->leftStickRight();
    }
    
    void action() override {
        gamepad->buttonA();
    }
};

// 客户端 - 游戏控制器
class GameController {
private:
    std::unique_ptr<GameInput> input;
    
public:
    GameController(std::unique_ptr<GameInput> gameInput) : input(std::move(gameInput)) {}
    
    void handlePlayerMovement() {
        // 统一的输入处理逻辑，不关心具体的输入设备
        input->moveUp();
        input->moveRight();
        input->action();
    }
    
    void setInput(std::unique_ptr<GameInput> newInput) {
        input = std::move(newInput);
    }
};

// 对象适配器示例 - 音频系统适配器
class OldAudioSystem {
public:
    void playWav(const std::string& filename) {
        // 只支持WAV格式的旧音频系统
    }
};

class ModernAudioInterface {
public:
    virtual ~ModernAudioInterface() = default;
    virtual void play(const std::string& filename, const std::string& format) = 0;
    virtual void setVolume(float volume) = 0;
};

class AudioAdapter : public ModernAudioInterface {
private:
    std::unique_ptr<OldAudioSystem> oldSystem;
    
public:
    AudioAdapter(std::unique_ptr<OldAudioSystem> system) : oldSystem(std::move(system)) {}
    
    void play(const std::string& filename, const std::string& format) override {
        if (format == "wav" || format == "WAV") {
            oldSystem->playWav(filename);
        } else {
            // 可以在这里添加格式转换逻辑
            // 或者使用其他库来处理不支持的格式
        }
    }
    
    void setVolume(float volume) override {
        // 旧系统可能不支持音量控制，可以在这里添加兼容逻辑
        (void)volume;  // 忽略参数以避免警告
    }
};