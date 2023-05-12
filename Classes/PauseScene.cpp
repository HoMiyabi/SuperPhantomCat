#include "PauseScene.h"
#include "Definitions.h"

USING_NS_CC;

bool PauseScene::init()
{
	if (!Scene::init()) {
		return false;
	}

	Director::getInstance()->getEventDispatcher()->removeAllEventListeners();

	auto layer = LayerColor::create();
	addChild(layer);

	auto title = Label::create("PAUSE", FONT, 36);
	layer->addChild(title);

	auto keyListener = EventListenerKeyboard::create();
	keyListener->onKeyPressed = [](EventKeyboard::KeyCode key, Event* e) {
		if (key == EventKeyboard::KeyCode::KEY_ESCAPE) {
			Director::getInstance()->popScene();
		}
	};

	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyListener, this);

	return true;
}
