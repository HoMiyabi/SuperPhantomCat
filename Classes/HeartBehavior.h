#pragma once

#include "Behavior.h"
#include "Collider.h"

class HeartBehavior: public Behavior
{
private:
	const cocos2d::Size size_ {40.0f, 40.0f};
public:
	void onAdd() override;
};

inline void HeartBehavior::onAdd()
{
	auto collider = Collider::create(size_);
	_owner->addComponent(collider);
	collider->setType(Collider::TYPE::TRIGGER);
	collider->setContactCallback([this](Collider* collider, class ActiveCollider* activeCollider, Direction direction) {
		USING_NS_CC;
		Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("PICK_HEART");
		_owner->removeFromParent();
	});
}