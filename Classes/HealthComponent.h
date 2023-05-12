#pragma once

#include "cocos2d.h"
#include "GameScene.h"

class HealthComponent : public cocos2d::Component
{
private:
	int health_ = 1;
	const int offset_ = 100;
public:
	inline static std::string name_ = "HealthComponent";
	bool init() override
	{
		setName(name_);
		return true;
	}
	void set(int health)
	{
		health_ = health;
	}
	void onAdd() override
	{
		USING_NS_CC;
		for (int i = 0; i < health_; i++) {
			auto heart = Sprite::create("ui_icon_heart.png");
			heart->setPosition(i * offset_, 0);
			heart->setCameraMask(getOwner()->getCameraMask());
			getOwner()->addChild(heart, 1, i);
		}

		//Hurt Reduce One
		auto listener = EventListenerCustom::create("HURT", [&](EventCustom* event) {
			if (health_ > 0) {
				health_--;
				auto heart = getOwner()->getChildByTag(health_);
				heart->removeFromParent();
			}
			if (health_ == 0) {
				auto game = Create<GameScene>();
				Director::getInstance()->replaceScene(game);
			}
			});
		Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, getOwner());
		Director::getInstance()->getEventDispatcher()->addCustomEventListener("PICK_HEART", [this](EventCustom* event) {

			plusOne();
		});
	}
	void plusOne()
	{
		USING_NS_CC;
		log("heart");
		auto heart = Sprite::create("ui_icon_heart.png");
		heart->setPosition(health_ * offset_, 0);
		heart->setCameraMask(getOwner()->getCameraMask());
		getOwner()->addChild(heart, 1, health_);
		health_++;
	}
};