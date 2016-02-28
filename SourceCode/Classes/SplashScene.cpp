#include "SplashScene.h"
#include "GameScene.h"

USING_NS_CC;

Scene * SplashScene::createScene()
{
	auto scene = Scene::create();
	auto layer = SplashScene::create();
	scene->addChild(layer);
	return scene;
}

bool SplashScene::init()
{
	if(!Layer::init())
		return false;

	auto size = Director::getInstance()->getWinSize();

	TTFConfig config("fonts/Marker Felt.ttf", 180);

	auto GameName = Label::createWithTTF(config, "Minesweeper");
	GameName->setPosition(Point(size.width / 2, size.height * 3 / 4));
	this->addChild(GameName);

	auto GameAuthor = Label::createWithTTF(config, "GAME BY CodeLadyJJY");
	GameAuthor->setPosition(Point(size.width / 2, size.height / 2));
	GameAuthor->setScale(0.3);
	this->addChild(GameAuthor);

	//°´Å¥
	auto menuItemStartEasy = MenuItemFont::create("Easy", CC_CALLBACK_1(SplashScene::menuCallBack, this));
	menuItemStartEasy->setColor(Color3B::WHITE);
	menuItemStartEasy->setTag(1);
	auto menuItemStartMedium = MenuItemFont::create("Medium", CC_CALLBACK_1(SplashScene::menuCallBack, this));
	menuItemStartMedium->setColor(Color3B::WHITE);
	menuItemStartMedium->setTag(2);
	auto menuItemStartHard = MenuItemFont::create("Hard", CC_CALLBACK_1(SplashScene::menuCallBack, this));
	menuItemStartHard->setColor(Color3B::WHITE);
	menuItemStartHard->setTag(3);

	auto menu = Menu::create(menuItemStartEasy, menuItemStartMedium, menuItemStartHard, NULL);
	menu->setPosition(Point::ZERO);
	menuItemStartEasy->setPosition(Point(size.width / 2, size.height*0.4));
	menuItemStartMedium->setPosition(Point(size.width / 2, size.height*0.3));
	menuItemStartHard->setPosition(Point(size.width / 2, size.height*0.2));
	this->addChild(menu);

	return true;
}

void SplashScene::menuCallBack(Ref* obj)
{
	auto target = (Node*)obj;

	Scene *scene;

	switch (target->getTag())
	{
	case 1:
		scene = GameScene::createScene(1);
		break;
	case 2:
		scene = GameScene::createScene(2);
		break;
	case 3:
		scene = GameScene::createScene(3);
		break;
	default:
		break;
	}
	Director::getInstance()->replaceScene(scene);
}