#ifndef PACSEARCH
#define PACSEARCH

#include "stdafx.h"
#include "PacManMap.h"

#define AVGVALUE 10 //搜索时的权重值
//地图标记
#define BARRIER  1
#define PATH  0
#define STARTPOS 2
#define DESPOS   3
#define MINPATH  4

struct PacPoint 
{
	int c_row;
	int c_col;
};

struct PacNode //用于搜索时记录的节点
{
	int g;
	int h;
	int f; //f = g + h;
	int row;
	int col;
	PacPoint dir;//指向方向位置
	PacNode *parent;
};

struct PacList //存放值的表 分open and close
{
	PacNode *L_pt;
	PacList *pNext;
};

struct PointNode //入栈节点
{
	PacPoint PN_pt;
	PointNode *pNext;
};

class PacStack //用来读取存放位置的栈结构
{
public:
	void InitStack();
	void PushStack(PacPoint);
	bool PopStack(PacPoint &);
	bool IsEmpty() const; 
	void ClearStack();

protected:
	PointNode *pTop;
	PointNode *pButtom;
};

class PacSearch
{
public:
	PacSearch();
	~PacSearch();
	static PacSearch *InitSearch();
	static void Destroy();
	PacNode *GetNode();
	void PushOpen(PacNode *); //进空表
	PacNode *FindOpen(int, int);
	void PushClose(PacNode *); //进已读取表, 已经排除的位置
	bool FindClose(int, int);
	bool IsBarrier(int, int);
	int GetHValue(int, int);
	int GetDesRow() const;
	int GetDesCol() const;
	void SetDesRow(int);
	void SetDesCol(int);
	void CreateNodeArea(PacNode *, int, int, int);//用于建立周围新节点
	void AreaNode(PacNode *); //在基于建立好的节点后建立连接

	void MinPath(bool [MAPLENTH][MAPLENTH], int, int, int, int, PacStack &); //寻找最短路径
	bool SEARCH(bool barrier[MAPLENTH][MAPLENTH], int StartRow, int StartCol, int DesRow, int DesCol, PacStack &GreenStack); //接口函数

private:
	int map_s[MAPLENTH][MAPLENTH];
	int m_iDesRow;
	int m_iDesCol;
};

#endif