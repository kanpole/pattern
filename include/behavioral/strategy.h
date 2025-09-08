#pragma once
#include <memory>
#include <vector>
#include <string>

/**
 * 策略模式 (Strategy Pattern)
 * 游戏开发中常用于：AI行为切换、不同攻击方式、移动算法等
 * 特点：定义一系列算法，把它们封装起来，并使它们可以相互替换
 */

// 前向声明
class AIEnemy;

// 策略接口 - AI行为策略
class AIBehavior {
public:
    virtual ~AIBehavior() = default;
    virtual void execute(AIEnemy* enemy, float deltaTime) = 0;
    virtual std::string getBehaviorName() const = 0;
    virtual bool canExecute(AIEnemy* enemy) const = 0;
};

// 上下文类 - 敌人
class AIEnemy {
private:
    std::string name;
    float x, y;
    int health, maxHealth;
    float speed;
    float detectionRange;
    float attackRange;
    std::unique_ptr<AIBehavior> currentBehavior;
    
    // 目标相关
    float targetX, targetY;
    bool hasTarget;
    
public:
    AIEnemy(const std::string& enemyName, float posX = 0, float posY = 0) 
        : name(enemyName), x(posX), y(posY), health(100), maxHealth(100), 
          speed(50.0f), detectionRange(100.0f), attackRange(30.0f),
          targetX(0), targetY(0), hasTarget(false) {}
    
    void setBehavior(std::unique_ptr<AIBehavior> behavior) {
        currentBehavior = std::move(behavior);
    }
    
    void update(float deltaTime) {
        if (currentBehavior && currentBehavior->canExecute(this)) {
            currentBehavior->execute(this, deltaTime);
        }
    }
    
    // 移动相关方法
    void moveTowards(float targetX, float targetY, float deltaTime) {
        float dx = targetX - x;
        float dy = targetY - y;
        float distance = std::sqrt(dx * dx + dy * dy);
        
        if (distance > 1.0f) {  // 避免抖动
            dx /= distance;
            dy /= distance;
            
            x += dx * speed * deltaTime;
            y += dy * speed * deltaTime;
        }
    }
    
    void patrol(const std::vector<std::pair<float, float>>& waypoints, float deltaTime) {
        // 巡逻逻辑的简化实现
        static size_t currentWaypoint = 0;
        if (!waypoints.empty()) {
            auto& target = waypoints[currentWaypoint % waypoints.size()];
            moveTowards(target.first, target.second, deltaTime);
            
            // 如果接近目标点，切换到下一个路点
            float dx = target.first - x;
            float dy = target.second - y;
            if (dx * dx + dy * dy < 25.0f) {  // 5像素范围内
                currentWaypoint++;
            }
        }
    }
    
    void attack() {
        // 攻击逻辑
        // dealDamageToTarget();
        // playAttackAnimation();
    }
    
    void flee(float fromX, float fromY, float deltaTime) {
        float dx = x - fromX;
        float dy = y - fromY;
        float distance = std::sqrt(dx * dx + dy * dy);
        
        if (distance > 0) {
            dx /= distance;
            dy /= distance;
            
            x += dx * speed * 1.5f * deltaTime;  // 逃跑速度更快
            y += dy * speed * 1.5f * deltaTime;
        }
    }
    
    // 访问器和修改器
    const std::string& getName() const { return name; }
    float getX() const { return x; }
    float getY() const { return y; }
    void setPosition(float newX, float newY) { x = newX; y = newY; }
    
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    void takeDamage(int damage) { health = std::max(0, health - damage); }
    void heal(int amount) { health = std::min(maxHealth, health + amount); }
    
    float getSpeed() const { return speed; }
    void setSpeed(float newSpeed) { speed = newSpeed; }
    
    float getDetectionRange() const { return detectionRange; }
    float getAttackRange() const { return attackRange; }
    
    void setTarget(float tX, float tY) { targetX = tX; targetY = tY; hasTarget = true; }
    void clearTarget() { hasTarget = false; }
    bool getHasTarget() const { return hasTarget; }
    float getTargetX() const { return targetX; }
    float getTargetY() const { return targetY; }
    
    float getDistanceToTarget() const {
        if (!hasTarget) return -1.0f;
        float dx = targetX - x;
        float dy = targetY - y;
        return std::sqrt(dx * dx + dy * dy);
    }
    
    std::string getCurrentBehaviorName() const {
        return currentBehavior ? currentBehavior->getBehaviorName() : "无行为";
    }
};

// 具体策略 - 巡逻行为
class PatrolBehavior : public AIBehavior {
private:
    std::vector<std::pair<float, float>> waypoints;
    
public:
    PatrolBehavior(const std::vector<std::pair<float, float>>& points) : waypoints(points) {}
    
    void execute(AIEnemy* enemy, float deltaTime) override {
        enemy->patrol(waypoints, deltaTime);
    }
    
    std::string getBehaviorName() const override {
        return "巡逻";
    }
    
    bool canExecute(AIEnemy* enemy) const override {
        // 巡逻行为总是可以执行，除非敌人死亡
        return enemy->getHealth() > 0;
    }
    
    void addWaypoint(float x, float y) {
        waypoints.push_back({x, y});
    }
};

// 具体策略 - 追击行为
class ChaseBehavior : public AIBehavior {
public:
    void execute(AIEnemy* enemy, float deltaTime) override {
        if (enemy->getHasTarget()) {
            enemy->moveTowards(enemy->getTargetX(), enemy->getTargetY(), deltaTime);
        }
    }
    
    std::string getBehaviorName() const override {
        return "追击";
    }
    
    bool canExecute(AIEnemy* enemy) const override {
        return enemy->getHealth() > 0 && enemy->getHasTarget() &&
               enemy->getDistanceToTarget() > enemy->getAttackRange();
    }
};

// 具体策略 - 攻击行为
class AttackBehavior : public AIBehavior {
private:
    float attackCooldown = 1.0f;  // 攻击冷却时间
    float lastAttackTime = 0.0f;
    
public:
    void execute(AIEnemy* enemy, float deltaTime) override {
        lastAttackTime += deltaTime;
        
        if (lastAttackTime >= attackCooldown) {
            enemy->attack();
            lastAttackTime = 0.0f;
        }
    }
    
    std::string getBehaviorName() const override {
        return "攻击";
    }
    
    bool canExecute(AIEnemy* enemy) const override {
        return enemy->getHealth() > 0 && enemy->getHasTarget() &&
               enemy->getDistanceToTarget() <= enemy->getAttackRange();
    }
};

// 具体策略 - 逃跑行为
class FleeBehavior : public AIBehavior {
public:
    void execute(AIEnemy* enemy, float deltaTime) override {
        if (enemy->getHasTarget()) {
            enemy->flee(enemy->getTargetX(), enemy->getTargetY(), deltaTime);
        }
    }
    
    std::string getBehaviorName() const override {
        return "逃跑";
    }
    
    bool canExecute(AIEnemy* enemy) const override {
        // 当生命值低于30%时执行逃跑行为
        return enemy->getHealth() > 0 && 
               static_cast<float>(enemy->getHealth()) / enemy->getMaxHealth() < 0.3f;
    }
};

// 具体策略 - 防御行为
class DefendBehavior : public AIBehavior {
private:
    float defendDuration = 2.0f;
    float defendTimer = 0.0f;
    
public:
    void execute(AIEnemy* enemy, float deltaTime) override {
        defendTimer += deltaTime;
        
        // 防御期间减少移动速度
        enemy->setSpeed(enemy->getSpeed() * 0.5f);
        
        if (defendTimer >= defendDuration) {
            defendTimer = 0.0f;
            // 防御结束，可以切换到其他行为
        }
    }
    
    std::string getBehaviorName() const override {
        return "防御";
    }
    
    bool canExecute(AIEnemy* enemy) const override {
        // 当生命值在30%-60%之间时可能进入防御状态
        float healthPercent = static_cast<float>(enemy->getHealth()) / enemy->getMaxHealth();
        return enemy->getHealth() > 0 && healthPercent >= 0.3f && healthPercent <= 0.6f;
    }
};

// 具体策略 - 狂暴行为
class BerserkBehavior : public AIBehavior {
public:
    void execute(AIEnemy* enemy, float deltaTime) override {
        // 狂暴状态下移动速度和攻击频率增加
        enemy->setSpeed(enemy->getSpeed() * 1.5f);
        
        if (enemy->getHasTarget()) {
            enemy->moveTowards(enemy->getTargetX(), enemy->getTargetY(), deltaTime);
            
            // 更频繁的攻击
            if (enemy->getDistanceToTarget() <= enemy->getAttackRange()) {
                enemy->attack();
            }
        }
    }
    
    std::string getBehaviorName() const override {
        return "狂暴";
    }
    
    bool canExecute(AIEnemy* enemy) const override {
        // 当生命值低于20%时进入狂暴状态
        return enemy->getHealth() > 0 && 
               static_cast<float>(enemy->getHealth()) / enemy->getMaxHealth() < 0.2f;
    }
};

// AI控制器 - 管理策略切换
class StrategyAIController {
private:
    AIEnemy* controlledEnemy;
    std::vector<std::unique_ptr<AIBehavior>> availableBehaviors;
    float behaviorUpdateInterval = 1.0f;  // 每秒检查一次行为切换
    float timeSinceLastUpdate = 0.0f;
    
public:
    StrategyAIController(AIEnemy* enemy) : controlledEnemy(enemy) {
        initializeBehaviors();
    }
    }
    
    void initializeBehaviors() {
        // 创建所有可用的行为策略
        availableBehaviors.push_back(std::make_unique<PatrolBehavior>(
            std::vector<std::pair<float, float>>{{0, 0}, {100, 0}, {100, 100}, {0, 100}}
        ));
        availableBehaviors.push_back(std::make_unique<ChaseBehavior>());
        availableBehaviors.push_back(std::make_unique<AttackBehavior>());
        availableBehaviors.push_back(std::make_unique<FleeBehavior>());
        availableBehaviors.push_back(std::make_unique<DefendBehavior>());
        availableBehaviors.push_back(std::make_unique<BerserkBehavior>());
        
        // 设置初始行为
        controlledEnemy->setBehavior(std::make_unique<PatrolBehavior>(
            std::vector<std::pair<float, float>>{{0, 0}, {100, 0}, {100, 100}, {0, 100}}
        ));
    }
    
    void update(float deltaTime) {
        timeSinceLastUpdate += deltaTime;
        
        // 定期检查是否需要切换行为
        if (timeSinceLastUpdate >= behaviorUpdateInterval) {
            selectBestBehavior();
            timeSinceLastUpdate = 0.0f;
        }
        
        // 更新当前行为
        controlledEnemy->update(deltaTime);
    }
    
    void selectBestBehavior() {
        // 根据优先级选择最合适的行为
        // 优先级：逃跑 > 狂暴 > 攻击 > 防御 > 追击 > 巡逻
        
        for (auto& behavior : availableBehaviors) {
            if (behavior->canExecute(controlledEnemy)) {
                std::string currentBehavior = controlledEnemy->getCurrentBehaviorName();
                std::string newBehavior = behavior->getBehaviorName();
                
                // 只在需要时切换行为
                if (currentBehavior != newBehavior) {
                    // 创建新的行为实例
                    if (newBehavior == "逃跑") {
                        controlledEnemy->setBehavior(std::make_unique<FleeBehavior>());
                        break;
                    } else if (newBehavior == "狂暴") {
                        controlledEnemy->setBehavior(std::make_unique<BerserkBehavior>());
                        break;
                    } else if (newBehavior == "攻击") {
                        controlledEnemy->setBehavior(std::make_unique<AttackBehavior>());
                        break;
                    } else if (newBehavior == "防御") {
                        controlledEnemy->setBehavior(std::make_unique<DefendBehavior>());
                        break;
                    } else if (newBehavior == "追击") {
                        controlledEnemy->setBehavior(std::make_unique<ChaseBehavior>());
                        break;
                    }
                }
            }
        }
    }
    
    void setTarget(float x, float y) {
        controlledEnemy->setTarget(x, y);
    }
    
    void clearTarget() {
        controlledEnemy->clearTarget();
    }
    
    void forceBehavior(const std::string& behaviorName) {
        // 强制切换到指定行为
        if (behaviorName == "巡逻") {
            controlledEnemy->setBehavior(std::make_unique<PatrolBehavior>(
                std::vector<std::pair<float, float>>{{0, 0}, {100, 0}, {100, 100}, {0, 100}}
            ));
        } else if (behaviorName == "追击") {
            controlledEnemy->setBehavior(std::make_unique<ChaseBehavior>());
        } else if (behaviorName == "攻击") {
            controlledEnemy->setBehavior(std::make_unique<AttackBehavior>());
        } else if (behaviorName == "逃跑") {
            controlledEnemy->setBehavior(std::make_unique<FleeBehavior>());
        } else if (behaviorName == "防御") {
            controlledEnemy->setBehavior(std::make_unique<DefendBehavior>());
        } else if (behaviorName == "狂暴") {
            controlledEnemy->setBehavior(std::make_unique<BerserkBehavior>());
        }
    }
};

// 不同难度的AI策略
class EasyAI : public AIBehavior {
public:
    void execute(AIEnemy* enemy, float deltaTime) override {
        // 简单AI：只会巡逻，反应较慢
        std::vector<std::pair<float, float>> waypoints = {{0, 0}, {50, 0}, {50, 50}, {0, 50}};
        enemy->patrol(waypoints, deltaTime * 0.5f);  // 移动速度减半
    }
    
    std::string getBehaviorName() const override { return "简单AI"; }
    bool canExecute(AIEnemy* enemy) const override { return enemy->getHealth() > 0; }
};

class HardAI : public AIBehavior {
private:
    StrategyAIController* controller;
    
public:
    HardAI(StrategyAIController* ctrl) : controller(ctrl) {}
    
    void execute(AIEnemy* enemy, float deltaTime) override {
        // 困难AI：智能行为切换，快速反应
        if (controller) {
            controller->update(deltaTime * 1.5f);  // 更快的反应速度
        }
    }
    
    std::string getBehaviorName() const override { return "困难AI"; }
    bool canExecute(AIEnemy* enemy) const override { return enemy->getHealth() > 0; }
};

// 战斗管理器 - 展示策略模式的应用
class BattleManager {
private:
    std::vector<std::unique_ptr<AIEnemy>> enemies;
    std::vector<std::unique_ptr<StrategyAIController>> aiControllers;
    
public:
    void createEnemies(int count, const std::string& difficulty = "normal") {
        for (int i = 0; i < count; ++i) {
            auto enemy = std::make_unique<AIEnemy>("敌人" + std::to_string(i + 1), i * 50.0f, 0);
            
            auto aiController = std::make_unique<StrategyAIController>(enemy.get());
            
            // 根据难度设置不同的AI策略
            if (difficulty == "easy") {
                enemy->setBehavior(std::make_unique<EasyAI>());
            } else if (difficulty == "hard") {
                enemy->setBehavior(std::make_unique<HardAI>(aiController.get()));
            }
            // normal难度使用默认的StrategyAIController逻辑
            
            aiControllers.push_back(std::move(aiController));
            enemies.push_back(std::move(enemy));
        }
    }
    
    void updateBattle(float deltaTime) {
        for (auto& controller : aiControllers) {
            controller->update(deltaTime);
        }
    }
    
    void setPlayerPosition(float x, float y) {
        // 让所有敌人以玩家为目标
        for (auto& controller : aiControllers) {
            controller->setTarget(x, y);
        }
    }
    
    void printEnemyStates() const {
        for (const auto& enemy : enemies) {
            // printf("%s: %s (血量: %d/%d)\n", 
            //        enemy->getName().c_str(),
            //        enemy->getCurrentBehaviorName().c_str(),
            //        enemy->getHealth(),
            //        enemy->getMaxHealth());
        }
    }
    
    void damageEnemies(int damage) {
        // 对所有敌人造成伤害，观察行为变化
        for (auto& enemy : enemies) {
            enemy->takeDamage(damage);
        }
    }
    
    size_t getAliveEnemyCount() const {
        size_t count = 0;
        for (const auto& enemy : enemies) {
            if (enemy->getHealth() > 0) {
                count++;
            }
        }
        return count;
    }
};