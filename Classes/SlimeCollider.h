#pragma once

#include "Collider.h"
#include "Definitions.h"
#include "SimpleAudioEngine.h"

class SlimeCollider : public Collider
{
public:
	void contactDown(class Hero* hero) override
	{
		hero->setVelocityY(1000);
		hero->setState(Hero::State::JUMP);
		getOwner()->removeFromParent();
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("/AudioClip/inGame_monster_timiddied.wav");
	}
	void contactLeft(class Hero* hero) override
	{
		USING_NS_CC;
		log("hurt");
		hero->setVelocity({700, 1000});
		hero->setState(Hero::State::HURT);
		hero->setControlAble(LEFT, false);
		hero->setControlAble(RIGHT, false);
		hero->setControlAble(UP, false);
		Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("HURT");
	}
	void contactRight(class Hero* hero) override
	{
		USING_NS_CC;
		hero->setVelocity({-700, 1000});
		hero->setState(Hero::State::HURT);
		hero->setControlAble(LEFT, false);
		hero->setControlAble(RIGHT, false);
		hero->setControlAble(UP, false);
		Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("HURT");
	}
};