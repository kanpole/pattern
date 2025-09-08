#pragma once
#include <memory>
#include <string>
#include <vector>

/**
 * 责任链模式 (Chain of Responsibility Pattern)
 * 游戏开发中常用于：事件处理系统、UI输入处理、伤害计算链等
 * 特点：将请求的发送者和接收者解耦，让多个对象都有机会处理请求
 */

// 请求基类 - 游戏事件
class GameEvent {
public:
    enum EventType {
        INPUT_EVENT,
        COLLISION_EVENT,
        DAMAGE_EVENT,
        UI_EVENT,
        AUDIO_EVENT
    };
    
protected:
    EventType type;
    bool handled;
    
public:
    GameEvent(EventType eventType) : type(eventType), handled(false) {}
    virtual ~GameEvent() = default;
    
    EventType getType() const { return type; }
    bool isHandled() const { return handled; }
    void setHandled(bool h) { handled = h; }
};

// 具体请求 - 输入事件
class InputEvent : public GameEvent {
private:
    int keyCode;
    bool isKeyDown;
    
public:
    InputEvent(int key, bool down) 
        : GameEvent(INPUT_EVENT), keyCode(key), isKeyDown(down) {}
    
    int getKeyCode() const { return keyCode; }
    bool getKeyDown() const { return isKeyDown; }
};

// 具体请求 - 伤害事件
class DamageEvent : public GameEvent {
private:
    int damage;
    std::string damageType;
    std::string targetId;
    
public:
    DamageEvent(int dmg, const std::string& type, const std::string& target)
        : GameEvent(DAMAGE_EVENT), damage(dmg), damageType(type), targetId(target) {}
    
    int getDamage() const { return damage; }
    void setDamage(int dmg) { damage = dmg; }
    const std::string& getDamageType() const { return damageType; }
    const std::string& getTargetId() const { return targetId; }
};

// 处理者抽象基类
class EventHandler {
protected:
    std::unique_ptr<EventHandler> nextHandler;
    std::string handlerName;
    
public:
    EventHandler(const std::string& name) : handlerName(name) {}
    virtual ~EventHandler() = default;
    
    // 设置责任链中的下一个处理者
    void setNext(std::unique_ptr<EventHandler> handler) {
        nextHandler = std::move(handler);
    }
    
    // 处理请求的方法
    virtual void handleEvent(GameEvent* event) {
        if (canHandle(event)) {
            processEvent(event);
            event->setHandled(true);
        } else if (nextHandler) {
            // 传递给链中的下一个处理者
            nextHandler->handleEvent(event);
        }
    }
    
protected:
    // 判断是否能处理该事件
    virtual bool canHandle(GameEvent* event) = 0;
    
    // 实际处理事件的方法
    virtual void processEvent(GameEvent* event) = 0;
    
public:
    const std::string& getName() const { return handlerName; }
};

// 具体处理者 - 输入处理器
class InputHandler : public EventHandler {
public:
    InputHandler() : EventHandler("输入处理器") {}
    
protected:
    bool canHandle(GameEvent* event) override {
        return event->getType() == GameEvent::INPUT_EVENT;
    }
    
    void processEvent(GameEvent* event) override {
        InputEvent* inputEvent = static_cast<InputEvent*>(event);
        
        // 处理输入事件
        if (inputEvent->getKeyDown()) {
            handleKeyPress(inputEvent->getKeyCode());
        } else {
            handleKeyRelease(inputEvent->getKeyCode());
        }
    }
    
private:
    void handleKeyPress(int keyCode) {
        // 处理按键按下
        switch (keyCode) {
            case 87: // W键
                // 向前移动
                break;
            case 83: // S键
                // 向后移动
                break;
            case 65: // A键
                // 向左移动
                break;
            case 68: // D键
                // 向右移动
                break;
        }
    }
    
    void handleKeyRelease(int keyCode) {
        // 处理按键释放
    }
};

// 具体处理者 - 伤害处理器
class DamageHandler : public EventHandler {
public:
    DamageHandler() : EventHandler("伤害处理器") {}
    
protected:
    bool canHandle(GameEvent* event) override {
        return event->getType() == GameEvent::DAMAGE_EVENT;
    }
    
    void processEvent(GameEvent* event) override {
        DamageEvent* damageEvent = static_cast<DamageEvent*>(event);
        
        // 应用伤害修正
        int finalDamage = calculateFinalDamage(damageEvent);
        damageEvent->setDamage(finalDamage);
        
        // 应用伤害
        applyDamage(damageEvent);
    }
    
private:
    int calculateFinalDamage(DamageEvent* event) {
        int baseDamage = event->getDamage();
        
        // 根据伤害类型应用修正
        if (event->getDamageType() == "fire") {
            return static_cast<int>(baseDamage * 1.2f);  // 火焰伤害+20%
        } else if (event->getDamageType() == "ice") {
            return static_cast<int>(baseDamage * 0.8f);  // 冰霜伤害-20%
        }
        
        return baseDamage;
    }
    
    void applyDamage(DamageEvent* event) {
        // 对目标应用伤害
        // findTarget(event->getTargetId())->takeDamage(event->getDamage());
    }
};

// 具体处理者 - 防御处理器
class DefenseHandler : public EventHandler {
public:
    DefenseHandler() : EventHandler("防御处理器") {}
    
protected:
    bool canHandle(GameEvent* event) override {
        // 只处理伤害事件，用于计算防御减免
        return event->getType() == GameEvent::DAMAGE_EVENT;
    }
    
    void processEvent(GameEvent* event) override {
        DamageEvent* damageEvent = static_cast<DamageEvent*>(event);
        
        // 计算防御减免
        int defense = getTargetDefense(damageEvent->getTargetId());
        int reducedDamage = std::max(1, damageEvent->getDamage() - defense);
        
        damageEvent->setDamage(reducedDamage);
        
        // 不设置为已处理，让后续处理者继续处理
    }
    
private:
    int getTargetDefense(const std::string& targetId) {
        // 获取目标的防御值
        // return findTarget(targetId)->getDefense();
        return 10;  // 示例值
    }
};

// 具体处理者 - 特殊效果处理器
class SpecialEffectHandler : public EventHandler {
public:
    SpecialEffectHandler() : EventHandler("特殊效果处理器") {}
    
protected:
    bool canHandle(GameEvent* event) override {
        return event->getType() == GameEvent::DAMAGE_EVENT;
    }
    
    void processEvent(GameEvent* event) override {
        DamageEvent* damageEvent = static_cast<DamageEvent*>(event);
        
        // 应用特殊效果
        if (damageEvent->getDamageType() == "poison") {
            applyPoisonEffect(damageEvent->getTargetId());
        } else if (damageEvent->getDamageType() == "stun") {
            applyStunEffect(damageEvent->getTargetId());
        }
    }
    
private:
    void applyPoisonEffect(const std::string& targetId) {
        // 应用中毒效果
    }
    
    void applyStunEffect(const std::string& targetId) {
        // 应用眩晕效果
    }
};

// 事件管理器 - 管理责任链
class EventManager {
private:
    std::unique_ptr<EventHandler> handlerChain;
    std::vector<GameEvent*> eventQueue;
    
public:
    EventManager() {
        setupHandlerChain();
    }
    
    void setupHandlerChain() {
        // 构建责任链：输入 -> 防御 -> 特殊效果 -> 伤害
        auto inputHandler = std::make_unique<InputHandler>();
        auto defenseHandler = std::make_unique<DefenseHandler>();
        auto effectHandler = std::make_unique<SpecialEffectHandler>();
        auto damageHandler = std::make_unique<DamageHandler>();
        
        // 建立链式关系
        inputHandler->setNext(std::move(defenseHandler));
        inputHandler->setNext(std::move(effectHandler));
        effectHandler->setNext(std::move(damageHandler));
        
        handlerChain = std::move(inputHandler);
    }
    
    void addEvent(GameEvent* event) {
        eventQueue.push_back(event);
    }
    
    void processEvents() {
        for (auto* event : eventQueue) {
            if (handlerChain) {
                handlerChain->handleEvent(event);
            }
        }
        
        // 清理已处理的事件
        eventQueue.clear();
    }
    
    // 动态添加处理者到链的开头
    void addHandlerToChain(std::unique_ptr<EventHandler> newHandler) {
        newHandler->setNext(std::move(handlerChain));
        handlerChain = std::move(newHandler);
    }
};

// 具体处理者 - UI事件处理器
class UIEventHandler : public EventHandler {
public:
    UIEventHandler() : EventHandler("UI事件处理器") {}
    
protected:
    bool canHandle(GameEvent* event) override {
        return event->getType() == GameEvent::UI_EVENT;
    }
    
    void processEvent(GameEvent* event) override {
        // 处理UI事件
        // handleUIInteraction(event);
    }
};

// 游戏战斗系统 - 使用责任链处理战斗事件
class CombatSystem {
private:
    EventManager eventManager;
    
public:
    CombatSystem() {
        // 可以为战斗系统添加专门的处理者
        auto uiHandler = std::make_unique<UIEventHandler>();
        eventManager.addHandlerToChain(std::move(uiHandler));
    }
    
    void playerAttack(const std::string& targetId, int damage, const std::string& damageType) {
        // 创建伤害事件并添加到事件队列
        auto damageEvent = new DamageEvent(damage, damageType, targetId);
        eventManager.addEvent(damageEvent);
        
        // 立即处理事件
        eventManager.processEvents();
        
        delete damageEvent;
    }
    
    void handleInput(int keyCode, bool isDown) {
        auto inputEvent = new InputEvent(keyCode, isDown);
        eventManager.addEvent(inputEvent);
        eventManager.processEvents();
        
        delete inputEvent;
    }
    
    void processFrame() {
        // 每帧处理事件队列
        eventManager.processEvents();
    }
};