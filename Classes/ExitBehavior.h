#pragma once

#include "Behavior.h"
#include "Definitions.h"
#include "EndScene.h"
#include "cocos2d.h"
#include "CoinCountNode.h"

class ExitBehavior : public Behavior
{
public:
	void set(cocos2d::ValueMap data);
};

inline void ExitBehavior::set(cocos2d::ValueMap data)
{
	float width = data.at("width").asFloat();
	float height = data.at("height").asFloat();
	float x = data.at("x").asFloat();
	float y = data.at("y").asFloat();
	_owner->setPosition(x, y);
	auto collider = Collider::create({width, height});
	_owner->addComponent(collider);
	collider->setContactCallback([this](Collider* collider, ActiveCollider* activeCollider, Direction direction) {
		USING_NS_CC;
		float totalCount = UserDefault::getInstance()->getIntegerForKey("coin", 0);
		int count = dynamic_cast<CoinCountNode*>(_owner->getScene()->getChildByName("uiLayer")->getChildByName("coinCountNode"))->getCount();
		auto endScene = EndScene::create(count);
		_owner->setVisible(false);
		collider->setEnabled(false);
		cocos2d::Director::getInstance()->replaceScene(endScene);
	});
}