#pragma once

#include "Behavior.h"
#include "Collider.h"
#include "AudioEngine.h"

class BrickBehavior: public Behavior
{
private:
	cocos2d::ValueMap data_;
public:
	CREATE_FUNC(BrickBehavior)
	void set(cocos2d::ValueMap data);
};

inline void BrickBehavior::set(cocos2d::ValueMap data)
{
	data_ = data;
	auto collider = Collider::create(_owner->getContentSize());
	_owner->addComponent(collider);
	collider->setContactCallback([this](Collider* collider, ActiveCollider* activeCollider, Direction direction) {
		switch (direction) {
			case UP: {
				if (activeCollider->getVelocityY() > 0) {
					activeCollider->setVelocityY(0);
				}
				cocos2d::experimental::AudioEngine::play2d("/audio/inGame_efx_blockBreak.mp3");

				std::string dispatch = data_.at("dispatch").asString();
				cocos2d::Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(dispatch);

				_owner->removeFromParent();
				break;
			}
		}
	});
}