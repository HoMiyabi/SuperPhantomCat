#include "GameScene.h"
#include "Hero.h"
#include "BrickBehavior.h"
#include "SlimeBehavior.h"
#include "CoinBehavior.h"
#include "CoinAudio.h"
#include "TiledMapBehavior.h"
#include "MushroomBehavior.h"
#include "CoverBehavior.h"
#include "TriggerBehavior.h"
#include "Definitions.h"
#include "CameraBehavior.h"
#include "HideBrickBehavior.h"
#include "ExitBehavior.h"
#include "HeartBehavior.h"
#include "CoinCountNode.h"
#include "HealthNode.h"
#include "CameraHelper.h"
#include "AudioEngine.h"

USING_NS_CC;

void GameScene::onEnterTransitionDidFinish()
{
    Scene::onEnterTransitionDidFinish();
    scheduleUpdate();
}

bool GameScene::init()
{
    if (!Scene::initWithPhysics()) {
        return false;
    }
    //getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);

    Director::getInstance()->getEventDispatcher()->removeAllEventListeners();

    //Background Music
    experimental::AudioEngine::play2d("/audio/bgm_happy_1.mp3", true, 0.7);

    constexpr float zoomRatio = 0.6f;
    auto bgViewSize = visibleSize * zoomRatio;


    //Background Part

    //Background Layer
    auto backgroundLayer = LayerGradient::create(Color4B(4, 178, 255, 255), Color4B(52, 242, 255, 255));
    addChild(backgroundLayer);
    backgroundLayer->changeWidthAndHeight(bgViewSize.width, bgViewSize.height);


    //Mountain
    auto mountain = Sprite::create("/bg/base_home_bg_mountain.png");
    backgroundLayer->addChild(mountain, 1);
    mountain->setPosition(Vec2 {0.412, 0.123} * bgViewSize);
    mountain->setColor(Color3B {255, 1, 253});


    //Cloud
    auto cloud = Sprite::create("/bg/img_cloud.png");
    backgroundLayer->addChild(cloud);
    cloud->setPosition(Vec2 {0.868, 0.927} * bgViewSize);

    //Background Camera
    auto backgroundCamera = Camera::createOrthographic(bgViewSize.width, bgViewSize.height, 0, 1);
    addChild(backgroundCamera);
    backgroundCamera->setCameraFlag(CameraFlag::USER1);

    backgroundLayer->setCameraMask(static_cast<unsigned short>(CameraFlag::USER1));


    //Game Part

    //Game Layer
    auto gameLayer = Layer::create();
    addChild(gameLayer);

    //Load TiledMap
    auto tiledMap = cocos2d::TMXTiledMap::create("tile_1_1.tmx");
    auto mapBehavior = Create<TiledMapBehavior>();
    tiledMap->addComponent(mapBehavior);
    //const Size tileSize = Size(64.0f, 64.0f);
    gameLayer->addChild(tiledMap, static_cast<int>(ZOrder::TILED_MAP), "tiledMap");


    //Coin Audio
    auto coinAudio = Create<CoinAudio>();
    gameLayer->addChild(coinAudio);
    auto layers = getLayers(tiledMap);
    for (auto layer : layers) {
        //layer->getTexture()->setAliasTexParameters();
        auto &layerName = layer->getLayerName();
        auto &layerSize = layer->getLayerSize();
        if (layerName == "Mushroom") {
            for (float y = 0; y < layerSize.height; y++) {
                for (float x = 0; x < layerSize.width; x++) {
                    auto tile = layer->getTileAt({x, y});
                    if (tile) {
                        auto behavior = Create<MushroomBehaivor>();
                        tile->addComponent(behavior);
                        behavior->set(layer, x, y, tiledMap->getLayer("LightMushroom"));
                    }
                }
            }
        } else if (layerName == "LightMushroom") {
            for (float y = 0; y < layerSize.height; y++) {
                for (float x = 0; x < layerSize.width; x++) {
                    auto tile = layer->getTileAt({x, y});
                    if (tile) {
                        tile->setVisible(false);
                    }
                }
            }
        } else if (layerName == "OneWay") {
            for (float y = 0; y < layerSize.height; y++) {
                for (float x = 0; x < layerSize.width; x++) {
                    if (layer->getTileGIDAt({x, y})) {
                        auto tile = layer->getTileAt({x, y});
                        auto collider = Collider::create({tile->getContentSize().width, 1}, {0, tile->getContentSize().height / 2});
                        tile->addComponent(collider);
                        collider->setType(Collider::TYPE::ONE_WAY);
                    }
                }
            }
        } else if (layerName == "Ground") {

            GID_TO_TYPE[177] = TYPE_QUESTION;
            GID_TO_TYPE[178] = TYPE_BRICK;
            GID_TO_TYPE[179] = TYPE_HIDE;
            for (float y = 0; y < layerSize.height; y++) {
                for (float x = 0; x < layerSize.width; x++) {
                    if (layer->getTileGIDAt({x, y})) {
                        auto tile = layer->getTileAt({x, y});
                        switch (GID_TO_TYPE[layer->getTileGIDAt({x, y})]) {
                        case TYPE_QUESTION:
                        case TYPE_BRICK: {
                            auto behavior = BrickBehavior::create();
                            tile->addComponent(behavior);
                            break;
                        }
                        case TYPE_HIDE: {
                            auto behavior = HideBrickBehavior::create();
                            tile->addComponent(behavior);
                            break;
                        }
                        default: {
                            if (x - 1 >= 0 && x + 1 < layerSize.width && y - 1 >= 0 && y + 1 < layerSize.height) {
                                if (layer->getTileGIDAt({x - 1, y}) &&
                                    layer->getTileGIDAt({x + 1, y}) &&
                                    layer->getTileGIDAt({x, y + 1}) &&
                                    layer->getTileGIDAt({x, y - 1})) {
                                    break;
                                }
                            }
                            auto collider = Collider::create(tile->getContentSize());
                            tile->addComponent(collider);
                            break;
                        }
                        }
                    }
                }
            }
        } else if (layerName == "Brick") {
            for (float y = 0; y < layerSize.height; y++) {
                for (float x = 0; x < layerSize.width; x++) {
                    if (layer->getTileGIDAt({x, y})) {
                        auto tile = layer->getTileAt({x, y});
                    }
                }
            }
        } else if (layerName == "coin") {
            for (float y = 0; y < layerSize.height; y++) {
                for (float x = 0; x < layerSize.width; x++) {
                    auto tile = layer->getTileAt({x, y});
                    if (tile) {
                        tile->addComponent(Create<CoinBehavior>());
                    }
                }
            }
        } else if (layerName == "cover") {
            for (float y = 0; y < layerSize.height; y++) {
                for (float x = 0; x < layerSize.width; x++) {
                    auto tile = layer->getTileAt({x, y});
                    if (tile) {
                        tile->addComponent(Create<CoverBehavior>());
                    }
                }
            }
        } else if (layerName == "Cover") {
            for (float y = 0; y < layerSize.height; y++) {
                for (float x = 0; x < layerSize.width; x++) {
                    auto tile = layer->getTileAt({x, y});
                    if (tile) {
                        tile->addComponent(Create<CoverBehavior>());
                    }
                }
            }
        } else if (layerName == "Heart") {
            for (float y = 0; y < layerSize.height; y++) {
                for (float x = 0; x < layerSize.width; x++) {
                    auto tile = layer->getTileAt({x, y});
                    if (tile) {
                        tile->addComponent(Create<HeartBehavior>());
                    }
                }
            }
        }
    }

    auto dataGroup = tiledMap->getObjectGroup("data");
    //Index Pos Data
    std::unordered_map<int, ValueMap> ID_TO_DATA;
    for (auto& value : dataGroup->getObjects()) {
        auto& data = value.asValueMap();
        const auto& name = data.at("name").asString();
        const int id = data.at("id").asInt();
        if (name == "pos") {
            ID_TO_DATA[id] = data;
        }
    }

    //Load Data
    for (auto& value : dataGroup->getObjects()) {
        auto& data = value.asValueMap();
        std::string name = data.at("name").asString();
        float x = data.at("x").asFloat();
        float y = data.at("y").asFloat();
        if (name == "coin") {
            auto tile = tiledMap->getLayer(name)->getTileAt(mapBehavior->getPositionInTile({x, y}));
            
            auto behavior = GetComponent<CoinBehavior>(tile);
            behavior->setData(data);
            behavior->hide();

        } else if (name == "hero") {
            hero = Hero::create("/nono/idle/img_hero_nono_idle_0_1.png");
            gameLayer->addChild(hero, static_cast<int>(ZOrder::HERO));
            hero->setPosition(x, y);


        } else if (name == "slime") {
            auto slime = Sprite::create("/slime/mons_slime_run_0.png");
            slime->setPosition(x, y);
            auto behavior = Create<SlimeBehaivor>();
            slime->addComponent(behavior);

            int destId = data.at("dest").asInt();
            auto& destData = ID_TO_DATA[destId];
            float destX = destData.at("x").asFloat();
            float destY = destData.at("y").asFloat();
            behavior->set({destX, destY});
            gameLayer->addChild(slime, 114514);
        } else if (name == "brick") {
            auto tile = tiledMap->getLayer("Ground")->getTileAt(mapBehavior->getPositionInTile({x, y}));
            GetComponent<BrickBehavior>(tile)->set(data);

        } else if (name == "Trigger") {
            auto node = Node::create();
            gameLayer->addChild(node);
            auto behavior = Create<TriggerBehavior>();
            node->addComponent(behavior);
            behavior->set(data);
        } else if (name == "cover") {
            auto sprite = tiledMap->getLayer(name)->getTileAt(mapBehavior->getPositionInTile({x, y}));
            GetComponent<CoverBehavior>(sprite)->set(data);
        } else if (name == "exit") {
            auto node = Node::create();
            auto behavior = Create<ExitBehavior>();
            node->addComponent(behavior);
            behavior->set(data);
            gameLayer->addChild(node);
        } else if (name == "Cover") {
            float width = data.at("width").asFloat();
            float height = data.at("height").asFloat();
            auto layer = tiledMap->getLayer(name);
            for (float y1 = y; y1 < y + height; y1 += tiledMap->getTileSize().height) {
                for (float x1 = x; x1 < x + width; x1 += tiledMap->getTileSize().width) {
                    auto tPos = mapBehavior->getPositionInTile({x1, y1});
                    auto tile = layer->getTileAt(tPos);
                    if (tile) {
                        log("setdata");
                        GetComponent<CoverBehavior>(tile)->set(data);
                    }
                }
            }
        }
    }

    //Game Camera
    Rect rectRange {tiledMap->getPosition(), tiledMap->getMapSize() * tiledMap->getTileSize()};
    auto gameViewSize = visibleSize * zoomRatio;
    gameCamera = Camera::createOrthographic(gameViewSize.width, gameViewSize.height, 0, 1);
    auto cameraHelper = CameraHelper::create(hero);
    addChild(cameraHelper, 0, "cameraHelper");
    cameraHelper->addChild(gameCamera);
    auto cameraBehavior = CameraBehavior::create(gameViewSize, rectRange, hero);
    gameCamera->addComponent(cameraBehavior);

    gameCamera->setCameraFlag(CameraFlag::USER2);
    gameLayer->setCameraMask(static_cast<unsigned short>(CameraFlag::USER2));
    

    //UI Part

    //UI Layer
    auto uiLayer = Layer::create();
    addChild(uiLayer);
    uiLayer->setName("uiLayer");

    //Health
    auto healthNode = HealthNode::create();
    uiLayer->addChild(healthNode);
    healthNode->setPosition(visibleSize* Vec2 {0.04, 0.94});
    healthNode->setScale(1.5);

    //Coin Count
    auto coinCountNode = CoinCountNode::create();
    uiLayer->addChild(coinCountNode, 1, "coinCountNode");
    coinCountNode->setPosition(visibleSize * Vec2 {0.88, 0.91});
    coinCountNode->setScale(1.5);
    
    //KeyListener
    auto keyListener = EventListenerKeyboard::create();
    keyListener->onKeyPressed = [&](EventKeyboard::KeyCode keyCode, Event* event) {
        keyMap[keyCode] = true;
        if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE) {
            Director::getInstance()->end();
        }
    };
    keyListener->onKeyReleased = [&](EventKeyboard::KeyCode keyCode, Event* event) {
        keyMap[keyCode] = false;
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyListener, this);
    getChildByName<CameraHelper*>("cameraHelper")->updateX();
    return true;
}

void GameScene::update(float dt)
{
    log("\n dt %f", dt);

    hero->update(dt, keyMap);

    
    getChildByName<CameraHelper*>("cameraHelper")->updateX();
    GetComponent<CameraBehavior>(gameCamera)->updateY(dt);

}