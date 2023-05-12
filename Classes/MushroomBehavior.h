#pragma once

#include "Behavior.h"
#include "Collider.h"
#include "ActiveCollider.h"

class MushroomBehaivor: public Behavior
{
private:
	cocos2d::Node* lightMushroom_ = nullptr;
	cocos2d::Node* leftMushroom_ = nullptr;
	cocos2d::Node* rightMushroom_ = nullptr;
	bool earlyJump_ = false;
	const float JUMP_SPEED_HIGHER_ = 1000;
	const float JUMP_SPEED_LOWER_ = 650;
	const char* const JUMP_EFFECT_MR_ = "/audio/inGame_efx_spring.mp3";
	cocos2d::EventListenerCustom* earlyJumpListener_;
public:
	void set(cocos2d::TMXLayer* layer, float x, float y, cocos2d::TMXLayer* lightLayer);
	void contactDown(class Collider* collider, class ActiveCollider* activeCollider);
};

inline void MushroomBehaivor::set(cocos2d::TMXLayer* layer, float x, float y, cocos2d::TMXLayer* lightLayer)
{
	USING_NS_CC;

	if (layer->getTileGIDAt({x - 1, y})) {
		leftMushroom_ = layer->getTileAt({x - 1, y});
	}
	if (layer->getTileGIDAt({x + 1, y})) {
		rightMushroom_ = layer->getTileAt({x + 1, y});
	}
	lightMushroom_ = lightLayer->getTileAt({x, y});

	earlyJumpListener_ = EventListenerCustom::create("JUMP", [=](EventCustom* event) {
		Hero* hero = static_cast<Hero*>(event->getUserData());
		//Stop Spring
		earlyJump_ = true;

		//Recover Hero
		hero->setControlAble(LEFT, true);
		hero->setControlAble(RIGHT, true);
		hero->setJumpSpeed(hero->JUMP_SPEED_DEFAULT_);
		hero->setJumpEffect(hero->JUMP_EFFECT_DEFAULT_);

		//Pause Listener
		earlyJumpListener_->setEnabled(false);
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(earlyJumpListener_, 1);
	//Pause Listener
	earlyJumpListener_->setEnabled(false);

	auto collider = Collider::create({_owner->getContentSize().width, 1}, {0, _owner->getContentSize().height / 2});
	_owner->addComponent(collider);
	collider->setType(Collider::TYPE::ONE_WAY);
	collider->setContactCallback([this](class Collider* collider, class ActiveCollider* activeCollider, Direction direction) {
		if (direction == DOWN) {
			contactDown(collider, activeCollider);
		}
	});
}
inline void MushroomBehaivor::contactDown(class Collider* collider, class ActiveCollider* activeCollider)
{
	USING_NS_CC;
	if (_owner->getNumberOfRunningActions() == 0) {
		auto hero = dynamic_cast<Hero*>(activeCollider->getOwner());
		//Set Hero
		hero->setControlAble(LEFT, false);
		hero->setControlAble(RIGHT, false);
		hero->setJumpSpeed(JUMP_SPEED_HIGHER_);
		hero->setJumpEffect(JUMP_EFFECT_MR_);
		activeCollider->setVelocityX(0);

		//Close Left Right Mushroom Collider
		if (leftMushroom_) {
			GetComponent<Collider>(leftMushroom_)->setEnabled(false);
		}
		if (rightMushroom_) {
			GetComponent<Collider>(rightMushroom_)->setEnabled(false);
		}

		//Resume Listener
		earlyJump_ = false;
		earlyJumpListener_->setEnabled(true);

		//Action
		auto scaleDown = EaseSineOut::create(ScaleTo::create(0.3f, 1.0f, 0.75f));
		auto fadeOut = EaseSineOut::create(FadeOut::create(0.3f));
		auto down = Spawn::createWithTwoActions(scaleDown, fadeOut);

		auto spring = CallFunc::create([=]() {
			if (!earlyJump_) {
				hero->setJumpSpeed(JUMP_SPEED_LOWER_);
				hero->setState(Hero::State::JUMP);

				//Recover Hero
				hero->setControlAble(LEFT, true);
				hero->setControlAble(RIGHT, true);
				hero->setJumpSpeed(hero->JUMP_SPEED_DEFAULT_);
				hero->setJumpEffect(hero->JUMP_EFFECT_DEFAULT_);
			}
		});

		auto scaleUp = EaseSineIn::create(ScaleTo::create(0.2f, 1.0f, 1.0f));
		auto fadeIn = EaseSineIn::create(FadeIn::create(0.2f));
		auto up = Spawn::createWithTwoActions(scaleUp, fadeIn);

		auto recoverLeftRight = CallFunc::create([=]() {
			if (leftMushroom_) {
				GetComponent<Collider>(leftMushroom_)->setEnabled(true);
			}
			if (rightMushroom_) {
				GetComponent<Collider>(rightMushroom_)->setEnabled(true);
			}
		});

		_owner->runAction(Sequence::create(down, spring, up, recoverLeftRight, nullptr));

		auto show = CallFunc::create([=]() {
			lightMushroom_->setVisible(true);
		});
		auto hide = CallFunc::create([=]() {
			lightMushroom_->setVisible(false);
		});
		lightMushroom_->runAction(Sequence::create(show, scaleDown->clone(), scaleUp->clone(), hide, nullptr));
	}
}