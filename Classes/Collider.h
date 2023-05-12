#pragma once

#include "cocos2d.h"
#include "Definitions.h"

class Collider : public cocos2d::Component
{
public:
	enum class TYPE { NORMAL, ONE_WAY, TRIGGER };
	inline static std::string name_ = "Collider";
private:
	TYPE type_ = TYPE::NORMAL;
	std::function<void(class Collider*, class ActiveCollider*, Direction)> contactCallback_ = nullptr;
	cocos2d::Size size_;
	cocos2d::Vec2 offset_;
public:
	void setType(TYPE type);
	TYPE getType();
	void contact(class ActiveCollider* activeCollider, Direction direction);
	static Collider* create(const cocos2d::Size& size, const cocos2d::Vec2& offset = cocos2d::Vec2::ZERO);
	bool initWithSize(const cocos2d::Size& size, const cocos2d::Vec2& offset);
	void setContactCallback(std::function<void(class Collider*, class ActiveCollider*, Direction)> callback);
	void onAdd();
};

inline void Collider::setType(TYPE type) { type_ = type; }
inline Collider::TYPE Collider::getType() { return type_; }
inline void Collider::contact(class ActiveCollider* activeCollider, Direction direction)
{
	if (contactCallback_) {
		contactCallback_(this, activeCollider, direction);
	}
}
inline Collider* Collider::create(const cocos2d::Size& size, const cocos2d::Vec2& offset)
{
	Collider* pRet = new(std::nothrow) Collider(); if (pRet && pRet->initWithSize(size, offset)) {
		pRet->autorelease(); return pRet;
	} else {
		delete pRet; pRet = nullptr; return nullptr;
	}
}
inline bool Collider::initWithSize(const cocos2d::Size& size, const cocos2d::Vec2& offset)
{
	setName(name_);
	size_ = size;
	offset_ = offset;
	return true;
}
inline void Collider::setContactCallback(std::function<void(class Collider*, class ActiveCollider*, Direction)> callback)
{
	contactCallback_ = callback;
}
inline void Collider::onAdd()
{
	USING_NS_CC;
	auto body = PhysicsBody::createBox(size_, PHYSICSBODY_MATERIAL_DEFAULT, offset_);
	body->setDynamic(false);
	_owner->setPhysicsBody(body);
}