#pragma once

#include "Collider.h"

class OneWayCollider : public Collider
{
public:
	bool init() override
	{
		Collider::init();
		isOneWay_ = true;
		return true;
	}
	void onAdd() override
	{
		USING_NS_CC;
		auto body = PhysicsBody::createBox(Size {getOwner()->getContentSize().width, 32}, PHYSICSBODY_MATERIAL_DEFAULT, Vec2(0, 16));
		body->setDynamic(false);
		getOwner()->setPhysicsBody(body);
	}
};