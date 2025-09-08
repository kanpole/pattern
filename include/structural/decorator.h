#pragma once
#include <memory>
#include <string>
#include <vector>

/**
 * 装饰器模式 (Decorator Pattern)
 * 游戏开发中常用于：角色技能增强、武器附魔、道具效果叠加等
 * 特点：动态地给对象添加新功能，而不改变其结构
 */

// 组件接口 - 游戏角色
class Character {
public:
    virtual ~Character() = default;
    virtual int getAttack() const = 0;
    virtual int getDefense() const = 0;
    virtual int getSpeed() const = 0;
    virtual std::string getDescription() const = 0;
    virtual void useSkill() = 0;
};

// 具体组件 - 基础战士
class BaseWarrior : public Character {
private:
    int baseAttack;
    int baseDefense;
    int baseSpeed;
    
public:
    BaseWarrior() : baseAttack(50), baseDefense(30), baseSpeed(20) {}
    
    int getAttack() const override { return baseAttack; }
    int getDefense() const override { return baseDefense; }
    int getSpeed() const override { return baseSpeed; }
    
    std::string getDescription() const override {
        return "基础战士";
    }
    
    void useSkill() override {
        // 基础技能
    }
};

// 装饰器基类
class CharacterDecorator : public Character {
protected:
    std::unique_ptr<Character> character;
    
public:
    CharacterDecorator(std::unique_ptr<Character> char_ptr) 
        : character(std::move(char_ptr)) {}
    
    int getAttack() const override {
        return character->getAttack();
    }
    
    int getDefense() const override {
        return character->getDefense();
    }
    
    int getSpeed() const override {
        return character->getSpeed();
    }
    
    std::string getDescription() const override {
        return character->getDescription();
    }
    
    void useSkill() override {
        character->useSkill();
    }
};

// 具体装饰器 - 武器装饰器
class WeaponDecorator : public CharacterDecorator {
private:
    std::string weaponName;
    int attackBonus;
    
public:
    WeaponDecorator(std::unique_ptr<Character> char_ptr, const std::string& weapon, int bonus)
        : CharacterDecorator(std::move(char_ptr)), weaponName(weapon), attackBonus(bonus) {}
    
    int getAttack() const override {
        return CharacterDecorator::getAttack() + attackBonus;
    }
    
    std::string getDescription() const override {
        return CharacterDecorator::getDescription() + " + " + weaponName;
    }
    
    void useSkill() override {
        CharacterDecorator::useSkill();
        // 武器特殊技能
    }
};

// 具体装饰器 - 护甲装饰器
class ArmorDecorator : public CharacterDecorator {
private:
    std::string armorName;
    int defenseBonus;
    
public:
    ArmorDecorator(std::unique_ptr<Character> char_ptr, const std::string& armor, int bonus)
        : CharacterDecorator(std::move(char_ptr)), armorName(armor), defenseBonus(bonus) {}
    
    int getDefense() const override {
        return CharacterDecorator::getDefense() + defenseBonus;
    }
    
    std::string getDescription() const override {
        return CharacterDecorator::getDescription() + " + " + armorName;
    }
};

// 具体装饰器 - 敏捷药水装饰器
class SpeedPotionDecorator : public CharacterDecorator {
private:
    int speedBonus;
    int duration;  // 持续时间
    
public:
    SpeedPotionDecorator(std::unique_ptr<Character> char_ptr, int bonus, int time)
        : CharacterDecorator(std::move(char_ptr)), speedBonus(bonus), duration(time) {}
    
    int getSpeed() const override {
        return CharacterDecorator::getSpeed() + speedBonus;
    }
    
    std::string getDescription() const override {
        return CharacterDecorator::getDescription() + " + 敏捷药水";
    }
    
    void useSkill() override {
        CharacterDecorator::useSkill();
        // 敏捷药水特效
    }
    
    int getDuration() const { return duration; }
    void reduceDuration(int time) { duration -= time; }
};

// 具体装饰器 - 魔法附魔装饰器
class MagicEnchantmentDecorator : public CharacterDecorator {
private:
    std::string enchantmentType;
    int magicAttackBonus;
    int magicDefenseBonus;
    
public:
    MagicEnchantmentDecorator(std::unique_ptr<Character> char_ptr, 
                             const std::string& type, int attackBonus, int defenseBonus)
        : CharacterDecorator(std::move(char_ptr)), 
          enchantmentType(type), 
          magicAttackBonus(attackBonus), 
          magicDefenseBonus(defenseBonus) {}
    
    int getAttack() const override {
        return CharacterDecorator::getAttack() + magicAttackBonus;
    }
    
    int getDefense() const override {
        return CharacterDecorator::getDefense() + magicDefenseBonus;
    }
    
    std::string getDescription() const override {
        return CharacterDecorator::getDescription() + " + " + enchantmentType + "附魔";
    }
    
    void useSkill() override {
        CharacterDecorator::useSkill();
        // 魔法附魔特殊效果
    }
};

// 装饰器管理器 - 管理多个装饰器效果
class DecorationManager {
private:
    std::vector<std::string> activeDecorations;
    
public:
    // 应用武器装饰
    std::unique_ptr<Character> applyWeapon(std::unique_ptr<Character> character, 
                                          const std::string& weaponName, int attackBonus) {
        activeDecorations.push_back("武器: " + weaponName);
        return std::make_unique<WeaponDecorator>(std::move(character), weaponName, attackBonus);
    }
    
    // 应用护甲装饰
    std::unique_ptr<Character> applyArmor(std::unique_ptr<Character> character, 
                                         const std::string& armorName, int defenseBonus) {
        activeDecorations.push_back("护甲: " + armorName);
        return std::make_unique<ArmorDecorator>(std::move(character), armorName, defenseBonus);
    }
    
    // 应用药水效果
    std::unique_ptr<Character> applySpeedPotion(std::unique_ptr<Character> character, 
                                               int speedBonus, int duration) {
        activeDecorations.push_back("敏捷药水");
        return std::make_unique<SpeedPotionDecorator>(std::move(character), speedBonus, duration);
    }
    
    // 应用魔法附魔
    std::unique_ptr<Character> applyMagicEnchantment(std::unique_ptr<Character> character, 
                                                    const std::string& type, 
                                                    int attackBonus, int defenseBonus) {
        activeDecorations.push_back("魔法附魔: " + type);
        return std::make_unique<MagicEnchantmentDecorator>(
            std::move(character), type, attackBonus, defenseBonus);
    }
    
    // 获取当前所有装饰效果
    const std::vector<std::string>& getActiveDecorations() const {
        return activeDecorations;
    }
    
    // 清空装饰记录
    void clearDecorations() {
        activeDecorations.clear();
    }
};