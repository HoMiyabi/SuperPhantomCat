#pragma once

#include "Behavior.h"
#include "cocos2d.h"
#include "CameraHelper.h"

class CameraBehavior: public Behavior
{
private:
	cocos2d::Size viewSize_;
	cocos2d::Rect rectRange_;
	cocos2d::Vec2 center_;
	float right_ = 50;
	float left_ = -50;
	float down_ = -50;
	float up_ = 50;
	enum class StateX { LEFT, RIGHT, TO_LEFT, TO_RIGHT };
	StateX stateX_ = StateX::RIGHT;
	cocos2d::Node* target_;
	enum ActionTag { X };
	CameraHelper* helper;
public:
	static CameraBehavior* create(const cocos2d::Size viewSize, const cocos2d::Rect& rectRange, cocos2d::Node* target);
	bool initWithSize(const cocos2d::Size viewSize, const cocos2d::Rect& rectRange, cocos2d::Node* target);
	void onAdd() override;
	void toLeft();
	void toRight();
	void updateY(float dt);
	void setCameraCenter(cocos2d::Vec2 pos);
	float getCameraCenterX();
	float getCameraCenterY();
};

inline CameraBehavior* CameraBehavior::create(const cocos2d::Size viewSize, const cocos2d::Rect& rectRange, cocos2d::Node* target)
{
	CameraBehavior* pRet = new(std::nothrow) CameraBehavior(); if (pRet && pRet->initWithSize(viewSize, rectRange, target)) {
		pRet->autorelease(); return pRet;
	} else {
		delete pRet; pRet = nullptr; return nullptr;
	}
}
inline bool CameraBehavior::initWithSize(const cocos2d::Size viewSize, const cocos2d::Rect& rectRange, cocos2d::Node* target)
{
	Behavior::init();
	viewSize_ = viewSize;
	rectRange_ = rectRange;
	target_ = target;
	return true;
}
inline void CameraBehavior::onAdd()
{
	USING_NS_CC;
	helper = dynamic_cast<CameraHelper*>(_owner->getParent());
	setCameraCenter({right_, helper->getTargetY() + up_});
	Director::getInstance()->getEventDispatcher()->addCustomEventListener("CAMERA_TO_LEFT", [this](EventCustom* e) {
		toLeft();
	});
	Director::getInstance()->getEventDispatcher()->addCustomEventListener("CAMERA_TO_RIGHT", [this](EventCustom* e) {
		toRight();
	});
}

inline void CameraBehavior::toLeft()
{
	USING_NS_CC;
	if (stateX_ == StateX::RIGHT || stateX_ == StateX::TO_RIGHT) {
		if (stateX_ == StateX::TO_RIGHT) {
			_owner->stopActionByTag(ActionTag::X);
		}
		stateX_ = StateX::TO_LEFT;
		auto move = EaseCubicActionOut::create(MoveBy::create(2, {left_ - getCameraCenterX(), 0}));
		auto f = CallFunc::create([this]() {
			stateX_ = StateX::LEFT;
		});
		auto seq = Sequence::createWithTwoActions(move, f);
		_owner->runAction(seq);
		seq->setTag(ActionTag::X);
	}
}

inline void CameraBehavior::toRight()
{
	USING_NS_CC;
	if (stateX_ == StateX::LEFT || stateX_ == StateX::TO_LEFT) {
		if (stateX_ == StateX::TO_LEFT) {
			_owner->stopActionByTag(ActionTag::X);
		}
		stateX_ = StateX::TO_RIGHT;
		auto move = EaseCubicActionOut::create(MoveBy::create(2, {right_ - getCameraCenterX(), 0}));
		auto f = CallFunc::create([this]() {
			stateX_ = StateX::RIGHT;
		});
		auto seq = Sequence::createWithTwoActions(move, f);
		_owner->runAction(seq);
		seq->setTag(ActionTag::X);
	}
}

inline void CameraBehavior::updateY(float dt)
{
	USING_NS_CC;
	float targetY = std::min(GetComponent<ActiveCollider>(target_)->lastContactDownPos_.y + helper->halfHeight_, helper->getTargetY());
	float gap = targetY + up_ - getCameraCenterY();
	if (gap > 600) {
		gap = 600;
	} else if (gap < -600) {
		gap = -600;
	}
	
	if (targetY > 1318) {
		log("qiguai");
	}
	float speedY = 5 * (gap - 75.0 * sin(0.01 * gap));
	float offsetY = speedY * dt;
	cocos2d::log("targetY %f gap %f speed %f dt %f offset %f", targetY, gap, speedY, dt, offsetY);
	float pos = std::max(_owner->getPositionY() + offsetY, 0.0f);
	_owner->setPositionY(pos);
}

inline float CameraBehavior::getCameraCenterX()
{
	return _owner->getPositionX() + viewSize_.width / 2;
}

inline float CameraBehavior::getCameraCenterY()
{
	return _owner->getPositionY() + viewSize_.height / 2;
}

inline void CameraBehavior::setCameraCenter(cocos2d::Vec2 pos)
{
	_owner->setPosition(pos - viewSize_ / 2);
}