#ifndef __GAMESCENE_H__
#define __GAMESCENE_H__

#include "cocos2d.h"

USING_NS_CC;
using namespace std;

const int GRID_WIDTH = 5;	//Easy难度是5x5网格
const int GRID_HEIGHT = 5;
const int MAP_WIDTH = 20;	//地图最大是20x20网格
const int MAP_HEIGHT = 20;
const int GRID_SIZE = 128;	//网格最大是128x128像素
const int MINES_NUMS[3] = { 4, 20, 45 };	//根据游戏难度分配的地雷数
const int TIMES[3] = { 60, 300, 666 };		//剩余时间

//网格实际类型
enum class GRID_REAL_TYPE		//0 数字, 1 地雷
{
	NUM,
	MINE
};

//网格表面类型
enum class GRID_FACE_TYPE		//0 遮挡砖块，1 数字, 2 旗帜
{
	NOTHING,
	NUM,
	FLAG
};

//网格结构
typedef struct 
{
	int number;				//周围地雷数
	GRID_REAL_TYPE realType;//网格实际类型
	GRID_FACE_TYPE faceType;//网格表面类型
	Sprite* sp;
}GRID;

typedef struct
{
	int x;
	int y;
}MyPoint;

class GameScene : public Layer
{
private:
	int minesNums;						//地雷数
	int remainderTime;					//剩余时间
	vector<MyPoint> minesPosition;		//地雷位置
	vector<MyPoint> wrongFlag;			//记录放错地方的旗帜
	GRID map[MAP_WIDTH + 2][MAP_HEIGHT + 2];	//游戏地图

	bool isClicked;						//用来判断单击或双击
	int m_x;							//点击坐标
	int m_y;
	int m_type;							//点击类型，0 左键，1 右键
	long startTime;						//记录游戏开始时间
	Size size;							//窗口大小
	Label * minesNumsLabel;				//地雷数标签
	Label * timeLabel;					//时间标签
	EventListenerMouse * listener;		//鼠标监听
	void onMouseDown(Event *event);		//处理鼠标点击事件
	void singleClicked(float t);		//处理单击事件
	void doubleClicked(int x, int y);	//处理双击事件
	void updateTime(float t);			//更新时间

	void setMines();					//放置地雷
	void setNumber();					//计算数字
	void showNumer(int x, int y, int n);//显示数字
	void showBlank(int x, int y);		//显示空白网格
	void showFlag(int x, int y);		//显示旗帜
	void showAround(int x, int y);		//双击显示周围数字
	void copewithAround(int x, int y);
	int calcFlagNumber(int x, int y);	//计算周围旗帜数量
	bool isWin();						//判读是否胜利
	void gameOver(string str);			//游戏结束
	void gameWin();						//游戏胜利

public:
	static Scene * createScene(int type);
	virtual bool init();
	void menuCallBack(Ref* obj);
	CREATE_FUNC(GameScene);
};

#endif