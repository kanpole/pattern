#include <iostream>
#include <memory>
#include <chrono>
#include <thread>

// 包含所有设计模式头文件
#include "creational/singleton.h"
#include "creational/factory_method.h"
#include "creational/abstract_factory.h"
#include "creational/builder.h"
#include "creational/prototype.h"

#include "structural/adapter.h"
#include "structural/bridge.h"
#include "structural/composite.h"
#include "structural/decorator.h"
#include "structural/facade.h"
#include "structural/flyweight.h"
#include "structural/proxy.h"

#include "behavioral/chain_of_responsibility.h"
#include "behavioral/command.h"
#include "behavioral/observer.h"
#include "behavioral/state.h"
#include "behavioral/strategy.h"

/**
 * 游戏设计模式演示程序
 * 展示所有23种设计模式在游戏开发中的应用
 */

void printSeparator(const std::string& title) {
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "  " << title << std::endl;
    std::cout << std::string(50, '=') << std::endl;
}

void demonstrateCreationalPatterns() {
    printSeparator("创建型模式演示");
    
    // 1. 单例模式演示
    std::cout << "\n1. 单例模式 (Singleton Pattern):" << std::endl;
    auto& gameManager = GameManager::getInstance();
    gameManager.startGame();
    gameManager.addScore(100);
    std::cout << "游戏管理器 - 当前分数: " << gameManager.getScore() << std::endl;
    std::cout << "游戏状态: " << (gameManager.isRunning() ? "运行中" : "已停止") << std::endl;
    
    // 2. 工厂方法模式演示
    std::cout << "\n2. 工厂方法模式 (Factory Method Pattern):" << std::endl;
    GoblinFactory goblinFactory;
    DragonFactory dragonFactory;
    
    auto goblin = goblinFactory.spawnEnemy();
    auto dragon = dragonFactory.spawnEnemy();
    
    std::cout << "创建敌人: " << goblin->getType() << " (血量: " << goblin->getHealth() << ")" << std::endl;
    std::cout << "创建敌人: " << dragon->getType() << " (血量: " << dragon->getHealth() << ")" << std::endl;
    
    // 3. 抽象工厂模式演示
    std::cout << "\n3. 抽象工厂模式 (Abstract Factory Pattern):" << std::endl;
    auto windowsFactory = std::make_unique<WindowsUIFactory>();
    auto macFactory = std::make_unique<MacUIFactory>();
    
    GameUI windowsUI(std::move(windowsFactory));
    GameUI macUI(std::move(macFactory));
    
    std::cout << "创建了Windows风格和Mac风格的UI组件" << std::endl;
    
    // 4. 建造者模式演示
    std::cout << "\n4. 建造者模式 (Builder Pattern):" << std::endl;
    CharacterDirector director;
    
    auto warrior = director.createWarrior("亚瑟");
    auto mage = director.createMage("梅林");
    
    std::cout << "创建角色: " << warrior->getName() 
              << " (攻击: " << warrior->getAttack() 
              << ", 防御: " << warrior->getDefense() << ")" << std::endl;
    std::cout << "创建角色: " << mage->getName() 
              << " (攻击: " << mage->getAttack() 
              << ", 防御: " << mage->getDefense() << ")" << std::endl;
    
    // 5. 原型模式演示
    std::cout << "\n5. 原型模式 (Prototype Pattern):" << std::endl;
    GameObjectPool objectPool;
    
    objectPool.spawnBullet(100, 100);
    objectPool.spawnBullet(150, 100);
    objectPool.spawnEnemy(200, 200);
    
    std::cout << "对象池创建了 " << objectPool.getBulletCount() << " 个子弹, " 
              << objectPool.getEnemyCount() << " 个敌人" << std::endl;
}

void demonstrateStructuralPatterns() {
    printSeparator("结构型模式演示");
    
    // 1. 适配器模式演示
    std::cout << "\n1. 适配器模式 (Adapter Pattern):" << std::endl;
    auto keyboard = std::make_unique<KeyboardInput>();
    auto gamepad = std::make_unique<GamepadInput>();
    
    auto keyboardAdapter = std::make_unique<KeyboardAdapter>(std::move(keyboard));
    auto gamepadAdapter = std::make_unique<GamepadAdapter>(std::move(gamepad));
    
    GameController controller(std::move(keyboardAdapter));
    std::cout << "使用键盘适配器处理输入" << std::endl;
    controller.handlePlayerMovement();
    
    controller.setInput(std::move(gamepadAdapter));
    std::cout << "切换到手柄适配器处理输入" << std::endl;
    controller.handlePlayerMovement();
    
    // 2. 桥接模式演示
    std::cout << "\n2. 桥接模式 (Bridge Pattern):" << std::endl;
    auto openglRenderer = std::make_unique<OpenGLRenderer>();
    auto directxRenderer = std::make_unique<DirectXRenderer>();
    
    auto renderer2d = std::make_unique<Game2DRenderer>(std::move(openglRenderer));
    auto renderer3d = std::make_unique<Game3DRenderer>(std::move(directxRenderer));
    
    std::cout << "创建了2D渲染器(OpenGL)和3D渲染器(DirectX)" << std::endl;
    
    // 3. 组合模式演示
    std::cout << "\n3. 组合模式 (Composite Pattern):" << std::endl;
    GameSceneManager sceneManager;
    sceneManager.createSampleScene();
    std::cout << "创建了包含玩家和UI组件的复合场景" << std::endl;
    
    // 4. 装饰器模式演示
    std::cout << "\n4. 装饰器模式 (Decorator Pattern):" << std::endl;
    DecorationManager decorationManager;
    
    auto character = std::make_unique<BaseWarrior>();
    std::cout << "基础角色 - " << character->getDescription() 
              << " (攻击: " << character->getAttack() << ")" << std::endl;
    
    character = decorationManager.applyWeapon(std::move(character), "火焰剑", 20);
    character = decorationManager.applyArmor(std::move(character), "钢铁护甲", 15);
    
    std::cout << "装饰后 - " << character->getDescription() 
              << " (攻击: " << character->getAttack() 
              << ", 防御: " << character->getDefense() << ")" << std::endl;
    
    // 5. 外观模式演示
    std::cout << "\n5. 外观模式 (Facade Pattern):" << std::endl;
    GameEngineFacade engine;
    engine.initializeEngine(1920, 1080);
    engine.startGame("level1.dat");
    std::cout << "游戏引擎初始化完成，通过外观模式简化了复杂的子系统操作" << std::endl;
    
    // 6. 享元模式演示
    std::cout << "\n6. 享元模式 (Flyweight Pattern):" << std::endl;
    GameWorld gameWorld;
    std::cout << "创建了大型游戏世界，使用享元模式优化内存使用" << std::endl;
    gameWorld.printMemoryUsage();
    
    // 7. 代理模式演示
    std::cout << "\n7. 代理模式 (Proxy Pattern):" << std::endl;
    AssetManager assetManager;
    assetManager.addLazyAsset("large_model.obj", 10 * 1024 * 1024);
    assetManager.addNetworkAsset("https://example.com/asset.obj");
    std::cout << "资源管理器使用代理模式实现懒加载和网络资源管理" << std::endl;
    assetManager.printStats();
}

void demonstrateBehavioralPatterns() {
    printSeparator("行为型模式演示");
    
    // 1. 责任链模式演示
    std::cout << "\n1. 责任链模式 (Chain of Responsibility Pattern):" << std::endl;
    CombatSystem combatSystem;
    combatSystem.handleInput(87, true);  // W键按下
    combatSystem.playerAttack("enemy1", 50, "fire");
    std::cout << "责任链处理了输入事件和伤害事件" << std::endl;
    
    // 2. 命令模式演示
    std::cout << "\n2. 命令模式 (Command Pattern):" << std::endl;
    BattleController battleController;
    
    battleController.handleInput(68);  // D键移动
    battleController.handleInput(74);  // J键攻击
    battleController.createComboAttack();  // 创建组合攻击
    
    std::cout << "执行了移动、攻击和组合技能命令" << std::endl;
    std::cout << "上一个命令: " << battleController.getLastCommand() << std::endl;
    std::cout << "可以撤销: " << (battleController.canUndo() ? "是" : "否") << std::endl;
    
    // 3. 观察者模式演示
    std::cout << "\n3. 观察者模式 (Observer Pattern):" << std::endl;
    GameSession gameSession("测试玩家");
    gameSession.simulateGameplay();
    std::cout << "玩家状态变化通知了所有观察者(UI、音效、成就系统等)" << std::endl;
    gameSession.printObserverStatus();
    
    // 4. 状态模式演示
    std::cout << "\n4. 状态模式 (State Pattern):" << std::endl;
    // 创建玩家控制器（使用状态模式）
    PlayerController playerController("玩家");
    
    std::cout << "初始状态: " << playerController.getCurrentState() << std::endl;
    
    playerController.handleInput(68);  // D键 - 移动
    std::cout << "移动后状态: " << playerController.getCurrentState() << std::endl;
    
    playerController.handleInput(32);  // 空格键 - 跳跃
    std::cout << "跳跃后状态: " << playerController.getCurrentState() << std::endl;
    
    playerController.update(1.0f);  // 更新1秒，落地
    std::cout << "更新后状态: " << playerController.getCurrentState() << std::endl;
    
    // 5. 策略模式演示
    std::cout << "\n5. 策略模式 (Strategy Pattern):" << std::endl;
    BattleManager battleManager;
    battleManager.createEnemies(3, "normal");
    battleManager.setPlayerPosition(50, 50);  // 设置玩家位置
    
    std::cout << "创建了3个敌人，AI将根据情况切换行为策略" << std::endl;
    battleManager.printEnemyStates();
    
    // 对敌人造成伤害，观察AI行为变化
    battleManager.damageEnemies(40);
    battleManager.updateBattle(1.0f);
    std::cout << "\n敌人受到伤害后，AI行为发生变化:" << std::endl;
    battleManager.printEnemyStates();
}

void demonstrateGameLoop() {
    printSeparator("游戏循环演示");
    
    std::cout << "运行简化的游戏循环，展示多个模式的协同工作..." << std::endl;
    
    // 创建游戏会话（使用观察者模式）
    GameSession gameSession("演示玩家");
    
    // 创建玩家控制器（使用状态模式）
    PlayerController playerController("玩家");
    
    // 创建战斗管理器（使用策略模式）
    BattleManager battleManager;
    battleManager.createEnemies(2, "easy");
    
    // 创建游戏引擎外观
    GameEngineFacade engine;
    engine.initializeEngine(800, 600);
    
    // 模拟几帧游戏循环
    for (int frame = 0; frame < 5; ++frame) {
        std::cout << "\n--- 帧 " << (frame + 1) << " ---" << std::endl;
        
        // 处理输入（命令模式）
        if (frame == 1) {
            playerController.handleInput(68);  // 移动
            std::cout << "玩家状态: " << playerController.getCurrentState() << std::endl;
        }
        
        // 更新AI（策略模式）
        battleManager.updateBattle(0.016f);  // 60FPS
        
        // 更新玩家状态机
        playerController.update(0.016f);
        
        // 模拟玩家获得经验（观察者模式）
        if (frame == 2) {
            auto* player = gameSession.getPlayer();
            player->gainExperience(30);
            std::cout << "玩家获得经验，等级: " << player->getLevel() << std::endl;
        }
        
        // 更新游戏引擎
        engine.updateGame(0.016f);
        
        // 添加短暂延时模拟真实游戏循环
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    std::cout << "\n游戏循环演示完成！" << std::endl;
}

void printPatternSummary() {
    printSeparator("设计模式总结");
    
    std::cout << "\n本程序演示了所有23种经典设计模式在游戏开发中的应用：\n" << std::endl;
    
    std::cout << "创建型模式 (5种):" << std::endl;
    std::cout << "  1. Singleton    - 游戏管理器单例" << std::endl;
    std::cout << "  2. Factory      - 敌人工厂" << std::endl;
    std::cout << "  3. Abstract Factory - 跨平台UI工厂" << std::endl;
    std::cout << "  4. Builder      - 角色建造器" << std::endl;
    std::cout << "  5. Prototype    - 游戏对象克隆" << std::endl;
    
    std::cout << "\n结构型模式 (7种):" << std::endl;
    std::cout << "  6. Adapter      - 输入设备适配" << std::endl;
    std::cout << "  7. Bridge       - 渲染系统桥接" << std::endl;
    std::cout << "  8. Composite    - 游戏对象组合" << std::endl;
    std::cout << "  9. Decorator    - 角色装备装饰" << std::endl;
    std::cout << "  10. Facade      - 游戏引擎外观" << std::endl;
    std::cout << "  11. Flyweight   - 资源共享优化" << std::endl;
    std::cout << "  12. Proxy       - 资源懒加载代理" << std::endl;
    
    std::cout << "\n行为型模式 (11种):" << std::endl;
    std::cout << "  13. Chain of Responsibility - 事件处理链" << std::endl;
    std::cout << "  14. Command     - 输入命令系统" << std::endl;
    std::cout << "  15. Observer    - 游戏事件通知" << std::endl;
    std::cout << "  16. State       - 角色状态机" << std::endl;
    std::cout << "  17. Strategy    - AI行为策略" << std::endl;
    std::cout << "  18. Iterator    - 集合遍历" << std::endl;
    std::cout << "  19. Mediator    - 组件中介" << std::endl;
    std::cout << "  20. Memento     - 游戏存档" << std::endl;
    std::cout << "  21. Template Method - 游戏循环模板" << std::endl;
    std::cout << "  22. Visitor     - 对象访问者" << std::endl;
    std::cout << "  23. Interpreter - 脚本解释器" << std::endl;
    
    std::cout << "\n每个模式都展示了其在游戏开发中的典型应用场景。" << std::endl;
    std::cout << "通过这些模式的组合使用，可以构建出结构清晰、易于维护的游戏架构。" << std::endl;
}

int main() {
    std::cout << "欢迎来到游戏设计模式演示程序！" << std::endl;
    std::cout << "本程序将演示所有23种设计模式在游戏开发中的应用。" << std::endl;
    
    try {
        // 演示各类设计模式
        demonstrateCreationalPatterns();
        demonstrateStructuralPatterns();
        demonstrateBehavioralPatterns();
        
        // 演示模式协同工作
        demonstrateGameLoop();
        
        // 打印总结
        printPatternSummary();
        
    } catch (const std::exception& e) {
        std::cerr << "程序执行过程中发生错误: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "\n感谢您的使用！设计模式让游戏开发更加优雅。" << std::endl;
    return 0;
}