#pragma once
#include <memory>
#include <string>
#include <unordered_map>

/**
 * 状态模式 (State Pattern)
 * 游戏开发中常用于：角色状态管理、游戏状态机、AI行为状态等
 * 特点：允许对象在内部状态改变时改变它的行为，看起来像是修改了它的类
 */

// 前向声明
class StateCharacter;

// 状态接口
class CharacterState {
public:
    virtual ~CharacterState() = default;
    
    // 状态相关的行为
    virtual void handleInput(StateCharacter* character, int inputCode) = 0;
    virtual void update(StateCharacter* character, float deltaTime) = 0;
    virtual void onEnter(StateCharacter* character) = 0;
    virtual void onExit(StateCharacter* character) = 0;
    
    virtual std::string getStateName() const = 0;
    virtual bool canTransitionTo(const std::string& newState) const = 0;
};

// 上下文类 - 游戏角色
class StateCharacter {
private:
    std::unique_ptr<CharacterState> currentState;
    std::unordered_map<std::string, std::unique_ptr<CharacterState>> states;
    
    // 角色属性
    std::string name;
    float x, y;
    int health, maxHealth;
    float moveSpeed;
    bool isGrounded;
    float jumpVelocity;
    int mana, maxMana;
    
public:
    StateCharacter(const std::string& charName) 
        : name(charName), x(0), y(0), health(100), maxHealth(100), 
          moveSpeed(100.0f), isGrounded(true), jumpVelocity(0), mana(50), maxMana(50) {
        
        initializeStates();
        setState("idle");
    }
    
    void initializeStates();  // 在类定义后实现
    
    void handleInput(int inputCode) {
        if (currentState) {
            currentState->handleInput(this, inputCode);
        }
    }
    
    void update(float deltaTime) {
        if (currentState) {
            currentState->update(this, deltaTime);
        }
    }
    
    void setState(const std::string& stateName) {
        auto it = states.find(stateName);
        if (it != states.end()) {
            // 检查是否可以转换到新状态
            if (!currentState || currentState->canTransitionTo(stateName)) {
                if (currentState) {
                    currentState->onExit(this);
                }
                
                currentState = std::move(it->second);
                currentState->onEnter(this);
                
                // 重新创建状态实例（用于下次使用）
                createStateInstance(stateName);
            }
        }
    }
    
    std::string getCurrentStateName() const {
        return currentState ? currentState->getStateName() : "无状态";
    }
    
    // 访问器和修改器
    const std::string& getName() const { return name; }
    float getX() const { return x; }
    float getY() const { return y; }
    void setPosition(float newX, float newY) { x = newX; y = newY; }
    void move(float deltaX, float deltaY) { x += deltaX; y += deltaY; }
    
    int getHealth() const { return health; }
    void setHealth(int hp) { health = std::max(0, std::min(maxHealth, hp)); }
    
    float getMoveSpeed() const { return moveSpeed; }
    void setMoveSpeed(float speed) { moveSpeed = speed; }
    
    bool getIsGrounded() const { return isGrounded; }
    void setGrounded(bool grounded) { isGrounded = grounded; }
    
    float getJumpVelocity() const { return jumpVelocity; }
    void setJumpVelocity(float velocity) { jumpVelocity = velocity; }
    
    int getMana() const { return mana; }
    void setMana(int mp) { mana = std::max(0, std::min(maxMana, mp)); }
    
private:
    void createStateInstance(const std::string& stateName);  // 在类定义后实现
};

// 具体状态 - 空闲状态
class IdleState : public CharacterState {
public:
    void handleInput(StateCharacter* character, int inputCode) override {
        switch (inputCode) {
            case 65: // A键 - 向左移动
            case 68: // D键 - 向右移动
                character->setState("walking");
                break;
            case 32: // 空格键 - 跳跃
                if (character->getIsGrounded()) {
                    character->setState("jumping");
                }
                break;
            case 74: // J键 - 攻击
                character->setState("attacking");
                break;
            case 75: // K键 - 施法
                if (character->getMana() >= 10) {
                    character->setState("casting");
                }
                break;
        }
    }
    
    void update(StateCharacter* character, float deltaTime) override {
        // 空闲状态下的更新逻辑
        // 可能包括待机动画、缓慢回复魔法值等
        int currentMana = character->getMana();
        if (currentMana < 50) {  // 假设最大魔法值是50
            character->setMana(currentMana + static_cast<int>(5 * deltaTime));
        }
    }
    
    void onEnter(StateCharacter* character) override {
        // 进入空闲状态时的处理
        character->setMoveSpeed(0);
        // playAnimation("idle");
    }
    
    void onExit(StateCharacter* character) override {
        // 退出空闲状态时的处理
    }
    
    std::string getStateName() const override { return "idle"; }
    
    bool canTransitionTo(const std::string& newState) const override {
        // 空闲状态可以转换到任何其他状态
        return newState == "walking" || newState == "jumping" || 
               newState == "attacking" || newState == "casting";
    }
};

// 具体状态 - 行走状态
class WalkingState : public CharacterState {
private:
    float walkSpeed = 100.0f;
    
public:
    void handleInput(StateCharacter* character, int inputCode) override {
        switch (inputCode) {
            case 65: // A键 - 向左移动
                character->move(-walkSpeed * 0.016f, 0);  // 假设60FPS
                break;
            case 68: // D键 - 向右移动
                character->move(walkSpeed * 0.016f, 0);
                break;
            case 32: // 空格键 - 跳跃
                if (character->getIsGrounded()) {
                    character->setState("jumping");
                }
                break;
            case 74: // J键 - 攻击
                character->setState("attacking");
                break;
            case 0: // 没有输入
                character->setState("idle");
                break;
        }
    }
    
    void update(StateCharacter* character, float deltaTime) override {
        // 检查是否还在移动
        // 如果没有持续的移动输入，转换到空闲状态
    }
    
    void onEnter(StateCharacter* character) override {
        character->setMoveSpeed(walkSpeed);
        // playAnimation("walk");
    }
    
    void onExit(StateCharacter* character) override {
        character->setMoveSpeed(0);
    }
    
    std::string getStateName() const override { return "walking"; }
    
    bool canTransitionTo(const std::string& newState) const override {
        return newState == "idle" || newState == "jumping" || newState == "attacking";
    }
};

// 具体状态 - 跳跃状态
class JumpingState : public CharacterState {
private:
    float jumpForce = 300.0f;
    float gravity = -500.0f;
    
public:
    void handleInput(StateCharacter* character, int inputCode) override {
        // 跳跃中仍可以左右移动
        switch (inputCode) {
            case 65: // A键
                character->move(-50 * 0.016f, 0);  // 空中移动速度较慢
                break;
            case 68: // D键
                character->move(50 * 0.016f, 0);
                break;
            case 74: // J键 - 空中攻击
                character->setState("attacking");
                break;
        }
    }
    
    void update(StateCharacter* character, float deltaTime) override {
        // 应用重力
        float currentVelocity = character->getJumpVelocity();
        currentVelocity += gravity * deltaTime;
        character->setJumpVelocity(currentVelocity);
        
        // 更新Y位置
        character->move(0, currentVelocity * deltaTime);
        
        // 检查是否落地
        if (character->getY() <= 0) {  // 假设地面Y坐标为0
            character->setPosition(character->getX(), 0);
            character->setGrounded(true);
            character->setJumpVelocity(0);
            character->setState("idle");
        }
    }
    
    void onEnter(StateCharacter* character) override {
        character->setGrounded(false);
        character->setJumpVelocity(jumpForce);
        // playAnimation("jump");
        // playSoundEffect("jump.wav");
    }
    
    void onExit(StateCharacter* character) override {
        // playAnimation("land");
    }
    
    std::string getStateName() const override { return "jumping"; }
    
    bool canTransitionTo(const std::string& newState) const override {
        return newState == "idle" || newState == "attacking";
    }
};

// 具体状态 - 攻击状态
class AttackingState : public CharacterState {
private:
    float attackDuration = 0.5f;  // 攻击持续时间
    float attackTimer = 0.0f;
    
public:
    void handleInput(StateCharacter* character, int inputCode) override {
        // 攻击期间不响应大部分输入，防止攻击被中断
        // 但可以允许特殊输入，如取消技能
    }
    
    void update(StateCharacter* character, float deltaTime) override {
        attackTimer += deltaTime;
        
        // 攻击结束后回到空闲状态
        if (attackTimer >= attackDuration) {
            if (character->getIsGrounded()) {
                character->setState("idle");
            } else {
                character->setState("jumping");
            }
        }
    }
    
    void onEnter(StateCharacter* character) override {
        attackTimer = 0.0f;
        character->setMoveSpeed(0);  // 攻击时停止移动
        // playAnimation("attack");
        // playSoundEffect("sword_swing.wav");
        
        // 执行攻击逻辑
        performAttack(character);
    }
    
    void onExit(StateCharacter* character) override {
        // 攻击状态结束
    }
    
    std::string getStateName() const override { return "attacking"; }
    
    bool canTransitionTo(const std::string& newState) const override {
        // 攻击状态只能转换到空闲或跳跃状态
        return newState == "idle" || newState == "jumping";
    }
    
private:
    void performAttack(StateCharacter* character) {
        // 执行攻击逻辑，如检测碰撞、造成伤害等
        // detectAttackCollision(character);
    }
};

// 具体状态 - 施法状态
class CastingState : public CharacterState {
private:
    float castDuration = 1.0f;  // 施法时间
    float castTimer = 0.0f;
    int manaCost = 10;
    
public:
    void handleInput(StateCharacter* character, int inputCode) override {
        // 施法可以被移动打断
        if (inputCode == 65 || inputCode == 68) {  // A或D键
            character->setState("walking");
        }
    }
    
    void update(StateCharacter* character, float deltaTime) override {
        castTimer += deltaTime;
        
        if (castTimer >= castDuration) {
            // 施法完成
            castSpell(character);
            character->setState("idle");
        }
    }
    
    void onEnter(StateCharacter* character) override {
        castTimer = 0.0f;
        character->setMoveSpeed(0);
        
        // 检查魔法值是否足够
        if (character->getMana() < manaCost) {
            character->setState("idle");
            return;
        }
        
        // playAnimation("cast");
        // playSoundEffect("spell_charge.wav");
    }
    
    void onExit(StateCharacter* character) override {
        if (castTimer < castDuration) {
            // 施法被打断，不消耗魔法值
            // playSoundEffect("spell_interrupted.wav");
        }
    }
    
    std::string getStateName() const override { return "casting"; }
    
    bool canTransitionTo(const std::string& newState) const override {
        return newState == "idle" || newState == "walking";
    }
    
private:
    void castSpell(StateCharacter* character) {
        // 消耗魔法值
        character->setMana(character->getMana() - manaCost);
        
        // 执行法术效果
        // createSpellEffect(character);
        // playSoundEffect("spell_cast.wav");
    }
};

// 状态工厂
class StateFactory {
public:
    static std::unique_ptr<CharacterState> createState(const std::string& stateName) {
        if (stateName == "idle") {
            return std::make_unique<IdleState>();
        } else if (stateName == "walking") {
            return std::make_unique<WalkingState>();
        } else if (stateName == "jumping") {
            return std::make_unique<JumpingState>();
        } else if (stateName == "attacking") {
            return std::make_unique<AttackingState>();
        } else if (stateName == "casting") {
            return std::make_unique<CastingState>();
        }
        return nullptr;
    }
};

// 实现之前声明的方法
void GameCharacter::initializeStates() {
    states["idle"] = StateFactory::createState("idle");
    states["walking"] = StateFactory::createState("walking");
    states["jumping"] = StateFactory::createState("jumping");
    states["attacking"] = StateFactory::createState("attacking");
    states["casting"] = StateFactory::createState("casting");
}

void GameCharacter::createStateInstance(const std::string& stateName) {
    states[stateName] = StateFactory::createState(stateName);
}

// 状态机管理器 - 用于调试和监控
class StateMachineManager {
private:
    std::vector<StateCharacter*> managedCharacters;
    
public:
    void addCharacter(StateCharacter* character) {
        managedCharacters.push_back(character);
    }
    
    void removeCharacter(StateCharacter* character) {
        managedCharacters.erase(
            std::remove(managedCharacters.begin(), managedCharacters.end(), character),
            managedCharacters.end()
        );
    }
    
    void updateAllCharacters(float deltaTime) {
        for (auto* character : managedCharacters) {
            character->update(deltaTime);
        }
    }
    
    void printCharacterStates() const {
        for (const auto* character : managedCharacters) {
            // printf("%s: %s\n", character->getName().c_str(), 
            //        character->getCurrentStateName().c_str());
        }
    }
};

// 游戏控制器 - 展示状态模式的使用
class PlayerController {
private:
    std::unique_ptr<GameCharacter> player;
    StateMachineManager stateManager;
    
public:
    PlayerController(const std::string& playerName) {
        player = std::make_unique<GameCharacter>(playerName);
        stateManager.addCharacter(player.get());
    }
    
    void handleInput(int keyCode) {
        player->handleInput(keyCode);
    }
    
    void update(float deltaTime) {
        stateManager.updateAllCharacters(deltaTime);
    }
    
    std::string getCurrentState() const {
        return player->getCurrentStateName();
    }
    
    StateCharacter* getPlayer() const {
        return player.get();
    }
    
    void simulateGameplay() {
        // 模拟一系列游戏输入
        handleInput(68);  // D键 - 开始向右移动
        update(0.1f);
        
        handleInput(32);  // 空格键 - 跳跃
        update(0.5f);     // 更新跳跃过程
        
        handleInput(74);  // J键 - 攻击
        update(0.6f);     // 等待攻击完成
        
        handleInput(75);  // K键 - 施法
        update(1.1f);     // 等待施法完成
        
        handleInput(0);   // 无输入 - 回到空闲
        update(0.1f);
    }
};