#pragma once
#include <memory>
#include <string>
#include <vector>

/**
 * 原型模式 (Prototype Pattern)
 * 游戏开发中常用于：克隆游戏对象（如子弹、敌人、道具）以提高性能
 * 特点：通过复制现有实例来创建新实例，而不是通过构造函数
 */

// 游戏对象抽象基类
class GameObject {
protected:
    float x, y;  // 位置
    std::string type;
    
public:
    GameObject(float posX = 0, float posY = 0) : x(posX), y(posY) {}
    virtual ~GameObject() = default;
    
    // 原型模式的核心：克隆方法
    virtual std::unique_ptr<GameObject> clone() const = 0;
    
    // 基本方法
    virtual void update() = 0;
    virtual void render() = 0;
    
    // 访问器
    float getX() const { return x; }
    float getY() const { return y; }
    void setPosition(float posX, float posY) { x = posX; y = posY; }
    std::string getType() const { return type; }
};

// 具体原型 - 子弹类
class Bullet : public GameObject {
private:
    float velocityX, velocityY;
    int damage;
    std::string bulletType;
    
public:
    Bullet(float posX = 0, float posY = 0, float velX = 10, float velY = 0, int dmg = 25, const std::string& bType = "普通子弹")
        : GameObject(posX, posY), velocityX(velX), velocityY(velY), damage(dmg), bulletType(bType) {
        type = "Bullet";
    }
    
    // 实现克隆方法
    std::unique_ptr<GameObject> clone() const override {
        return std::make_unique<Bullet>(x, y, velocityX, velocityY, damage, bulletType);
    }
    
    void update() override {
        x += velocityX;
        y += velocityY;
    }
    
    void render() override {
        // 子弹渲染逻辑
    }
    
    // 子弹特有方法
    int getDamage() const { return damage; }
    void setVelocity(float velX, float velY) { velocityX = velX; velocityY = velY; }
    std::string getBulletType() const { return bulletType; }
};

// 具体原型 - 敌人类
class EnemyUnit : public GameObject {
private:
    int health;
    int attackPower;
    float speed;
    std::string enemyType;
    
public:
    EnemyUnit(float posX = 0, float posY = 0, int hp = 100, int attack = 20, float spd = 2.0f, const std::string& eType = "哥布林")
        : GameObject(posX, posY), health(hp), attackPower(attack), speed(spd), enemyType(eType) {
        type = "Enemy";
    }
    
    // 实现克隆方法
    std::unique_ptr<GameObject> clone() const override {
        return std::make_unique<EnemyUnit>(x, y, health, attackPower, speed, enemyType);
    }
    
    void update() override {
        // 敌人AI逻辑
    }
    
    void render() override {
        // 敌人渲染逻辑
    }
    
    // 敌人特有方法
    int getHealth() const { return health; }
    int getAttackPower() const { return attackPower; }
    std::string getEnemyType() const { return enemyType; }
};

// 原型管理器 - 管理常用的原型对象
class PrototypeManager {
private:
    std::vector<std::unique_ptr<GameObject>> prototypes;
    
public:
    // 注册原型对象
    void registerPrototype(std::unique_ptr<GameObject> prototype) {
        prototypes.push_back(std::move(prototype));
    }
    
    // 根据类型克隆对象
    std::unique_ptr<GameObject> createObject(const std::string& type) {
        for (const auto& prototype : prototypes) {
            if (prototype->getType() == type) {
                return prototype->clone();
            }
        }
        return nullptr;
    }
    
    // 克隆指定索引的原型
    std::unique_ptr<GameObject> clonePrototype(size_t index) {
        if (index < prototypes.size()) {
            return prototypes[index]->clone();
        }
        return nullptr;
    }
    
    size_t getPrototypeCount() const { return prototypes.size(); }
};

// 游戏对象池 - 使用原型模式优化对象创建
class GameObjectPool {
private:
    std::unique_ptr<GameObject> bulletPrototype;
    std::unique_ptr<GameObject> enemyPrototype;
    std::vector<std::unique_ptr<GameObject>> activeBullets;
    std::vector<std::unique_ptr<GameObject>> activeEnemies;
    
public:
    GameObjectPool() {
        // 初始化原型对象
        bulletPrototype = std::make_unique<Bullet>(0, 0, 15, 0, 30, "快速子弹");
        enemyPrototype = std::make_unique<EnemyUnit>(0, 0, 80, 25, 1.5f, "强化哥布林");
    }
    
    // 创建子弹（通过克隆原型）
    void spawnBullet(float x, float y) {
        auto bullet = bulletPrototype->clone();
        bullet->setPosition(x, y);
        activeBullets.push_back(std::move(bullet));
    }
    
    // 创建敌人（通过克隆原型）
    void spawnEnemy(float x, float y) {
        auto enemy = enemyPrototype->clone();
        enemy->setPosition(x, y);
        activeEnemies.push_back(std::move(enemy));
    }
    
    // 更新所有活跃对象
    void updateAll() {
        for (auto& bullet : activeBullets) {
            bullet->update();
        }
        for (auto& enemy : activeEnemies) {
            enemy->update();
        }
    }
    
    size_t getBulletCount() const { return activeBullets.size(); }
    size_t getEnemyCount() const { return activeEnemies.size(); }
};