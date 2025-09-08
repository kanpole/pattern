#pragma once
#include <vector>
#include <memory>
#include <functional>
#include <string>

/**
 * 观察者模式 (Observer Pattern)
 * 游戏开发中常用于：事件系统、UI更新、成就系统、状态同步等
 * 特点：定义对象间的一对多依赖关系，当主题状态改变时，所有依赖者都会被通知
 */

// 前向声明
class Subject;

// 观察者接口
class Observer {
public:
    virtual ~Observer() = default;
    virtual void update(Subject* subject) = 0;
    virtual std::string getObserverName() const = 0;
};

// 主题接口
class Subject {
protected:
    std::vector<Observer*> observers;
    
public:
    virtual ~Subject() = default;
    
    void attach(Observer* observer) {
        observers.push_back(observer);
    }
    
    void detach(Observer* observer) {
        observers.erase(
            std::remove(observers.begin(), observers.end(), observer),
            observers.end()
        );
    }
    
    void notify() {
        for (auto* observer : observers) {
            observer->update(this);
        }
    }
    
    size_t getObserverCount() const {
        return observers.size();
    }
};

// 具体主题 - 游戏玩家
class Player : public Subject {
private:
    std::string name;
    int health;
    int maxHealth;
    int level;
    int experience;
    int score;
    float x, y;  // 位置
    
public:
    Player(const std::string& playerName, int hp = 100) 
        : name(playerName), health(hp), maxHealth(hp), level(1), experience(0), score(0), x(0), y(0) {}
    
    // 生命值相关
    void takeDamage(int damage) {
        int oldHealth = health;
        health = std::max(0, health - damage);
        
        if (health != oldHealth) {
            notify();  // 通知观察者生命值变化
        }
        
        if (health <= 0) {
            onPlayerDeath();
        }
    }
    
    void heal(int amount) {
        int oldHealth = health;
        health = std::min(maxHealth, health + amount);
        
        if (health != oldHealth) {
            notify();
        }
    }
    
    // 经验和等级相关
    void gainExperience(int exp) {
        experience += exp;
        
        // 检查是否升级
        int requiredExp = level * 100;  // 简单的升级公式
        if (experience >= requiredExp) {
            levelUp();
        }
        
        notify();
    }
    
    void levelUp() {
        level++;
        maxHealth += 20;
        health = maxHealth;  // 升级时恢复满血
        
        notify();  // 通知升级事件
    }
    
    // 分数相关
    void addScore(int points) {
        score += points;
        notify();
    }
    
    // 位置相关
    void setPosition(float newX, float newY) {
        x = newX;
        y = newY;
        notify();
    }
    
    // 访问器
    const std::string& getName() const { return name; }
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    int getLevel() const { return level; }
    int getExperience() const { return experience; }
    int getScore() const { return score; }
    float getX() const { return x; }
    float getY() const { return y; }
    
    bool isAlive() const { return health > 0; }
    
private:
    void onPlayerDeath() {
        // 玩家死亡时的特殊处理
        // 这里也会触发通知，让观察者处理死亡事件
    }
};

// 具体观察者 - UI血条
class HealthBar : public Observer {
private:
    std::string uiElementId;
    int lastKnownHealth;
    int lastKnownMaxHealth;
    
public:
    HealthBar(const std::string& elementId) 
        : uiElementId(elementId), lastKnownHealth(0), lastKnownMaxHealth(0) {}
    
    void update(Subject* subject) override {
        Player* player = static_cast<Player*>(subject);
        
        int currentHealth = player->getHealth();
        int maxHealth = player->getMaxHealth();
        
        // 只在生命值发生变化时更新UI
        if (currentHealth != lastKnownHealth || maxHealth != lastKnownMaxHealth) {
            updateHealthDisplay(currentHealth, maxHealth);
            lastKnownHealth = currentHealth;
            lastKnownMaxHealth = maxHealth;
        }
    }
    
    std::string getObserverName() const override {
        return "血条UI (" + uiElementId + ")";
    }
    
private:
    void updateHealthDisplay(int health, int maxHealth) {
        float healthPercent = static_cast<float>(health) / maxHealth * 100;
        // updateUIElement(uiElementId, healthPercent);
    }
};

// 具体观察者 - 经验条
class ExperienceBar : public Observer {
private:
    std::string uiElementId;
    int lastKnownLevel;
    int lastKnownExp;
    
public:
    ExperienceBar(const std::string& elementId) 
        : uiElementId(elementId), lastKnownLevel(0), lastKnownExp(0) {}
    
    void update(Subject* subject) override {
        Player* player = static_cast<Player*>(subject);
        
        int currentLevel = player->getLevel();
        int currentExp = player->getExperience();
        
        if (currentLevel != lastKnownLevel || currentExp != lastKnownExp) {
            updateExperienceDisplay(currentLevel, currentExp);
            
            if (currentLevel > lastKnownLevel) {
                showLevelUpEffect();
            }
            
            lastKnownLevel = currentLevel;
            lastKnownExp = currentExp;
        }
    }
    
    std::string getObserverName() const override {
        return "经验条UI (" + uiElementId + ")";
    }
    
private:
    void updateExperienceDisplay(int level, int exp) {
        int requiredExp = level * 100;
        float expPercent = static_cast<float>(exp) / requiredExp * 100;
        // updateUIElement(uiElementId, expPercent);
    }
    
    void showLevelUpEffect() {
        // 显示升级特效
        // playLevelUpAnimation();
    }
};

// 具体观察者 - 分数显示
class ScoreDisplay : public Observer {
private:
    std::string displayId;
    int lastKnownScore;
    
public:
    ScoreDisplay(const std::string& id) : displayId(id), lastKnownScore(0) {}
    
    void update(Subject* subject) override {
        Player* player = static_cast<Player*>(subject);
        int currentScore = player->getScore();
        
        if (currentScore != lastKnownScore) {
            updateScoreDisplay(currentScore);
            
            if (currentScore > lastKnownScore) {
                showScoreGainEffect(currentScore - lastKnownScore);
            }
            
            lastKnownScore = currentScore;
        }
    }
    
    std::string getObserverName() const override {
        return "分数显示 (" + displayId + ")";
    }
    
private:
    void updateScoreDisplay(int score) {
        // updateTextElement(displayId, "Score: " + std::to_string(score));
    }
    
    void showScoreGainEffect(int gainedPoints) {
        // 显示分数增加的飞字效果
        // showFloatingText("+" + std::to_string(gainedPoints));
    }
};

// 具体观察者 - 音效管理器
class AudioManager : public Observer {
private:
    bool soundEnabled;
    
public:
    AudioManager(bool enabled = true) : soundEnabled(enabled) {}
    
    void update(Subject* subject) override {
        if (!soundEnabled) return;
        
        Player* player = static_cast<Player*>(subject);
        
        // 根据玩家状态播放相应音效
        if (!player->isAlive()) {
            playDeathSound();
        } else if (player->getHealth() < player->getMaxHealth() * 0.2f) {
            playLowHealthWarning();
        }
    }
    
    std::string getObserverName() const override {
        return "音效管理器";
    }
    
    void setSoundEnabled(bool enabled) { soundEnabled = enabled; }
    
private:
    void playDeathSound() {
        // playSoundEffect("player_death.wav");
    }
    
    void playLowHealthWarning() {
        // playSoundEffect("low_health_warning.wav");
    }
};

// 具体观察者 - 成就系统
class AchievementSystem : public Observer {
private:
    std::vector<std::string> unlockedAchievements;
    
public:
    void update(Subject* subject) override {
        Player* player = static_cast<Player*>(subject);
        
        checkAchievements(player);
    }
    
    std::string getObserverName() const override {
        return "成就系统";
    }
    
    const std::vector<std::string>& getUnlockedAchievements() const {
        return unlockedAchievements;
    }
    
private:
    void checkAchievements(Player* player) {
        // 检查各种成就条件
        if (player->getLevel() >= 10 && !hasAchievement("等级大师")) {
            unlockAchievement("等级大师");
        }
        
        if (player->getScore() >= 10000 && !hasAchievement("分数达人")) {
            unlockAchievement("分数达人");
        }
        
        if (player->getHealth() == player->getMaxHealth() && !hasAchievement("满血战士")) {
            unlockAchievement("满血战士");
        }
    }
    
    bool hasAchievement(const std::string& achievement) {
        return std::find(unlockedAchievements.begin(), unlockedAchievements.end(), achievement) 
               != unlockedAchievements.end();
    }
    
    void unlockAchievement(const std::string& achievement) {
        unlockedAchievements.push_back(achievement);
        // showAchievementNotification(achievement);
    }
};

// 具体观察者 - 小地图
class MiniMap : public Observer {
private:
    std::string mapId;
    float lastX, lastY;
    
public:
    MiniMap(const std::string& id) : mapId(id), lastX(0), lastY(0) {}
    
    void update(Subject* subject) override {
        Player* player = static_cast<Player*>(subject);
        
        float currentX = player->getX();
        float currentY = player->getY();
        
        // 只在位置发生显著变化时更新小地图
        if (std::abs(currentX - lastX) > 5 || std::abs(currentY - lastY) > 5) {
            updatePlayerPositionOnMap(currentX, currentY);
            lastX = currentX;
            lastY = currentY;
        }
    }
    
    std::string getObserverName() const override {
        return "小地图 (" + mapId + ")";
    }
    
private:
    void updatePlayerPositionOnMap(float x, float y) {
        // updateMapMarker(mapId, x, y);
    }
};

// 事件管理器 - 使用观察者模式的高级应用
class GameEventManager {
private:
    std::vector<std::unique_ptr<Observer>> managedObservers;
    
public:
    // 为玩家设置所有观察者
    void setupPlayerObservers(Player* player) {
        // 创建并注册所有观察者
        auto healthBar = std::make_unique<HealthBar>("main_health_bar");
        auto expBar = std::make_unique<ExperienceBar>("main_exp_bar");
        auto scoreDisplay = std::make_unique<ScoreDisplay>("main_score");
        auto audioManager = std::make_unique<AudioManager>(true);
        auto achievementSystem = std::make_unique<AchievementSystem>();
        auto miniMap = std::make_unique<MiniMap>("game_minimap");
        
        // 注册到玩家
        player->attach(healthBar.get());
        player->attach(expBar.get());
        player->attach(scoreDisplay.get());
        player->attach(audioManager.get());
        player->attach(achievementSystem.get());
        player->attach(miniMap.get());
        
        // 保存观察者的所有权
        managedObservers.push_back(std::move(healthBar));
        managedObservers.push_back(std::move(expBar));
        managedObservers.push_back(std::move(scoreDisplay));
        managedObservers.push_back(std::move(audioManager));
        managedObservers.push_back(std::move(achievementSystem));
        managedObservers.push_back(std::move(miniMap));
    }
    
    void removeAllObservers(Player* player) {
        // 从玩家移除所有观察者
        for (auto& observer : managedObservers) {
            player->detach(observer.get());
        }
        managedObservers.clear();
    }
    
    size_t getObserverCount() const {
        return managedObservers.size();
    }
};

// 游戏会话 - 展示观察者模式的完整应用
class GameSession {
private:
    std::unique_ptr<Player> player;
    std::unique_ptr<GameEventManager> eventManager;
    
public:
    GameSession(const std::string& playerName) {
        player = std::make_unique<Player>(playerName, 100);
        eventManager = std::make_unique<GameEventManager>();
        
        // 设置所有观察者
        eventManager->setupPlayerObservers(player.get());
    }
    
    ~GameSession() {
        // 清理时移除所有观察者
        if (eventManager && player) {
            eventManager->removeAllObservers(player.get());
        }
    }
    
    void simulateGameplay() {
        // 模拟游戏过程中的各种事件
        
        // 玩家受到伤害
        player->takeDamage(25);
        
        // 玩家获得经验
        player->gainExperience(50);
        
        // 玩家移动
        player->setPosition(100, 200);
        
        // 玩家获得分数
        player->addScore(500);
        
        // 玩家治疗
        player->heal(15);
        
        // 玩家继续获得经验（可能升级）
        player->gainExperience(75);
    }
    
    Player* getPlayer() const { return player.get(); }
    
    void printObserverStatus() {
        // printf("当前观察者数量: %zu\n", eventManager->getObserverCount());
        // printf("玩家观察者数量: %zu\n", player->getObserverCount());
    }
};