#include "GameScene.h"
#include "SplashScene.h"

int g_gameType;					//游戏难度类型，1 Easy，2 Medium， 3 Hard

Scene * GameScene::createScene(int type)
{
	g_gameType = type;

	auto scene = Scene::create();
	auto layer = GameScene::create();
	scene->addChild(layer);
	return scene;
}

bool GameScene::init()
{
	if(!Layer::init())
		return false;

	size = Director::getInstance()->getWinSize();

	//初始化
	minesNums = MINES_NUMS[g_gameType - 1];
	remainderTime = TIMES[g_gameType - 1];
	isClicked = false;

	for (int i = 0; i <= GRID_WIDTH * g_gameType + 1; ++i)
		for (int j = 0; j <= GRID_HEIGHT * g_gameType + 1; ++j)
		{
			map[i][j].realType = GRID_REAL_TYPE::NUM;
			map[i][j].faceType = GRID_FACE_TYPE::NOTHING;
			map[i][j].number = 0;
		}

	for (int i = 1; i <= GRID_WIDTH * g_gameType; ++i)
		for (int j = 1; j <= GRID_HEIGHT * g_gameType; ++j)
		{
			auto sp = Sprite::create("MINESWEEPER_X.png");
			sp->setAnchorPoint(Vec2(1, 1));
			sp->setScale(1.0 / g_gameType);
			sp->setPosition(Vec2(GRID_SIZE * i / g_gameType, GRID_SIZE * j / g_gameType));
			addChild(sp);
			map[i][j].sp = sp;
		}

	setMines();
	setNumber();

	//按钮
	auto menuItemMainMenu = MenuItemFont::create("Main Menu", CC_CALLBACK_1(GameScene::menuCallBack, this));
	menuItemMainMenu->setColor(Color3B::WHITE);
	menuItemMainMenu->setAnchorPoint(Vec2(1, 0.5));

	auto menu = Menu::create(menuItemMainMenu,  NULL);
	menu->setPosition(Vec2::ZERO);
	menuItemMainMenu->setPosition(Vec2(size.width, size.height * 0.6));
	addChild(menu);

	//显示地雷数
	auto sp1 = Sprite::create("MINESWEEPER_M.png");
	sp1->setAnchorPoint(Vec2(1, 0.5));
	sp1->setScale(0.5);
	sp1->setPosition(Vec2(size.width, size.height * 0.4));
	addChild(sp1);

	auto sp2 = Sprite::create("MINESWEEPER_tray.png");
	sp2->setAnchorPoint(Vec2(1, 0.5));
	sp2->setScale(0.5);
	sp2->setPosition(Vec2(size.width - sp1->getBoundingBox().size.width, size.height * 0.4));
	addChild(sp2);

	minesNumsLabel = Label::createWithSystemFont(StringUtils::format("%d", minesNums), "Arial", 33);
	minesNumsLabel->setColor(Color3B::BLACK);
	minesNumsLabel->setPosition(Vec2(size.width - sp1->getBoundingBox().size.width - sp2->getBoundingBox().size.width / 2, size.height * 0.4));
	addChild(minesNumsLabel);

	//显示时间
	sp1 = Sprite::create("MINESWEEPER_C.png");
	sp1->setAnchorPoint(Vec2(1, 0.5));
	sp1->setScale(0.5);
	sp1->setPosition(Vec2(size.width, size.height * 0.2));
	addChild(sp1);

	sp2 = Sprite::create("MINESWEEPER_tray.png");
	sp2->setAnchorPoint(Vec2(1, 0.5));
	sp2->setScale(0.5);
	sp2->setPosition(Vec2(size.width - sp1->getBoundingBox().size.width, size.height * 0.2));
	addChild(sp2);

	timeLabel = Label::createWithSystemFont(StringUtils::format("%d", remainderTime), "Arial", 33);
	timeLabel->setColor(Color3B::BLACK);
	timeLabel->setPosition(Vec2(size.width - sp1->getBoundingBox().size.width - sp2->getBoundingBox().size.width / 2, size.height * 0.2));
	addChild(timeLabel);

	//触摸监听事件
	listener = EventListenerMouse::create();

	listener->onMouseDown = CC_CALLBACK_1(GameScene::onMouseDown, this);

	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	//游戏开始
	startTime = clock();
	schedule(schedule_selector(GameScene::updateTime), 1.0f);

	return true;
}

//更新时间
void GameScene::updateTime(float t)
{
	remainderTime--;
	timeLabel->setString(StringUtils::format("%d", remainderTime));
	if (remainderTime == 0)
	{
		gameOver("Time Out");
	}
}

void GameScene::onMouseDown(Event *event)
{
	EventMouse * e = (EventMouse*)event;

	//超出网格范围
	if (e->getCursorX() > GRID_SIZE * GRID_WIDTH)
		return;

	m_x = e->getCursorX() * g_gameType / GRID_SIZE + 1;
	m_y = e->getCursorY() * g_gameType / GRID_SIZE + 1;

	if (isClicked)
	{
		isClicked = false;
		doubleClicked(m_x, m_y);
	}
	else
	{
		isClicked = true;
		m_type = e->getMouseButton();
		scheduleOnce(schedule_selector(GameScene::singleClicked), 0.25f);
	}
}

void GameScene::singleClicked(float t)
{
	if (isClicked)
		isClicked = false;

	if (map[m_x][m_y].faceType != GRID_FACE_TYPE::NUM)
	{
		//左键
		if (m_type == 0)
		{
			//点击到地雷
			if (map[m_x][m_y].realType == GRID_REAL_TYPE::MINE)
			{
				gameOver("Bomb!");

				//显示‘X’
				auto sp = Sprite::create("MINESWEEPER_W.png");
				sp->setAnchorPoint(Vec2(1, 1));
				sp->setScale(1.0 / g_gameType);
				sp->setPosition(Vec2(GRID_SIZE * m_x / g_gameType, GRID_SIZE * m_y / g_gameType));
				addChild(sp, 2);
			}
			//点击到数字
			else if (map[m_x][m_y].realType == GRID_REAL_TYPE::NUM && map[m_x][m_y].number != 0)
			{
				showNumer(m_x, m_y, map[m_x][m_y].number);
			}
			//点击到空白
			else
			{
				showBlank(m_x, m_y);
			}
		}
		//右键
		else if (m_type == 1)
		{
			if (map[m_x][m_y].faceType == GRID_FACE_TYPE::NOTHING)
			{
				map[m_x][m_y].faceType = GRID_FACE_TYPE::FLAG;
				showFlag(m_x, m_y);

				minesNums--;
				minesNumsLabel->setString(StringUtils::format("%d", minesNums));
			}
			else if(map[m_x][m_y].faceType == GRID_FACE_TYPE::FLAG)
			{
				map[m_x][m_y].faceType = GRID_FACE_TYPE::NOTHING;
				map[m_x][m_y].sp->removeFromParent();

				minesNums++;
				minesNumsLabel->setString(StringUtils::format("%d", minesNums));
			}
		}

		if (minesNums == 0 && isWin())
			gameWin();
	}
}

void GameScene::doubleClicked(int x, int y)
{
	if (map[x][y].faceType == GRID_FACE_TYPE::NOTHING)
	{
		m_type = 0;
		scheduleOnce(schedule_selector(GameScene::singleClicked), 0);
		return;
	}
	else if (map[x][y].faceType == GRID_FACE_TYPE::FLAG)
		return;

	if (map[x][y].number != 0 && calcFlagNumber(x, y) == map[x][y].number)
	{
		showAround(x, y);
	}

	if (minesNums == 0 && isWin())
		gameWin();
}

//放置地雷
void GameScene::setMines()
{
	//放置地雷
	srand((unsigned)time(NULL));
	int x, y;

	for (int i = 0; i != minesNums;)
	{
		x = rand() % (GRID_WIDTH * g_gameType) + 1;
		y = rand() % (GRID_WIDTH * g_gameType) + 1;

		if (map[x][y].realType != GRID_REAL_TYPE::MINE)
		{
			map[x][y].realType = GRID_REAL_TYPE::MINE;
			i++;
			minesPosition.push_back(MyPoint{x, y});
		}
	}
}

//计算数字
void GameScene::setNumber()
{
	int number = 0;

	for (int i = 1; i <= GRID_WIDTH * g_gameType; ++i)
		for (int j = 1; j <= GRID_HEIGHT * g_gameType; ++j)
		{
			number = 0;

			if (map[i][j].realType != GRID_REAL_TYPE::MINE)
			{
				for (int m = -1; m <= 1; ++m)
					for (int n = -1; n <= 1; ++n)
						if (!(m == 0 && n == 0) && map[i + m][j + n].realType == GRID_REAL_TYPE::MINE)
							number++;

				map[i][j].number = number;
			}
		}
}

//显示数字
void GameScene::showNumer(int x, int y, int n)
{
	map[x][y].faceType = GRID_FACE_TYPE::NUM;

	auto sp = Sprite::create(StringUtils::format("MINESWEEPER_%d.png", n));
	sp->setAnchorPoint(Vec2(1, 1));
	sp->setScale(1.0 / g_gameType);
	sp->setPosition(Vec2(GRID_SIZE * x / g_gameType, GRID_SIZE * y / g_gameType));
	addChild(sp);
	map[x][y].sp = sp;
}

//显示空白网格
void GameScene::showBlank(int x, int y)
{
	if (x < 1 || x > GRID_WIDTH * g_gameType || y < 1 || y > GRID_HEIGHT * g_gameType)
		return;

	if (map[x][y].realType == GRID_REAL_TYPE::MINE)
		return;

	if (map[x][y].faceType == GRID_FACE_TYPE::FLAG)
		return;

	if (map[x][y].faceType == GRID_FACE_TYPE::NUM)
		return;

	showNumer(x, y, map[x][y].number);

	if (map[x][y].number != 0)
		return;

	for (int m = -1; m <= 1; ++m)
		for (int n = -1; n <= 1; ++n)
			if (!(m == 0 && n == 0))
				showBlank(x + m, y + n);
}

//显示旗帜
void GameScene::showFlag(int x, int y)
{
	auto sp = Sprite::create("MINESWEEPER_F.png");
	sp->setAnchorPoint(Vec2(1, 1));
	sp->setScale(1.0 / g_gameType);
	sp->setPosition(Vec2(GRID_SIZE * x / g_gameType, GRID_SIZE * y / g_gameType));
	addChild(sp);
	map[x][y].sp = sp;
}

//双击显示周围数字
void GameScene::showAround(int x, int y)
{
	for (int i = -1; i <= 1; ++i)
		for (int j = -1; j <= 1; ++j)
		{
			if (!(i == 0 && j == 0) && (x + i) >= 1 && (x + i) <= GRID_WIDTH * g_gameType && (y + j) >= 1 && (y + j) <= GRID_HEIGHT * g_gameType)
				copewithAround(x + i, y + j);
		}

	if (wrongFlag.size() != 0)
	{
		gameOver("Bomb!");
		wrongFlag.clear();
	}
}

void GameScene::copewithAround(int x, int y)
{
	if (map[x][y].faceType == GRID_FACE_TYPE::NOTHING && map[x][y].realType == GRID_REAL_TYPE::NUM)
	{
		if (map[x][y].number != 0)
			showNumer(x, y, map[x][y].number);
		else
			showBlank(x, y);
	}
	else if (map[x][y].faceType == GRID_FACE_TYPE::FLAG && map[x][y].realType != GRID_REAL_TYPE::MINE)
		wrongFlag.push_back(MyPoint{ x, y });
}

//计算周围旗帜数量
int GameScene::calcFlagNumber(int x, int y)
{
	int number = 0;

	for (int m = -1; m <= 1; ++m)
		for (int n = -1; n <= 1; ++n)
			if (!(m == 0 && n == 0) && map[x + m][y + n].faceType == GRID_FACE_TYPE::FLAG)
				number++;

	return number;
}

//判读是否胜利
bool GameScene::isWin()
{
	for (int i = 1; i <= GRID_WIDTH * g_gameType; ++i)
		for (int j = 1; j <= GRID_HEIGHT * g_gameType; ++j)
		{
			if (map[i][j].faceType == GRID_FACE_TYPE::NOTHING)
				return false;
			else if (map[i][j].faceType == GRID_FACE_TYPE::FLAG && map[i][j].realType != GRID_REAL_TYPE::MINE)
				return false;
		}

	return true;
}

void GameScene::gameOver(string str)
{
	_eventDispatcher->removeEventListener(listener);
	unschedule(schedule_selector(GameScene::updateTime));

	//显示所有地雷
	for (auto p : minesPosition)
	{
		auto sp = Sprite::create("MINESWEEPER_M.png");
		sp->setAnchorPoint(Vec2(1, 1));
		sp->setScale(1.0 / g_gameType);
		sp->setPosition(Vec2(GRID_SIZE * p.x / g_gameType, GRID_SIZE * p.y / g_gameType));
		addChild(sp);
		map[p.x][p.y].sp = sp;
	}

	//显示所有错误的旗帜
	for (auto p : wrongFlag)
	{
		auto sp = Sprite::create("MINESWEEPER_W.png");
		sp->setAnchorPoint(Vec2(1, 1));
		sp->setScale(1.0 / g_gameType);
		sp->setPosition(Vec2(GRID_SIZE * p.x / g_gameType, GRID_SIZE * p.y / g_gameType));
		addChild(sp, 2);
	}

	//显示game over
	auto gameOverLabel = Label::createWithSystemFont(str, "Arial", 100);
	gameOverLabel->setPosition(size / 2);
	addChild(gameOverLabel, 3);
}

void GameScene::gameWin()
{
	_eventDispatcher->removeEventListener(listener);
	unschedule(schedule_selector(GameScene::updateTime));

	//显示You Win!
	auto gameWinLabel = Label::createWithSystemFont("You Win!", "Arial", 100);
	gameWinLabel->setPosition(size / 2);
	addChild(gameWinLabel, 3);
}

void GameScene::menuCallBack(Ref* obj)
{
	auto scene = SplashScene::createScene();
	Director::getInstance()->replaceScene(scene);
}