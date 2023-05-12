#pragma once

#include "Collider.h"
#include "TriggerBehavior.h"

class TriggerCollider : public Collider
{
	void event(class Hero* hero, Direction direction) override
	{
		auto behavior = GetComponent<TriggerBehavior>(this);

	}
};