/*
Core File
基础移动类的位置检测, 碰撞检测, 地图更新
指令判断
*/

#include "stdafx.h"
#include "PacManMove.h"

//PacMove:
PacMap* PacMove::pMapStage = NULL;

PacMove::PacMove(int row, int col)
{
	m_iFrame = 1;
	pMapStage = NULL;
	
	this->m_iRow = row;
	this->m_iCol = col;
	this->pt.x = m_iCol*(pMapStage->iBarrierSize)+(pMapStage->iBarrierSize/2);
	this->pt.y = m_iRow*(pMapStage->iBarrierSize)+(pMapStage->iBarrierSize/2);
	this->m_iX = pt.x;
	this->m_iY = pt.y;
}

//将客户区坐标装换为矩阵坐标[行][列]
int PacMove::TransPoint(int RealPt)
{
	int temp = 0;
	/*
	窗口坐标的转换是在PacMan或Enemies处在每一个位置中心点
	进行判断, 在移动过程中则无法判断,也不需要判断,
	只要在方向更改时确定坐标即可
	以下 表达式中 -障碍物尺寸/2 目的是消除中心距离
	*/
	temp = (RealPt - pMapStage->iBarrierSize/2)/(pMapStage->iBarrierSize);

	return temp;
}

bool PacMove::IsCPoint()
{
	//碰撞前坐标判断, 和转换不同需要计算移动时是否到达中心点
	int n = (pt.x - pMapStage->iBarrierSize/2) % pMapStage->iBarrierSize;
	int k = (pt.y - pMapStage->iBarrierSize/2) % pMapStage->iBarrierSize;

	bool flag = (n==0 && k==0);

	return flag;
}

//更新地图行列
void PacMove::CenterPoint()
{
	if (IsCPoint())
	{
		m_iCol = TransPoint(pt.x);
		m_iRow = TransPoint(pt.y);
	}
}

bool PacMove::IsCollision()
{
	bool flag = false;

	//计入地图最大宽度, 使得PacMan在地图最一端是能进入到另一端
	int MapMaxLenth = pMapStage->iBarrierSize*MAPLENTH+pMapStage->iBarrierSize/2;
	int MapMinLenth = pMapStage->iBarrierSize/2;

	//刷新
	CenterPoint();

	if (m_iCol < 0 || m_iRow < 0 || (m_iCol > MAPLENTH-1) || m_iRow > (MAPLENTH -1))
	{
		flag = true;
	}
	else if (IsCPoint())
	{
		//此处判断前行方向指令是否有效
		switch (twCmd)
		{
		case UP:
			if (m_iRow > 0 && !pMapStage->BarrierMap[m_iRow-1][m_iCol])//逻辑地图中障碍物为假
			{
				flag = true;//不能前进
			}
			break;
		case DOWN:
			if (m_iRow < (MAPLENTH-1) && !pMapStage->BarrierMap[m_iRow+1][m_iCol])
			{
				flag = true;
			}
			break;
		case LEFT:
			if (m_iCol > 0 && !pMapStage->BarrierMap[m_iRow][m_iCol-1])
			{
				flag = true;
			}
			break;
		case RIGHT:
			if (m_iCol < (MAPLENTH - 1) && !pMapStage->BarrierMap[m_iRow][m_iCol+1])
			{
				flag = true;
			}
			break;
		}

		if (!flag)//行进方向没有障碍物
		{
			tw = twCmd;
		}
	}

	m_iX = pt.x;
	m_iY = pt.y;

	//此处是控制前行
	switch (tw)
	{
	case UP:
		if (m_iRow > 0 && !pMapStage->BarrierMap[m_iRow-1][m_iCol])//逻辑地图中障碍物为假
		{
			//行进方向有墙
			flag = true;
			break;
		}
		pt.y -= m_iSpeed;
		if (pt.y < MapMinLenth)
		{
			//向上行进后进入底部
			pt.y = MapMaxLenth;
		}
		break;
	case DOWN:
		if (m_iRow < (MAPLENTH-1) && !pMapStage->BarrierMap[m_iRow+1][m_iCol])
		{
			flag = true;
			break;
		}
		pt.y += m_iSpeed;
		if (pt.y > MapMaxLenth)
		{
			//向下行进后进入顶部
			pt.y = MapMinLenth;
		}
		break;
	case LEFT:
		if (m_iCol > 0 && !pMapStage->BarrierMap[m_iRow][m_iCol-1])
		{
			flag = true;
			break;
		}
		pt.x -= m_iSpeed;
		if (pt.x < MapMinLenth)
		{
			//向左行进后进入右端
			pt.x = MapMaxLenth;
		}
		break;
	case RIGHT:
		if (m_iCol < (MAPLENTH - 1) && !pMapStage->BarrierMap[m_iRow][m_iCol+1])
		{
			flag = true;
			break;
		}
		pt.x += m_iSpeed;
		if (pt.x > MapMaxLenth)
		{
			//向右行进后进入左端
			pt.x = MapMinLenth;
		}
		break;
	}
	
	return flag;
}

void PacMove::DrawBlank(HDC hdc)
{
	RECT rect;

	rect.top = m_iY - DRAWDIST;
	rect.bottom = m_iY + DRAWDIST;
	rect.left = m_iX - DRAWDIST;
	rect.right = m_iX + DRAWDIST;
	
	FillRect(hdc, &rect, CreateSolidBrush(RGB(255,255,255)));
}

void PacMove::SetPosition(int Row, int Col)
{
	m_iRow = Row;
	m_iCol = Col;

	this->pt.x = m_iCol*pMapStage->iBarrierSize + pMapStage->iBarrierSize/2;
	this->pt.y = m_iRow*pMapStage->iBarrierSize + pMapStage->iBarrierSize/2;
}

int PacMove::GetRow()
{
	return m_iRow;
}

int PacMove::GetCol()
{
	return m_iCol;
}

//End 

//PacMan:
PacMan::PacMan(int x, int y)
	:PacMove(x, y)
{
	this->m_iSpeed = PACSPEED;
	twCmd = tw = LEFT;
	m_uScore = 0;
}

void PacMan::MoveBehavior()
{
	IsCollision();
}

void PacMan::CenterPoint()
{
	PacMove::CenterPoint();

	if (IsCPoint())
	{
		if (m_iRow>=0 && m_iRow<MAPLENTH && m_iCol>=0 && m_iCol<=MAPLENTH)
		{
			if (pMapStage->PeaMap[m_iRow][m_iCol])
			{
				//吃掉豆子
				PlaySound(TEXT("sndcoin.wav"), NULL, SND_ASYNC | SND_FILENAME);
				m_uScore++;
				pMapStage->PeaMap[m_iRow][m_iCol] = false;
			}
		}
	}
}

void PacMan::SetTwCmd(ToWard cmd)
{
	twCmd = cmd;
}

bool PacMan::Win()
{
	//当地图中有无豆子时则Win
	for (int row=0; row<MAPLENTH; row++)
	{
		for (int col=0; col<MAPLENTH; col++)
		{
			if (pMapStage->PeaMap[row][col] == true)
			{
				return false;
			}
		}
	}

	PlaySound(TEXT("sndwin.wav"), NULL, SND_FILENAME | SND_ASYNC);
	return true;
}

unsigned int PacMan::GetScore()
{
	return m_uScore;
}

POINT PacMan::GetPosition()
{
	return pt;
}

void PacMan::Catch()
{
	tw = CATCH;
}

ToWard PacMan::GetToward()
{
	return tw;
}

void PacMan::Draw(HDC hdc)
{
	int x1 = 0, x2 = 0;
	int y1 = 0, y2 = 0;
	int offsetX;
	int offsetY;
	
	if (tw == CATCH)
	{
		
	}
	else if (m_iFrame % 2 == 0)
	{
		//嘴巴朝向

		 offsetX = DISTANCE/2+DRAWDIFF;
		 offsetY = DISTANCE/2+DRAWDIFF;

		 switch (tw)
		 {
		 case UP:
			 x1 = pt.x - offsetX;
			 x2 = pt.x + offsetX;
			 y1 = y2 = pt.y - offsetY;
			 break;
		 case DOWN:
			 x1 = pt.x + offsetX;
			 x2 = pt.x - offsetX;
			 y1 = y2 = pt.y + offsetY;
			 break;
		 case LEFT:
			 x1 = x2 = pt.x - offsetX;
			 y1 = pt.y + offsetY;
			 y2 = pt.y - offsetY;
			 break;
		 case RIGHT:
			 x1 = x2 = pt.x + offsetX;
			 y1 = pt.y - offsetY;
			 y2 = pt.y + offsetY;
			 break;
		 }

		 Arc(hdc, pt.x-DISTANCE, pt.y-DISTANCE, pt.x+DISTANCE,
			 pt.y+DISTANCE, x1, y1, x2, y2);
		 MoveToEx(hdc, x1, y1, NULL);
		 LineTo(hdc, pt.x, pt.y);
		 LineTo(hdc, x2, y2);
	}
	else if (m_iFrame % 3 == 0)
	{
		Ellipse(hdc, pt.x-DISTANCE, pt.y-DISTANCE,
			pt.x+DISTANCE, pt.y+DISTANCE);
	}
	else
	{
		switch (tw)
		{
		case UP:
			x1 = pt.x - DISTANCE;
			x2 = pt.x + DISTANCE;
			y1 = y2 = pt.y;
			break;
		case DOWN:
			x1 = pt.x + DISTANCE;
			x2 = pt.x - DISTANCE;
			y1 = y2 = pt.y;
			break;
		case LEFT:
			x1 = x2 = pt.x;
			y1 = pt.y + DISTANCE;
			y2 = pt.y - DISTANCE;
			break;
		case RIGHT:
			x1 = x2 = pt.x;
			y1 = pt.y - DISTANCE;
			y2 = pt.y + DISTANCE;
			break;
		}

		Arc(hdc, pt.x-DISTANCE, pt.y-DISTANCE, pt.x+DISTANCE, pt.y+DISTANCE,
			x1, y1, x2, y2);
		MoveToEx(hdc, x1, y1, NULL);
		LineTo(hdc, pt.x, pt.y);
		LineTo(hdc, x2, y2);
	}

	m_iFrame++;
	if (m_iFrame == 100)
		m_iFrame = 0;
}

//暂停功能
void PacMan::SysPause() //增加暂停提示
{
	//::FillRect(hdc, &rect, CreateSolidBrush(RGB(255,255,255)));
	while (true)
	{
		Sleep(1000);
		if ((GetAsyncKeyState(VK_SPACE) & 0x800f) || (GetAsyncKeyState('P') & 0x800f) ||
			(GetAsyncKeyState(VK_PAUSE) & 0x800f))
			break;
	}
}
//End

//Enemy:
PacMan* Enemy::pacman = NULL;

Enemy::Enemy(int x, int y)
	:PacMove(x, y)
{
	this->m_iSpeed = ENESPEED;
	tw = LEFT;
	twCmd = UP;
}

void Enemy::Catch()
{
	//通过坐标之间的差值和绘制图形的大小来判断是否被抓

	int EneDX = pt.x - pacman->GetPosition().x;
	int EneDY = pt.y - pacman->GetPosition().y;

	if ((-DRAWDIST < EneDX && EneDX < DRAWDIST) &&
		(-DRAWDIST < EneDY && EneDY < DRAWDIST))
	{
		PlaySound(TEXT("sndcatch.wav"), NULL, SND_FILENAME | SND_ASYNC);
		pacman->Catch();
	}
}

void Enemy::Draw(HDC hdc)
{
	HPEN pen = CreatePen(NULL, 0, color);
	HPEN prevPen = (HPEN)SelectObject(hdc, pen);

	//头部弧线
	Arc(hdc, pt.x-DISTANCE, pt.y-DISTANCE, pt.x+DISTANCE, pt.y+DISTANCE,
		pt.x+DISTANCE, pt.y, pt.x-DISTANCE, pt.y);
	
	const int LegLenth = DISTANCE / LEGCOUNT;
	
	if (m_iFrame % 2 == 0)
	{
		MoveToEx(hdc, pt.x-DISTANCE, pt.y, NULL);
		LineTo(hdc, pt.x-DISTANCE, pt.y+DISTANCE-LegLenth);
		MoveToEx(hdc, pt.x+DISTANCE, pt.y, NULL);
		LineTo(hdc, pt.x+DISTANCE, pt.y+DISTANCE-LegLenth);

		//绘制enemy腿部
		for (int i=0; i<LEGCOUNT; i++)
		{
			Arc(hdc, pt.x-DISTANCE+i*2*LegLenth, pt.y+DISTANCE-2*LegLenth,
				pt.x-DISTANCE+(i+1)*2*LegLenth, pt.y+DISTANCE,
				pt.x-DISTANCE+i*2*LegLenth, pt.y+DISTANCE-LegLenth,
				pt.x-DISTANCE+(i+1)*2*LegLenth, pt.y+DISTANCE-LegLenth);
		}
	}
	else
	{
		MoveToEx(hdc, pt.x-DISTANCE, pt.y, NULL);
		LineTo(hdc, pt.x-DISTANCE, pt.y+DISTANCE);
		MoveToEx(hdc, pt.x+DISTANCE, pt.y, NULL);
		LineTo(hdc, pt.x+DISTANCE, pt.y+DISTANCE);

		MoveToEx(hdc, pt.x-DISTANCE, pt.y+DISTANCE, NULL);
		LineTo(hdc, pt.x-DISTANCE+LegLenth, pt.y+DISTANCE-LegLenth);

		for (int i=0; i<LEGCOUNT-1; i++)
		{
			Arc(hdc, pt.x-DISTANCE+(1+i*2)*LegLenth, pt.y+DISTANCE - 2*LegLenth,
				pt.x-DISTANCE+(3+i*2)*LegLenth, pt.y+DISTANCE,
				pt.x-DISTANCE+(1+i*2)*LegLenth, pt.y+DISTANCE-LegLenth,
				pt.x-DISTANCE+(3+i*2)*LegLenth, pt.y+DISTANCE-LegLenth);
		}

		MoveToEx(hdc, pt.x+DISTANCE, pt.y+DISTANCE, NULL);
		LineTo(hdc, pt.x+DISTANCE-LegLenth, pt.y+DISTANCE-LegLenth);
	}

	//Enemy eyes
	int radius = DISTANCE/5;

	switch (tw)
	{
	case UP:
		Ellipse(hdc, pt.x-2*radius, pt.y-2*radius, pt.x, pt.y);
		Ellipse(hdc, pt.x, pt.y-2*radius, pt.x+2*radius, pt.y);
		break;
	case DOWN:
		Ellipse(hdc, pt.x-2*radius, pt.y, pt.x, pt.y+2*radius);
		Ellipse(hdc, pt.x, pt.y, pt.x+2*radius, pt.y+2*radius);
		break;
	case LEFT:
		Ellipse(hdc, pt.x-3*radius, pt.y-radius, pt.x-radius, pt.y+radius);
		Ellipse(hdc, pt.x-radius, pt.y-radius, pt.x+radius, pt.y-radius);
		break;
	case RIGHT:
		Ellipse(hdc, pt.x-radius, pt.y-radius, pt.x+radius, pt.y+radius);
		Ellipse(hdc, pt.x+radius, pt.y-radius, pt.x+3*radius, pt.y+radius);
		break;
	}

	m_iFrame++;
	if (m_iFrame == 100)
		m_iFrame = 0;
	SelectObject(hdc, prevPen);
	DeleteObject(pen);
}

void Enemy::MoveBehavior()
{
	bool flag = IsCollision();
	EnemyAI(flag);
	Catch();
}

//End
/*
	修改算法:
	使得红色敌人能在红色监视范围内进行区域性追踪
	修改成蓝色算法
*/
//RedEnemy:
RedEne::RedEne(int x, int y)
	:Enemy(x, y)
{
	color = RGB(255, 0, 0);
}

void RedEne::Draw(HDC hdc)
{
	Enemy::Draw(hdc);
}

/*
进行随机移动并有一点追踪能力的敌人
逻辑:
当在敌人周边时, 会进行判断 大致以一个圆形区域
当出现在行列位置相同情况, 并在范围内时, 开始追逐 
否则进行随机移动.
同时,会进行撞墙判断, 如果PacMan保持位置不动, 则当其出现在墙旁边时,
会维持这一状态, 当其维持该位置后一段时间后, 将随机移动
*/
void RedEne::EnemyAI(bool flag)
{
	//加上这个srand会使得派生类行为一致, 主要是同时间
	//srand(static_cast<unsigned int>(time(NULL)));
	unsigned int r = rand();
	const int RowDis = this->m_iRow - pacman->GetRow();
	const int ColDis = this->m_iCol - pacman->GetCol();
	static int prevRow;
	static int prevCol;

	srand(static_cast<unsigned int>(time(NULL)) + r);
	int rCmd = rand(); //随机产生的方向命令

	if (flag)//碰撞
	{
		if (rCmd % 4 == 0)
		{
			tw == UP ? twCmd=LEFT : twCmd=UP;
		}
		else if (rCmd % 2 == 0)
		{
			tw == RIGHT ? twCmd=UP : twCmd=RIGHT;
		}
		else if (rCmd % 3 == 0)
		{
			tw == DOWN ? twCmd=RIGHT : twCmd=DOWN; 
		}
		else 
		{
			tw == LEFT ? twCmd=DOWN : twCmd=LEFT;
		}
	}
	else
	{
		if ((sqrt((abs(RowDis)*abs(RowDis) + abs(ColDis)*abs(ColDis))) <= 2*REDESCAN) &&
			(prevRow != this->m_iRow && prevCol != this->m_iCol))
		{
			if (RowDis == 0)
			{
				if (ColDis <= REDESCAN && ColDis > 0)
				{
					twCmd = LEFT;
				}
				else if (ColDis < 0 && ColDis >= -REDESCAN)
				{
					twCmd = RIGHT;
				}
			}
			else if (ColDis == 0)
			{
				if (RowDis <= REDESCAN && RowDis > 0)
				{
					twCmd = UP;
					return;
				}
				else if (RowDis < 0 && RowDis > -REDESCAN)
				{
					twCmd = DOWN;
				}
			}
		}
		if (rCmd % 2 == 0)
		{
			tw != RIGHT ? twCmd = LEFT : twCmd = RIGHT;
		}
		else if (rCmd % 3 == 0)
		{
			tw != DOWN ? twCmd = UP : twCmd = DOWN;
		}
		else
		{
			tw != LEFT ? twCmd = RIGHT : twCmd = LEFT;
		}
	}
	prevRow = this->m_iRow;
	prevCol = this->m_iCol;

	return;
}

//End

/*
	算法修改:
	增加行进速度, 当展开追逐时,提高速度, 并且使得
	需要想出一个合适的增加比例: 
	逻辑:
	当出现在同一行或列时, 并且范围在BLUESCAN中, 则能够开始直线追逐
	如果不满足上一条件, 则能够大范围的确定一下Pacman的位置,通过
	行列比较来确定位置, 非精确
	如果上述条件都不满足或撞墙, 则调用RedEnemy的逻辑
*/
//BlueEne:
BlueEne::BlueEne(int x, int y)
	:RedEne(x, y)
{
	color = RGB(0, 0, 255);
}

void BlueEne::Draw(HDC hdc)
{
	Enemy::Draw(hdc);
}

void BlueEne::EnemyAI(bool flag)
{
	const int RowDis = this->m_iRow - pacman->GetRow();
	const int ColDis = this->m_iCol - pacman->GetCol();

	if (!flag && RowDis == 0)
	{
		if (ColDis <= BLUESCAN && ColDis > 0)
		{
			twCmd = LEFT;
		}
		else if (ColDis < 0 && ColDis >= -BLUESCAN)
		{
			twCmd = RIGHT;
		}
	}
	else if (!flag && ColDis == 0)
	{
		if (RowDis <= BLUESCAN && RowDis > 0)
		{
			twCmd = UP;
			return;
		}
		else if (RowDis < 0 && RowDis > -BLUESCAN)
		{
			twCmd = DOWN;
		}
	}
	else if(!flag && (RowDis*RowDis > ColDis*ColDis))
	{
		if (ColDis > 0)
		{
			twCmd = LEFT;
		}
		else if (ColDis < 0)
		{
			twCmd = RIGHT;
		}
	}
	else if (!flag && (RowDis*RowDis < ColDis*ColDis))
	{
		if (RowDis > 0)
		{
			twCmd = UP;
		}
		else if (RowDis < 0)
		{
			twCmd = DOWN;
		}
	}
	else
	{
		RedEne::EnemyAI(flag);
	}
}

//End

/*
	修改算法:
	绿色的敌人从蓝色派生出:
	每间隔10s?或者多少 进行一次寻址,找到PacMan的位置,
	时间间隔可以通过获取系统时间来确定 或者在间隔绘图刷新时进行判断
	在间隔秒内使用蓝色搜索范围// 可以适当调高搜索范围使得定位更加进准
*/
//GreenEne

GreenEne::GreenEne(int x, int y)
	:RedEne(x, y)
{
	color = RGB(0, 255, 0);
}

void GreenEne::Draw(HDC hdc)
{
	Enemy::Draw(hdc);
}

void GreenEne::EnemyAI(bool flag)
{
	const int RowDis = this->m_iRow - pacman->GetRow();
	const int ColDis = this->m_iCol - pacman->GetCol();

	if (!flag)
	{
		if (GreenStack.IsEmpty())
		{
			if (RowDis*RowDis > ColDis*ColDis)
			{
				if (ColDis > 0)
				{
					twCmd = LEFT;
				}
				else if (ColDis < 0)
				{
					twCmd = RIGHT;
				}
			}
			else
			{
				if (RowDis > 0)
				{
					twCmd = UP;
				}
				else if (RowDis < 0)
				{
					twCmd = DOWN;
				}
			}
		}
		else
		{
			if (GreenStack.PopStack(this->NextPoint))
			{
				if (this->NextPoint.c_col == this->m_iCol)
				{
					if (this->NextPoint.c_row >= this->m_iRow)
					{
						twCmd =	DOWN;
					}
					else
					{
						twCmd = UP;
					}
				}
				else if (this->NextPoint.c_row == this->m_iRow)
				{
					if (this->NextPoint.c_col >= this->m_iCol)
					{
						twCmd = RIGHT;
					}
					else
					{
						twCmd = LEFT;
					}
				}
			}
			
		
		}
	}
	else
	{
		RedEne::EnemyAI(flag);
	}
}

//End;