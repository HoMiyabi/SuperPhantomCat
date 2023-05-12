#pragma once

#include "cocos2d.h"

class GameScene : public cocos2d::Scene
{
public:
    bool init() override;
    CREATE_FUNC(GameScene)
    void update(float dt) override;
    enum class ZOrder { BACKGROUND, TILED_MAP, HERO };
    enum Type { TYPE_BRICK = 1, TYPE_QUESTION, TYPE_MUSHROOM, TYPE_HIDE };
    void onEnterTransitionDidFinish() override;
private:
    class Hero* hero = nullptr;
    cocos2d::Camera* gameCamera;
    const cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
    const cocos2d::Point origin = cocos2d::Director::getInstance()->getVisibleOrigin();

    std::map<cocos2d::EventKeyboard::KeyCode, bool> keyMap;
    std::map<uint32_t, Type> GID_TO_TYPE;
};