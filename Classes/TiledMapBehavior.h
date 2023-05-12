#pragma once

#include "Behavior.h"

class TiledMapBehavior: public Behavior
{
public:
	cocos2d::Vec2 getPositionInTile(cocos2d::Vec2 positionInPoints);
};

inline cocos2d::Vec2 TiledMapBehavior::getPositionInTile(cocos2d::Vec2 positionInPoints)
{
	USING_NS_CC;
	auto tiledMap = dynamic_cast<cocos2d::TMXTiledMap*>(getOwner());
	Size tileSize(64.0f, 64.0f);
	float x = positionInPoints.x / tileSize.width;
	float y = (tiledMap->getContentSize().height - positionInPoints.y - tileSize.height) / tileSize.height;
	return {x, y};
}