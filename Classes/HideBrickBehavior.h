#pragma once

#include "Behavior.h"
#include "Collider.h"
#include "AudioEngine.h"

class HideBrickBehavior: public Behavior
{
public:
	CREATE_FUNC(HideBrickBehavior)
	void onAdd() override;
};

inline void HideBrickBehavior::onAdd()
{
	_owner->setVisible(false);
	auto& size = _owner->getContentSize();
	auto collider = Collider::create({size.width - 15, size.height});
	_owner->addComponent(collider);
	collider->setContactCallback([this](class Collider* collider, class ActiveCollider* activeCollider, Direction direction) {
		if (direction == Direction::UP) {
			_owner->setVisible(true);
			cocos2d::experimental::AudioEngine::play2d("/audio/bump_head.mp3");
		}
	});
}