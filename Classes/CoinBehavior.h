#pragma once

#include "DataBehavior.h"
#include "AudioEngine.h"

class CoinBehavior: public Behavior
{
private:
	const cocos2d::Size size_ {40.0f, 40.0f};
	cocos2d::ValueMap data_;
public:
	void onAdd();
	void show();
	void hide();
	void setData(const cocos2d::ValueMap& data);
	CREATE_FUNC(CoinBehavior)
};

inline void CoinBehavior::onAdd()
{
	USING_NS_CC;
	auto collider = Collider::create(size_);
	_owner->addComponent(collider);
	collider->setType(Collider::TYPE::TRIGGER);
	collider->setContactCallback([this](Collider* collider, ActiveCollider* activeCollider, Direction direction) {
		USING_NS_CC;
		Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("PICK");

		auto label = Label::createWithTTF("+1", FONT, 24);
		_owner->getScene()->addChild(label, 100);
		label->setTextColor(Color4B(0, 255, 0, 255));
		label->setPosition(_owner->getPosition() + _owner->getContentSize() / 2 + Vec2 {0, 10});
		label->setCameraMask(static_cast<unsigned short>(CameraFlag::USER2));

		auto fadeOut = FadeOut::create(0.3);
		auto moveUp = MoveBy::create(0.3, {0, 5});
		auto remove = CallFunc::create([label]() {
			label->removeFromParent();
		});
		label->runAction(Sequence::create(fadeOut, moveUp, remove, nullptr));

		_owner->removeFromParent();
	});

	_owner->setPosition(_owner->getPosition() + Vec2 {0, 2});
	auto move = EaseSineInOut::create(MoveBy::create(1, {0, -4}));
	_owner->runAction(RepeatForever::create(Sequence::create(move, move->reverse(), nullptr)));
}
inline void CoinBehavior::show()
{
	USING_NS_CC;
	_owner->setVisible(true);
	auto collider = GetComponent<Collider>(this);
	if (collider) {
		collider->setEnabled(true);
	}
	experimental::AudioEngine::play2d("/audio/inGame_efx_coinListGenerate.mp3");
	log("showcoin");
	auto it = data_.find("dispatch");
	if (it != data_.end()) {
		auto dispatch = it->second.asString();
		Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(dispatch);

	}
}
inline void CoinBehavior::hide()
{
	_owner->setVisible(false);
	auto collider = GetComponent<Collider>(this);
	if (collider) {
		collider->setEnabled(false);
	}
}
inline void CoinBehavior::setData(const cocos2d::ValueMap& data)
{
	USING_NS_CC;
	data_ = data;
	std::string listen = data.at("listen").asString();
	Director::getInstance()->getEventDispatcher()->addCustomEventListener(listen, [=](EventCustom* event) {
		_owner->scheduleOnce([=](float d) {
			show();
		}, 0.1, "listen");
	});
}