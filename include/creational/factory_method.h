#pragma once
#include <memory>
#include <string>

/**
 * 工厂方法模式 (Factory Method Pattern)
 * 游戏开发中常用于：创建不同类型的敌人、武器、道具等
 * 特点：定义创建对象的接口，让子类决定实例化哪个类
 */

// 敌人基类
class Enemy {
public:
    virtual ~Enemy() = default;
    virtual void attack() = 0;
    virtual int getHealth() const = 0;
    virtual std::string getType() const = 0;
};

// 具体敌人类 - 哥布林
class Goblin : public Enemy {
private:
    int health = 50;
    
public:
    void attack() override {
        // 哥布林攻击逻辑
    }
    
    int getHealth() const override { return health; }
    std::string getType() const override { return "Goblin"; }
};

// 具体敌人类 - 龙
class Dragon : public Enemy {
private:
    int health = 200;
    
public:
    void attack() override {
        // 龙攻击逻辑
    }
    
    int getHealth() const override { return health; }
    std::string getType() const override { return "Dragon"; }
};

// 敌人工厂基类
class EnemyFactory {
public:
    virtual ~EnemyFactory() = default;
    
    // 工厂方法：由子类实现具体的创建逻辑
    virtual std::unique_ptr<Enemy> createEnemy() = 0;
    
    // 模板方法：提供统一的敌人创建流程
    std::unique_ptr<Enemy> spawnEnemy() {
        auto enemy = createEnemy();
        // 可以在这里添加通用的初始化逻辑
        return enemy;
    }
};

// 具体工厂 - 哥布林工厂
class GoblinFactory : public EnemyFactory {
public:
    std::unique_ptr<Enemy> createEnemy() override {
        return std::make_unique<Goblin>();
    }
};

// 具体工厂 - 龙工厂
class DragonFactory : public EnemyFactory {
public:
    std::unique_ptr<Enemy> createEnemy() override {
        return std::make_unique<Dragon>();
    }
};