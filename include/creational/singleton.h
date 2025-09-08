#pragma once
#include <memory>

/**
 * 单例模式 (Singleton Pattern)
 * 游戏开发中常用于：游戏管理器、资源管理器、配置管理器等全局唯一的对象
 * 特点：确保一个类只有一个实例，并提供全局访问点
 */
class GameManager {
private:
    static std::unique_ptr<GameManager> instance;  // 唯一实例
    
    // 私有构造函数，防止外部创建实例
    GameManager() : isGameRunning(false), score(0) {}
    
    bool isGameRunning;
    int score;

public:
    // 删除拷贝构造函数和赋值操作符，确保单例特性
    GameManager(const GameManager&) = delete;
    GameManager& operator=(const GameManager&) = delete;
    
    // 获取单例实例的静态方法
    static GameManager& getInstance() {
        if (!instance) {
            instance = std::unique_ptr<GameManager>(new GameManager());
        }
        return *instance;
    }
    
    // 游戏管理相关方法
    void startGame() { isGameRunning = true; }
    void endGame() { isGameRunning = false; }
    bool isRunning() const { return isGameRunning; }
    
    void addScore(int points) { score += points; }
    int getScore() const { return score; }
    void resetScore() { score = 0; }
};

// 静态成员定义
std::unique_ptr<GameManager> GameManager::instance = nullptr;