#pragma once

#include "Collider.h"
#include "Behavior.h"

class ExitCollider : public Collider
{
	void event(class Hero* hero, Direction direction) override
	{
		auto behavior = GetComponent<Behavior>(this);
		behavior->event();
	}
};