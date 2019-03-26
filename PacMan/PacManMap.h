#ifndef PACMANMAP_H
#define PACMANMAP_H

#include "stdafx.h"

/*
	地图大小长宽19单位构建19*19二维数组
	Pacman出现坐标为(9, 10)
	Enemy出现坐标为(9, 8)\
	R-ROW - Y
	C-COL - X
*/

#define MAPLENTH 19
#define PACSHOW_R 10 
#define PACSHOW_C 9
#define ENESHOW_R 8
#define ENESHOW_C 9

//基础地图类: 地图尺寸, 地图障碍物, Pea分布
class PacMap
{
	friend class PacMove;
	friend class PacMan;
public:
	void DrawMap(HDC);
	void DrawPea(HDC);

	PacMap(){}
	virtual ~PacMap(){}

	bool BarrierMap[MAPLENTH][MAPLENTH]; //障碍物地图要共享给搜索函数,所以变成public..减少复制一份地图吧...
protected:
	void ShowPosition();//PacMan Enemy出现位置

	//地图设计
	bool PeaMap[MAPLENTH][MAPLENTH];

protected:
	static int iBarrierSize; //障碍物大小
	static int iPeaSize;	 //豆子大小
	COLORREF color;
};

class Stage : public PacMap
{
public:
	Stage(int i=1); //初始关卡设置为1

	bool InitialMap();
	void StageWin();
	void InitColor();
	unsigned int GetStage();
	bool InitMap[MAPLENTH][MAPLENTH]; //文件连读取

private:
	unsigned int m_iSta;
	
};

#endif
