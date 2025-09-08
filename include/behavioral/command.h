#pragma once
#include <memory>
#include <vector>
#include <stack>
#include <string>

/**
 * 命令模式 (Command Pattern)
 * 游戏开发中常用于：输入处理、撤销/重做系统、宏命令、AI指令等
 * 特点：将请求封装为对象，从而支持撤销、排队等操作
 */

// 命令接口
class Command {
public:
    virtual ~Command() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual std::string getName() const = 0;
};

// 接收者 - 游戏角色
class GameCharacter {
private:
    float x, y;
    int health;
    std::string name;
    
public:
    GameCharacter(const std::string& charName, float posX = 0, float posY = 0, int hp = 100)
        : x(posX), y(posY), health(hp), name(charName) {}
    
    // 移动相关
    void moveUp(float distance) { y += distance; }
    void moveDown(float distance) { y -= distance; }
    void moveLeft(float distance) { x -= distance; }
    void moveRight(float distance) { x += distance; }
    
    // 战斗相关
    void attack() { /* 攻击逻辑 */ }
    void defend() { /* 防御逻辑 */ }
    void useSkill(const std::string& skillName) { /* 使用技能 */ }
    
    // 生命值操作
    void takeDamage(int damage) { health -= damage; }
    void heal(int amount) { health += amount; }
    
    // 访问器
    float getX() const { return x; }
    float getY() const { return y; }
    int getHealth() const { return health; }
    const std::string& getName() const { return name; }
    
    void setPosition(float posX, float posY) { x = posX; y = posY; }
};

// 具体命令 - 移动命令
class MoveCommand : public Command {
private:
    GameCharacter* character;
    float deltaX, deltaY;
    
public:
    MoveCommand(GameCharacter* char_ptr, float dx, float dy) 
        : character(char_ptr), deltaX(dx), deltaY(dy) {}
    
    void execute() override {
        character->setPosition(
            character->getX() + deltaX,
            character->getY() + deltaY
        );
    }
    
    void undo() override {
        character->setPosition(
            character->getX() - deltaX,
            character->getY() - deltaY
        );
    }
    
    std::string getName() const override {
        return "移动命令 (" + std::to_string(deltaX) + ", " + std::to_string(deltaY) + ")";
    }
};

// 具体命令 - 攻击命令
class AttackCommand : public Command {
private:
    GameCharacter* attacker;
    GameCharacter* target;
    int damage;
    bool executed;
    
public:
    AttackCommand(GameCharacter* att, GameCharacter* tgt, int dmg)
        : attacker(att), target(tgt), damage(dmg), executed(false) {}
    
    void execute() override {
        if (!executed) {
            attacker->attack();
            target->takeDamage(damage);
            executed = true;
        }
    }
    
    void undo() override {
        if (executed) {
            target->heal(damage);
            executed = false;
        }
    }
    
    std::string getName() const override {
        return attacker->getName() + " 攻击 " + target->getName();
    }
};

// 具体命令 - 技能命令
class SkillCommand : public Command {
private:
    GameCharacter* caster;
    std::string skillName;
    std::vector<GameCharacter*> targets;
    int skillEffect;
    bool executed;
    
public:
    SkillCommand(GameCharacter* char_ptr, const std::string& skill, 
                const std::vector<GameCharacter*>& tgts, int effect)
        : caster(char_ptr), skillName(skill), targets(tgts), skillEffect(effect), executed(false) {}
    
    void execute() override {
        if (!executed) {
            caster->useSkill(skillName);
            
            // 根据技能类型应用效果
            if (skillName == "治疗术") {
                for (auto* target : targets) {
                    target->heal(skillEffect);
                }
            } else if (skillName == "火球术") {
                for (auto* target : targets) {
                    target->takeDamage(skillEffect);
                }
            }
            executed = true;
        }
    }
    
    void undo() override {
        if (executed) {
            // 反向应用效果
            if (skillName == "治疗术") {
                for (auto* target : targets) {
                    target->takeDamage(skillEffect);  // 撤销治疗
                }
            } else if (skillName == "火球术") {
                for (auto* target : targets) {
                    target->heal(skillEffect);  // 撤销伤害
                }
            }
            executed = false;
        }
    }
    
    std::string getName() const override {
        return caster->getName() + " 使用 " + skillName;
    }
};

// 宏命令 - 组合多个命令
class MacroCommand : public Command {
private:
    std::vector<std::unique_ptr<Command>> commands;
    std::string macroName;
    
public:
    MacroCommand(const std::string& name) : macroName(name) {}
    
    void addCommand(std::unique_ptr<Command> command) {
        commands.push_back(std::move(command));
    }
    
    void execute() override {
        for (auto& command : commands) {
            command->execute();
        }
    }
    
    void undo() override {
        // 逆序撤销所有命令
        for (auto it = commands.rbegin(); it != commands.rend(); ++it) {
            (*it)->undo();
        }
    }
    
    std::string getName() const override {
        return "宏命令: " + macroName;
    }
};

// 调用者 - 命令管理器
class CommandManager {
private:
    std::stack<std::unique_ptr<Command>> undoStack;
    std::stack<std::unique_ptr<Command>> redoStack;
    size_t maxHistorySize;
    
public:
    CommandManager(size_t maxSize = 50) : maxHistorySize(maxSize) {}
    
    void executeCommand(std::unique_ptr<Command> command) {
        // 执行命令
        command->execute();
        
        // 添加到撤销栈
        undoStack.push(std::move(command));
        
        // 清空重做栈（执行新命令后无法重做之前撤销的操作）
        while (!redoStack.empty()) {
            redoStack.pop();
        }
        
        // 限制历史记录大小
        if (undoStack.size() > maxHistorySize) {
            // 移除最底部的命令（需要临时栈）
            std::stack<std::unique_ptr<Command>> tempStack;
            while (undoStack.size() > 1) {
                tempStack.push(std::move(undoStack.top()));
                undoStack.pop();
            }
            undoStack.pop();  // 移除最底部命令
            
            while (!tempStack.empty()) {
                undoStack.push(std::move(tempStack.top()));
                tempStack.pop();
            }
        }
    }
    
    bool undo() {
        if (!undoStack.empty()) {
            auto command = std::move(undoStack.top());
            undoStack.pop();
            
            command->undo();
            redoStack.push(std::move(command));
            return true;
        }
        return false;
    }
    
    bool redo() {
        if (!redoStack.empty()) {
            auto command = std::move(redoStack.top());
            redoStack.pop();
            
            command->execute();
            undoStack.push(std::move(command));
            return true;
        }
        return false;
    }
    
    bool canUndo() const { return !undoStack.empty(); }
    bool canRedo() const { return !redoStack.empty(); }
    
    std::string getLastCommandName() const {
        if (!undoStack.empty()) {
            return undoStack.top()->getName();
        }
        return "无命令";
    }
    
    void clearHistory() {
        while (!undoStack.empty()) undoStack.pop();
        while (!redoStack.empty()) redoStack.pop();
    }
};

// 输入处理器 - 将输入转换为命令
class InputProcessor {
private:
    CommandManager* commandManager;
    GameCharacter* controlledCharacter;
    
public:
    InputProcessor(CommandManager* cmdMgr, GameCharacter* character)
        : commandManager(cmdMgr), controlledCharacter(character) {}
    
    void handleKeyPress(int keyCode) {
        std::unique_ptr<Command> command = nullptr;
        
        switch (keyCode) {
            case 87: // W键 - 向上移动
                command = std::make_unique<MoveCommand>(controlledCharacter, 0, 10);
                break;
            case 83: // S键 - 向下移动
                command = std::make_unique<MoveCommand>(controlledCharacter, 0, -10);
                break;
            case 65: // A键 - 向左移动
                command = std::make_unique<MoveCommand>(controlledCharacter, -10, 0);
                break;
            case 68: // D键 - 向右移动
                command = std::make_unique<MoveCommand>(controlledCharacter, 10, 0);
                break;
        }
        
        if (command && commandManager) {
            commandManager->executeCommand(std::move(command));
        }
    }
    
    void handleUndoRedo(int keyCode) {
        if (!commandManager) return;
        
        if (keyCode == 90) {  // Z键 - 撤销
            commandManager->undo();
        } else if (keyCode == 89) {  // Y键 - 重做
            commandManager->redo();
        }
    }
    
    void setControlledCharacter(GameCharacter* character) {
        controlledCharacter = character;
    }
};

// AI命令队列 - AI使用命令模式
class CommandAIController {
private:
    std::vector<std::unique_ptr<Command>> commandQueue;
    GameCharacter* aiCharacter;
    
public:
    CommandAIController(GameCharacter* character) : aiCharacter(character) {}
    
    void planActions(const std::vector<GameCharacter*>& enemies) {
        // AI规划行动，生成命令序列
        for (auto* enemy : enemies) {
            // 如果敌人在攻击范围内，添加攻击命令
            if (isInRange(enemy)) {
                auto attackCmd = std::make_unique<AttackCommand>(aiCharacter, enemy, 25);
                commandQueue.push_back(std::move(attackCmd));
            } else {
                // 否则移动接近敌人
                auto moveCmd = createMoveTowardsCommand(enemy);
                commandQueue.push_back(std::move(moveCmd));
            }
        }
    }
    
    void executeNextAction(CommandManager* commandManager) {
        if (!commandQueue.empty() && commandManager) {
            auto command = std::move(commandQueue.front());
            commandQueue.erase(commandQueue.begin());
            commandManager->executeCommand(std::move(command));
        }
    }
    
    bool hasActionsQueued() const {
        return !commandQueue.empty();
    }
    
    void clearQueue() {
        commandQueue.clear();
    }
    
private:
    bool isInRange(GameCharacter* target) const {
        float dx = aiCharacter->getX() - target->getX();
        float dy = aiCharacter->getY() - target->getY();
        float distance = std::sqrt(dx * dx + dy * dy);
        return distance <= 50.0f;  // 攻击范围50像素
    }
    
    std::unique_ptr<Command> createMoveTowardsCommand(GameCharacter* target) {
        float dx = target->getX() - aiCharacter->getX();
        float dy = target->getY() - aiCharacter->getY();
        
        // 标准化移动向量
        float length = std::sqrt(dx * dx + dy * dy);
        if (length > 0) {
            dx = (dx / length) * 20;  // 移动速度20像素
            dy = (dy / length) * 20;
        }
        
        return std::make_unique<MoveCommand>(aiCharacter, dx, dy);
    }
};

// 游戏战斗控制器 - 展示命令模式的完整应用
class BattleController {
private:
    CommandManager commandManager;
    InputProcessor inputProcessor;
    std::vector<std::unique_ptr<CommandAIController>> aiControllers;
    std::vector<std::unique_ptr<GameCharacter>> characters;
    
public:
    BattleController() : inputProcessor(&commandManager, nullptr) {
        // 创建玩家角色
        auto player = std::make_unique<GameCharacter>("玩家", 100, 100, 150);
        inputProcessor.setControlledCharacter(player.get());
        characters.push_back(std::move(player));
        
        // 创建AI角色
        for (int i = 0; i < 3; ++i) {
            auto enemy = std::make_unique<GameCharacter>(
                "敌人" + std::to_string(i + 1), 200 + i * 50, 200, 100);
            
            auto aiController = std::make_unique<CommandAIController>(enemy.get());
            aiControllers.push_back(std::move(aiController));
            characters.push_back(std::move(enemy));
        }
    }
    
    void handleInput(int keyCode) {
        inputProcessor.handleKeyPress(keyCode);
        inputProcessor.handleUndoRedo(keyCode);
    }
    
    void updateAI() {
        // 更新所有AI控制器
        std::vector<GameCharacter*> allCharacters;
        for (const auto& character : characters) {
            allCharacters.push_back(character.get());
        }
        
        for (auto& aiController : aiControllers) {
            if (!aiController->hasActionsQueued()) {
                aiController->planActions(allCharacters);
            }
            aiController->executeNextAction(&commandManager);
        }
    }
    
    void createComboAttack() {
        // 创建组合攻击宏命令
        auto macro = std::make_unique<MacroCommand>("连击组合");
        
        if (characters.size() >= 2) {
            auto* player = characters[0].get();
            auto* target = characters[1].get();
            
            // 添加移动接近目标的命令
            macro->addCommand(std::make_unique<MoveCommand>(player, 50, 0));
            
            // 添加攻击命令
            macro->addCommand(std::make_unique<AttackCommand>(player, target, 30));
            
            // 添加技能命令
            std::vector<GameCharacter*> targets = {target};
            macro->addCommand(std::make_unique<SkillCommand>(
                player, "火球术", targets, 25));
        }
        
        commandManager.executeCommand(std::move(macro));
    }
    
    bool canUndo() const { return commandManager.canUndo(); }
    bool canRedo() const { return commandManager.canRedo(); }
    std::string getLastCommand() const { return commandManager.getLastCommandName(); }
};