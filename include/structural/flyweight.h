#pragma once
#include <unordered_map>
#include <memory>
#include <string>

/**
 * 享元模式 (Flyweight Pattern)
 * 游戏开发中常用于：大量相似对象的内存优化，如粒子系统、瓦片地图等
 * 特点：通过共享有效地支持大量细粒度对象
 */

// 享元接口 - 精灵纹理
class SpriteTexture {
public:
    virtual ~SpriteTexture() = default;
    
    // 内部状态：纹理数据（共享）
    virtual void render(float x, float y, float scale, float rotation) const = 0;
    virtual std::string getTextureName() const = 0;
    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;
};

// 具体享元 - 具体纹理实现
class ConcreteTexture : public SpriteTexture {
private:
    std::string textureName;  // 内部状态：纹理名称
    int width, height;        // 内部状态：纹理尺寸
    // 实际的纹理数据也是内部状态（这里简化）
    
public:
    ConcreteTexture(const std::string& name, int w, int h) 
        : textureName(name), width(w), height(h) {}
    
    void render(float x, float y, float scale, float rotation) const override {
        // 使用传入的外部状态（位置、缩放、旋转）进行渲染
        // 渲染逻辑使用内部状态（纹理数据）
        // renderTextureAtPosition(textureName, x, y, scale, rotation);
    }
    
    std::string getTextureName() const override { return textureName; }
    int getWidth() const override { return width; }
    int getHeight() const override { return height; }
};

// 享元工厂 - 纹理管理器
class TextureManager {
private:
    static std::unique_ptr<TextureManager> instance;
    std::unordered_map<std::string, std::shared_ptr<SpriteTexture>> texturePool;
    
    TextureManager() = default;
    
public:
    static TextureManager& getInstance() {
        if (!instance) {
            instance = std::unique_ptr<TextureManager>(new TextureManager());
        }
        return *instance;
    }
    
    // 获取享元对象（如果不存在则创建）
    std::shared_ptr<SpriteTexture> getTexture(const std::string& textureName) {
        auto it = texturePool.find(textureName);
        if (it != texturePool.end()) {
            return it->second;  // 返回已存在的享元
        }
        
        // 创建新的享元对象
        // 这里简化处理，实际应该从文件加载
        int width = 64, height = 64;  // 默认尺寸
        auto texture = std::make_shared<ConcreteTexture>(textureName, width, height);
        texturePool[textureName] = texture;
        
        return texture;
    }
    
    // 获取当前缓存的纹理数量
    size_t getTextureCount() const {
        return texturePool.size();
    }
    
    // 清理未使用的纹理
    void cleanup() {
        auto it = texturePool.begin();
        while (it != texturePool.end()) {
            if (it->second.use_count() == 1) {  // 只有管理器持有引用
                it = texturePool.erase(it);
            } else {
                ++it;
            }
        }
    }
};

std::unique_ptr<TextureManager> TextureManager::instance = nullptr;

// 上下文类 - 精灵对象（包含外部状态）
class Sprite {
private:
    std::shared_ptr<SpriteTexture> texture;  // 享元引用
    
    // 外部状态：每个精灵独有的状态
    float x, y;           // 位置
    float scale;          // 缩放
    float rotation;       // 旋转角度
    float velocityX, velocityY;  // 速度
    
public:
    Sprite(const std::string& textureName, float posX = 0, float posY = 0) 
        : x(posX), y(posY), scale(1.0f), rotation(0.0f), velocityX(0), velocityY(0) {
        // 从享元工厂获取纹理（享元对象）
        texture = TextureManager::getInstance().getTexture(textureName);
    }
    
    void update(float deltaTime) {
        // 更新外部状态
        x += velocityX * deltaTime;
        y += velocityY * deltaTime;
        rotation += 90.0f * deltaTime;  // 每秒旋转90度
    }
    
    void render() const {
        // 调用享元对象的方法，传入外部状态
        texture->render(x, y, scale, rotation);
    }
    
    // 外部状态的访问器
    void setPosition(float posX, float posY) { x = posX; y = posY; }
    void setScale(float s) { scale = s; }
    void setVelocity(float vx, float vy) { velocityX = vx; velocityY = vy; }
    
    float getX() const { return x; }
    float getY() const { return y; }
    std::string getTextureName() const { return texture->getTextureName(); }
};

// 粒子系统 - 享元模式的典型应用
class ParticleSystem {
private:
    std::vector<std::unique_ptr<Sprite>> particles;
    std::string particleTextureName;
    
public:
    ParticleSystem(const std::string& textureName) : particleTextureName(textureName) {}
    
    void emitParticle(float x, float y) {
        // 创建新粒子，所有粒子共享同一个纹理享元
        auto particle = std::make_unique<Sprite>(particleTextureName, x, y);
        
        // 设置随机的外部状态
        particle->setVelocity(
            (rand() % 200 - 100) / 10.0f,  // 随机水平速度
            (rand() % 200 - 100) / 10.0f   // 随机垂直速度
        );
        particle->setScale(0.5f + (rand() % 100) / 200.0f);  // 随机缩放
        
        particles.push_back(std::move(particle));
    }
    
    void update(float deltaTime) {
        for (auto& particle : particles) {
            particle->update(deltaTime);
        }
        
        // 移除超出屏幕的粒子
        particles.erase(
            std::remove_if(particles.begin(), particles.end(),
                [](const std::unique_ptr<Sprite>& p) {
                    return p->getX() < -100 || p->getX() > 1920 || 
                           p->getY() < -100 || p->getY() > 1080;
                }),
            particles.end()
        );
    }
    
    void render() const {
        for (const auto& particle : particles) {
            particle->render();
        }
    }
    
    size_t getParticleCount() const { return particles.size(); }
};

// 瓦片地图 - 另一个享元模式应用
class TileMap {
private:
    struct TileData {
        std::shared_ptr<SpriteTexture> texture;  // 享元引用
        int gridX, gridY;  // 外部状态：网格位置
    };
    
    std::vector<std::vector<TileData>> tiles;
    int mapWidth, mapHeight;
    int tileSize;
    
public:
    TileMap(int width, int height, int tileSizePx = 32) 
        : mapWidth(width), mapHeight(height), tileSize(tileSizePx) {
        tiles.resize(height, std::vector<TileData>(width));
    }
    
    void setTile(int x, int y, const std::string& textureName) {
        if (x >= 0 && x < mapWidth && y >= 0 && y < mapHeight) {
            tiles[y][x].texture = TextureManager::getInstance().getTexture(textureName);
            tiles[y][x].gridX = x;
            tiles[y][x].gridY = y;
        }
    }
    
    void render(float cameraX, float cameraY) const {
        for (int y = 0; y < mapHeight; ++y) {
            for (int x = 0; x < mapWidth; ++x) {
                if (tiles[y][x].texture) {
                    float worldX = x * tileSize - cameraX;
                    float worldY = y * tileSize - cameraY;
                    
                    // 只渲染在屏幕范围内的瓦片
                    if (worldX >= -tileSize && worldX <= 1920 + tileSize &&
                        worldY >= -tileSize && worldY <= 1080 + tileSize) {
                        tiles[y][x].texture->render(worldX, worldY, 1.0f, 0.0f);
                    }
                }
            }
        }
    }
    
    void generateRandomMap() {
        std::vector<std::string> tileTypes = {
            "grass.png", "stone.png", "water.png", "sand.png"
        };
        
        for (int y = 0; y < mapHeight; ++y) {
            for (int x = 0; x < mapWidth; ++x) {
                int randomIndex = rand() % tileTypes.size();
                setTile(x, y, tileTypes[randomIndex]);
            }
        }
    }
};

// 游戏世界 - 展示享元模式的效果
class GameWorld {
private:
    std::unique_ptr<TileMap> tileMap;
    std::unique_ptr<ParticleSystem> fireParticles;
    std::unique_ptr<ParticleSystem> waterParticles;
    std::vector<std::unique_ptr<Sprite>> enemies;
    
public:
    GameWorld() {
        // 创建100x100的瓦片地图
        tileMap = std::make_unique<TileMap>(100, 100);
        tileMap->generateRandomMap();
        
        // 创建粒子系统
        fireParticles = std::make_unique<ParticleSystem>("fire_particle.png");
        waterParticles = std::make_unique<ParticleSystem>("water_particle.png");
        
        // 创建大量敌人，它们共享相同的纹理享元
        for (int i = 0; i < 1000; ++i) {
            auto enemy = std::make_unique<Sprite>("goblin.png", 
                                                 rand() % 3200, rand() % 3200);
            enemy->setVelocity((rand() % 100 - 50) / 10.0f, 
                              (rand() % 100 - 50) / 10.0f);
            enemies.push_back(std::move(enemy));
        }
    }
    
    void update(float deltaTime) {
        // 发射粒子
        for (int i = 0; i < 5; ++i) {
            fireParticles->emitParticle(500 + rand() % 100, 300 + rand() % 100);
            waterParticles->emitParticle(700 + rand() % 100, 300 + rand() % 100);
        }
        
        // 更新所有对象
        fireParticles->update(deltaTime);
        waterParticles->update(deltaTime);
        
        for (auto& enemy : enemies) {
            enemy->update(deltaTime);
        }
    }
    
    void render(float cameraX, float cameraY) const {
        tileMap->render(cameraX, cameraY);
        fireParticles->render();
        waterParticles->render();
        
        for (const auto& enemy : enemies) {
            enemy->render();
        }
    }
    
    void printMemoryUsage() const {
        size_t textureCount = TextureManager::getInstance().getTextureCount();
        size_t totalSprites = enemies.size() + 
                             fireParticles->getParticleCount() + 
                             waterParticles->getParticleCount() + 
                             100 * 100;  // 瓦片数量
        
        // 在控制台输出内存使用情况
        // printf("纹理享元数量: %zu\n", textureCount);
        // printf("精灵对象总数: %zu\n", totalSprites);
        // printf("内存节省比例: %.2f%%\n", 
        //        (1.0f - (float)textureCount / totalSprites) * 100);
    }
};