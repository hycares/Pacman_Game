#ifndef PACMANMOVE_H
#define PACMANMOVE_H

#include "stdafx.h"
#include "PacManMap.h"
#include "PacSearch.h"

#define PACSPEED 6 // PacMan 速度
#define ENESPEED 4
//#define ENEMAXSP 5.5 //当敌人发现PacMan后的速度
#define DISTANCE 10//图形范围
#define DRAWDIFF 2 //绘图
#define DRAWDIST 12 
#define REDESCAN 8
#define BLUESCAN 30 //蓝色敌人监视范围 8
//#define ENECRAZY 5
#define LEGCOUNT 10

enum ToWard {UP, DOWN, LEFT, RIGHT, CATCH};//方向

class PacMove
{
public:
	int GetRow();
	int GetCol();

	void SetPosition(int, int);
	void DrawBlank(HDC);//绘制基本地图
	void virtual Draw(HDC hdc) = 0;//绘制PacMan Enemy,应用基本的Windows GDI
	void virtual MoveBehavior() = 0;//相当于多态吧 不同的派生类的行为不同
	PacMove(int, int);

	static PacMap* pMapStage;//必须是静态成员

protected:
	bool IsCollision(); //判断是否碰撞
	bool IsCPoint();   //判断是否到达中心位置
	int TransPoint(int);//坐标转换
	virtual void CenterPoint();//更新中心位置

	int m_iX;
	int m_iY;
	int m_iRow;
	int m_iCol;
	int m_iSpeed;
	int m_iFrame;
	ToWard tw;
	ToWard twCmd;
	POINT pt;
};

//PacMan
class PacMan : public PacMove
{
public:
	PacMan(int, int);
	POINT GetPosition();
	ToWard GetToward();
	bool Win();
	void Draw(HDC);
	void SetTwCmd(ToWard);
	void Catch();
	void MoveBehavior();
	void SysPause();
	unsigned int GetScore();
protected:
	virtual void  CenterPoint();
	unsigned int m_uScore;
};

//Enemy
class Enemy : public PacMove
{
public:
	Enemy(int, int);
	void virtual Draw(HDC);
	void MoveBehavior();

	static PacMan* pacman;

protected:
	void Catch();//PacMan被抓判断
	void virtual EnemyAI(bool) = 0;//不同颜色AI程度不同

	COLORREF color;
};

class RedEne : public Enemy
{
public:
	RedEne(int, int);
	void Draw(HDC);

protected:
	void EnemyAI(bool);
};

class BlueEne : public RedEne
{
public:
	BlueEne(int, int);
	void Draw(HDC);

protected:
	void EnemyAI(bool);
};

//绿色智能程度最高..棒棒
class GreenEne : public RedEne
{
public:
	GreenEne(int, int);
	void Draw(HDC);

	PacStack GreenStack;
	PacPoint NextPoint;
	PacSearch FindPac;

protected:
	void EnemyAI(bool);
};

#endif