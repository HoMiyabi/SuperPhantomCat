#include "WelcomeScene.h"
#include "GameScene.h"

USING_NS_CC;

bool WelcomeScene::init()
{
	if (!Scene::init()) {
		return false;
	}
	Director::getInstance()->getEventDispatcher()->removeAllEventListeners();

	const auto &visibleSize = Director::getInstance()->getVisibleSize();
	const auto &origin = Director::getInstance()->getVisibleOrigin();


	auto welcomeImage = Sprite::create("welcome.png");
	welcomeImage->setAnchorPoint({0, 0});
	welcomeImage->setPosition(origin);
	auto &imageSize = welcomeImage->getContentSize();
	welcomeImage->setScale(visibleSize.width / imageSize.width, visibleSize.height / imageSize.height);
	addChild(welcomeImage);
	
	auto keyListener = EventListenerKeyboard::create();
	keyListener->onKeyPressed = [](EventKeyboard::KeyCode keyCode, Event* event) {
		if (keyCode == EventKeyboard::KeyCode::KEY_J) {
			auto gameScene = GameScene::create();
			Director::getInstance()->replaceScene(gameScene);
		} else if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE) {
			Director::getInstance()->end();
		}
	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyListener, this);
	return true;
}