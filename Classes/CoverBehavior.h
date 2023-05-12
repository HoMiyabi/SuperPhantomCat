#pragma once

#include "Behavior.h"

class CoverBehavior : public Behavior
{
public:
	void set(cocos2d::ValueMap data);
	void hide();
	void show();
};

inline void CoverBehavior::set(cocos2d::ValueMap data)
{
	USING_NS_CC;
	auto it = data.find("type");
	if (it != data.end()) {
		std::string type = it->second.asString();
		if (type == "hide") {
			_owner->setVisible(false);
			_owner->setOpacity(0);
		}
	}

	std::string listen = data.at("listen").asString();
	auto listener = EventListenerCustom::create(listen, [=](EventCustom* event) {
		auto type = reinterpret_cast<std::string*>(event->getUserData());
		log("covertype %s", type->c_str());
		if (*type == "hide") {
			hide();
		} else if (*type == "show") {
			show();
		}
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(listener, 1);
}
inline void CoverBehavior::hide()
{
	USING_NS_CC;
	log("hide %p", this);
	auto collider = GetComponent<Collider>(this);
	if (collider) {
		collider->setEnabled(false);
	}
	auto fadeOut = FadeOut::create(0.2);
	auto remove = CallFunc::create([this]() {
		_owner->removeFromParent();
	});
	_owner->runAction(Sequence::create(fadeOut, remove, nullptr));
}
inline void CoverBehavior::show()
{
	USING_NS_CC;
	_owner->setVisible(true);
	auto fadeIn = FadeIn::create(0.2);
	_owner->runAction(fadeIn);
	auto collider = GetComponent<Collider>(this);
	if (collider) {
		collider->setEnabled(true);
	}
}