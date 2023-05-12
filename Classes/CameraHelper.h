#pragma once

#include "cocos2d.h"

class CameraHelper: public cocos2d::Node
{
public:
	cocos2d::Vec2 targetWorldPos_;
	float halfWidth_;
	float halfHeight_;
	cocos2d::Node* target_;
	static CameraHelper* create(cocos2d::Node* target);
	void updateX();
	float getTargetY();
	void initSelf(cocos2d::Node* target);
};

inline void CameraHelper::updateX()
{
	setPositionX(target_->getPositionX() + halfWidth_);
}

inline float CameraHelper::getTargetY()
{
	return target_->getPositionY() + halfHeight_;
}

inline void CameraHelper::initSelf(cocos2d::Node* target)
{
	target_ = target;
	halfWidth_ = target->getContentSize().width / 2;
	halfHeight_ = target->getContentSize().height / 2;
}

inline CameraHelper* CameraHelper::create(cocos2d::Node* target)
{
	CameraHelper* pRet = new(std::nothrow) CameraHelper(); if (pRet && pRet->init()) {
		pRet->autorelease(); pRet->initSelf(target); return pRet;
	} else {
		delete pRet; pRet = nullptr; return nullptr;
	}
}