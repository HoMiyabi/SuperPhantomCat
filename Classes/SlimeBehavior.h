#pragma once

#include "Behavior.h"
#include "ActiveCollider.h"
#include "AudioEngine.h"

class SlimeBehaivor: public Behavior
{
private:
	float speed_ = 50;
public:
	void set(const cocos2d::Vec2& dest);
};

inline void SlimeBehaivor::set(const cocos2d::Vec2& dest)
{
	USING_NS_CC;
	const auto delta = dest - _owner->getPosition();
	const float duration = delta.getLength() / speed_;
	auto moveBy = MoveBy::create(duration, delta);
	auto flipX = FlipX::create(true);
	Animation* animation = Animation::create();
	for (int i = 0; i <= 12; i++) {
		auto name = std::format("/slime/mons_slime_run_{}.png", i);
		animation->addSpriteFrameWithFile(name);
	}
	animation->setDelayPerUnit(0.1f);
	auto animate = Animate::create(animation);
	auto cycle = Sequence::create(moveBy, flipX, moveBy->reverse(), flipX->reverse(), nullptr);
	_owner->runAction(RepeatForever::create(cycle));
	_owner->runAction(RepeatForever::create(animate));

	auto collider = Collider::create(_owner->getContentSize());
	_owner->addComponent(collider);

	collider->setContactCallback([this](Collider* collider, ActiveCollider* activeCollider, Direction direction) {
		switch (direction) {
			case DOWN: {
				activeCollider->setVelocityY(1000);
				auto hero = dynamic_cast<Hero*>(activeCollider->getOwner());
				hero->setState(Hero::State::JUMP);
				_owner->removeFromParent();
				experimental::AudioEngine::play2d("/audio/inGame_monster_timiddied.mp3");
				break;
			}
			case RIGHT: {
				USING_NS_CC;
				auto hero = dynamic_cast<Hero*>(activeCollider->getOwner());
				activeCollider->setVelocity({-700, 1000});
				hero->setState(Hero::State::HURT);
				hero->setControlAble(LEFT, false);
				hero->setControlAble(RIGHT, false);
				hero->setControlAble(UP, false);
				Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("HURT");
				break;
			}
			case LEFT: {
				USING_NS_CC;
				auto hero = dynamic_cast<Hero*>(activeCollider->getOwner());
				activeCollider->setVelocity({700, 1000});
				hero->setState(Hero::State::HURT);
				hero->setControlAble(LEFT, false);
				hero->setControlAble(RIGHT, false);
				hero->setControlAble(UP, false);
				Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("HURT");
				break;
			}
		}
	});
}