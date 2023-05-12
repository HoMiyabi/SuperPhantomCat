#pragma once

#include "cocos2d.h"

class CoinCountNode: public cocos2d::Node
{
private:
	int count_ = 0;
    cocos2d::Label* label_ = nullptr;
public:
    CREATE_FUNC(CoinCountNode)
    bool init() override;
    void setCount(int count);
    void addCount(int delta);
    int getCount();
};

inline bool CoinCountNode::init()
{
    USING_NS_CC;
    if (Node::init()) {
        auto coin = Sprite::create("topbar_icon_coin.png");
        addChild(coin);
        label_ = Label::createWithTTF("0", "/Font/Linotte-Regular.ttf", 36);
        addChild(label_);
        label_->setAnchorPoint({0, 0.5});
        label_->enableBold();
        label_->setPosition(40, 0);
        Director::getInstance()->getEventDispatcher()->addCustomEventListener("PICK", [this](EventCustom* event) {
            addCount(1);
        });
        return true;
    }
    return false;
}
inline void CoinCountNode::setCount(int count)
{
    count_ = count;
    label_->setString(cocos2d::StringUtils::toString(count_));
}
inline void CoinCountNode::addCount(int delta)
{
    count_ += delta;
    label_->setString(cocos2d::StringUtils::toString(count_));
}
inline int CoinCountNode::getCount()
{
    return count_;
}