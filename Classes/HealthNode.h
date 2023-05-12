#pragma once

#include "cocos2d.h"
#include "GameScene.h"

class HealthNode: public cocos2d::Node
{
private:
    int count_ = 4;
    const int offset_ = 60;
public:
    CREATE_FUNC(HealthNode)
    bool init() override;
    void addOne();
    void reduceOne();
};

inline bool HealthNode::init()
{
    USING_NS_CC;
    if (Node::init()) {
        for (int i = 0; i < count_; i++) {
            auto heart = Sprite::create("ui_icon_heart.png");
            heart->setPosition(i * offset_, 0);
            addChild(heart, 1, i);
        }
        Director::getInstance()->getEventDispatcher()->addCustomEventListener("PICK_HEART", [this](EventCustom* event) {
            addOne();
        });
        Director::getInstance()->getEventDispatcher()->addCustomEventListener("HURT", [this](EventCustom* event) {
            reduceOne();
        });
        return true;
    }
    return false;
}
inline void HealthNode::addOne()
{
    USING_NS_CC;
    auto heart = Sprite::create("ui_icon_heart.png");
    heart->setPosition(count_ * offset_, 0);
    addChild(heart, 1, count_);
    count_++;
}
inline void HealthNode::reduceOne()
{
    count_--;
    auto heart = getChildByTag(count_);
    heart->removeFromParent();
    if (count_ == 0) {
        auto gameScene = GameScene::create();
        cocos2d::Director::getInstance()->replaceScene(gameScene);
    }
}