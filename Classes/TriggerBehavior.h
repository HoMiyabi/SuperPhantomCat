#pragma once

#include "Behavior.h"

class TriggerBehavior: public Behavior
{
private:
	cocos2d::ValueMap data_;
public:
	void set(const cocos2d::ValueMap& data);
};

inline void TriggerBehavior::set(const cocos2d::ValueMap& data)
{
	data_ = data;
	float width = data.at("width").asFloat();
	float height = data.at("height").asFloat();
	float x = data.at("x").asFloat();
	float y = data.at("y").asFloat();
	_owner->setPosition(x + width / 2.0f, y + height / 2.0f);
	auto collider = Collider::create({width, height});
	_owner->addComponent(collider);
	collider->setContactCallback([this](class Collider* collider, class ActiveCollider* activeCollider, Direction direction) {
		_owner->setVisible(false);
		collider->setEnabled(false);
		std::string dispatch = data_.at("dispatch").asString();
		std::string type = data_.at("type").asString();
		cocos2d::Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(dispatch, &type);
	});
}