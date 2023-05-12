#pragma once
#include "cocos2d.h"

enum Direction {RIGHT, UP, LEFT, DOWN, RIGHT_DOWN, RIGHT_UP, UP_LEFT, UP_RIGHT, LEFT_DOWN, LEFT_UP, DOWN_LEFT, DOWN_RIGHT};

const char* const FONT = "/Font/Linotte-Regular.ttf";

template<typename T>
T* Create()
{
    T* object = new (std::nothrow) T();
    if (object && object->init()) {
        object->autorelease();
        return object;
    }
    CC_SAFE_DELETE(object);
    return nullptr;
}

template<typename T>
T* GetComponent(cocos2d::Component* component)
{
    return dynamic_cast<T*>(component->getOwner()->getComponent(T::name_));
}

template<typename T>
T* GetComponent(cocos2d::Node* node)
{
    return dynamic_cast<T*>(node->getComponent(T::name_));
}

inline cocos2d::Size operator*(const cocos2d::Size& size1, const cocos2d::Size& size2)
{
    return {size1.width * size2.width, size1.height * size2.height};
}

inline cocos2d::Vec2 operator*(const cocos2d::Vec2& vec1, const cocos2d::Vec2& vec2)
{
    return {vec1.x * vec2.x, vec1.y * vec2.y};
}

inline std::vector<cocos2d::TMXLayer*> getLayers(cocos2d::TMXTiledMap* tiledMap)
{
    USING_NS_CC;
    std::vector<cocos2d::TMXLayer*> layers;
    auto& children = tiledMap->getChildren();
    for (auto& child : children) {
        cocos2d::TMXLayer* layer = dynamic_cast<cocos2d::TMXLayer*>(child);
        if (layer) {
            layers.emplace_back(layer);
        }
    }
    return layers;
}