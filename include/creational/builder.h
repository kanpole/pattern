#pragma once
#include <string>
#include <vector>
#include <memory>

/**
 * 建造者模式 (Builder Pattern)
 * 游戏开发中常用于：构建复杂的游戏角色、关卡、配置等
 * 特点：将复杂对象的构建过程与表示分离，使得同样的构建过程可以创建不同的表示
 */

// 游戏角色类
class GameCharacter {
private:
    std::string name;
    int health;
    int attack;
    int defense;
    std::vector<std::string> equipment;
    std::vector<std::string> skills;
    
public:
    // Setter方法
    void setName(const std::string& n) { name = n; }
    void setHealth(int h) { health = h; }
    void setAttack(int a) { attack = a; }
    void setDefense(int d) { defense = d; }
    void addEquipment(const std::string& item) { equipment.push_back(item); }
    void addSkill(const std::string& skill) { skills.push_back(skill); }
    
    // Getter方法
    std::string getName() const { return name; }
    int getHealth() const { return health; }
    int getAttack() const { return attack; }
    int getDefense() const { return defense; }
    const std::vector<std::string>& getEquipment() const { return equipment; }
    const std::vector<std::string>& getSkills() const { return skills; }
};

// 抽象建造者
class CharacterBuilder {
protected:
    std::unique_ptr<GameCharacter> character;
    
public:
    CharacterBuilder() { reset(); }
    virtual ~CharacterBuilder() = default;
    
    void reset() { character = std::make_unique<GameCharacter>(); }
    
    // 建造步骤接口
    virtual CharacterBuilder& setBasicInfo(const std::string& name) = 0;
    virtual CharacterBuilder& setAttributes() = 0;
    virtual CharacterBuilder& addEquipment() = 0;
    virtual CharacterBuilder& addSkills() = 0;
    
    // 获取最终产品
    std::unique_ptr<GameCharacter> getCharacter() {
        return std::move(character);
    }
};

// 具体建造者 - 战士建造者
class WarriorBuilder : public CharacterBuilder {
public:
    CharacterBuilder& setBasicInfo(const std::string& name) override {
        character->setName(name);
        return *this;
    }
    
    CharacterBuilder& setAttributes() override {
        character->setHealth(150);  // 战士血量高
        character->setAttack(80);   // 攻击力中等
        character->setDefense(90);  // 防御力高
        return *this;
    }
    
    CharacterBuilder& addEquipment() override {
        character->addEquipment("重型盔甲");
        character->addEquipment("铁剑");
        character->addEquipment("盾牌");
        return *this;
    }
    
    CharacterBuilder& addSkills() override {
        character->addSkill("重击");
        character->addSkill("格挡");
        character->addSkill("冲锋");
        return *this;
    }
};

// 具体建造者 - 法师建造者
class MageBuilder : public CharacterBuilder {
public:
    CharacterBuilder& setBasicInfo(const std::string& name) override {
        character->setName(name);
        return *this;
    }
    
    CharacterBuilder& setAttributes() override {
        character->setHealth(80);   // 法师血量低
        character->setAttack(120);  // 攻击力高
        character->setDefense(40);  // 防御力低
        return *this;
    }
    
    CharacterBuilder& addEquipment() override {
        character->addEquipment("法袍");
        character->addEquipment("法杖");
        character->addEquipment("魔法书");
        return *this;
    }
    
    CharacterBuilder& addSkills() override {
        character->addSkill("火球术");
        character->addSkill("冰冻术");
        character->addSkill("治疗术");
        return *this;
    }
};

// 指挥者 - 角色创建管理器
class CharacterDirector {
public:
    // 创建标准战士
    std::unique_ptr<GameCharacter> createWarrior(const std::string& name) {
        WarriorBuilder builder;
        return builder.setBasicInfo(name)
                     .setAttributes()
                     .addEquipment()
                     .addSkills()
                     .getCharacter();
    }
    
    // 创建标准法师
    std::unique_ptr<GameCharacter> createMage(const std::string& name) {
        MageBuilder builder;
        return builder.setBasicInfo(name)
                     .setAttributes()
                     .addEquipment()
                     .addSkills()
                     .getCharacter();
    }
    
    // 创建自定义角色（演示链式调用）
    std::unique_ptr<GameCharacter> createCustomCharacter(CharacterBuilder& builder, const std::string& name) {
        return builder.setBasicInfo(name)
                     .setAttributes()
                     .addEquipment()
                     .addSkills()
                     .getCharacter();
    }
};