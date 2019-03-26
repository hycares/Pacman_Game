#include "stdafx.h"
#include "PacManMap.h"

//PacMap:
int PacMap::iBarrierSize = 36;
int PacMap::iPeaSize = 3;

void PacMap::ShowPosition()
{
	PeaMap[ENESHOW_R][ENESHOW_C] = false;
	PeaMap[PACSHOW_R][PACSHOW_C] = false;
}

void PacMap::DrawMap(HDC hdc)
{
	RECT rect;
	HBRUSH hBrush;

	srand(static_cast<unsigned int>(time(NULL)));
	int style = rand()%4;

	if (style == 0)
	{
		hBrush = CreateSolidBrush(color);
	}
	else if (style == 1)
	{
		hBrush = CreateHatchBrush(HS_CROSS, color);
	}
	else if (style == 2)
	{
		hBrush = CreateHatchBrush(HS_DIAGCROSS, color);
	}
	else
	{
		hBrush = CreateHatchBrush(HS_BDIAGONAL, color);
	}
	//row,col Ìæ»»³É row col
	for (int row=0; row<MAPLENTH; row++)
	{
		for (int col=0; col<MAPLENTH; col++)
		{
			if(!BarrierMap[row][col])
			{
				rect.left = col*iBarrierSize;
				rect.top = row*iBarrierSize;
				rect.right = (col+1)*iBarrierSize;
				rect.bottom = (row+1)*iBarrierSize;

				FillRect(hdc, &rect, hBrush);
			}
		}
	}
}

void PacMap::DrawPea(HDC hdc)
{
	HBRUSH hBrush;
	HBRUSH hOldBrush;

	hBrush = CreateSolidBrush(RGB(232, 231, 39));
	hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

	for (int i=0; i<MAPLENTH; i++)
	{
		for (int j=0; j<MAPLENTH; j++)
		{
			if (PeaMap[i][j])
			{
				Ellipse(hdc,(iBarrierSize/2-iPeaSize)+j*iBarrierSize,
					(iBarrierSize/2-iPeaSize)+i*iBarrierSize,
					(iBarrierSize/2+iPeaSize)+j*iBarrierSize,
					(iBarrierSize/2+iPeaSize)+i*iBarrierSize);
			}
		}
	}

	DeleteObject(hBrush);
	SelectObject(hdc, hOldBrush);
}

//End

//Stage:

Stage::Stage(int i)
	:m_iSta(i)
{
}

void Stage::InitColor()
{
	unsigned r, g, b;
	srand(static_cast<unsigned int>(time(NULL)));
	r = rand()%256;
	g = (rand()+r)%256;
	b = (rand()+r+g)%256;
	if (r==255 && g==255 && b==255)
	{
		r = rand()%256;
		g = (rand()+r)%256;
		b = (rand()+r+g)%256;		
	}
	color = RGB(r, g, b);
}

bool Stage::InitialMap()
{
	fstream fileMap;
	char temp;
	char stage_n[15];

	sprintf_s(stage_n, "stage_%d.dat", m_iSta);
	fileMap.open(stage_n, ios::in);

	if (fileMap == NULL)
	{
		MessageBox(NULL, TEXT("error: Stage file error!"), TEXT("PacTips"), MB_ICONERROR);
		return false;
	}
	
	fileMap.seekg(0);
	
	for (int row=0; row<MAPLENTH; row++)
	{
		for (int col=0; col<MAPLENTH; col++)
		{
			fileMap >> temp;
			if (temp == 'A')
				InitMap[row][col] = true;
			else
				InitMap[row][col] = false;
		}
	}

	for (int i=0; i<MAPLENTH; i++)
	{
		for (int j=0; j<MAPLENTH; j++)
		{
			this->BarrierMap[i][j] = this->InitMap[i][j];
			this->PeaMap[i][j] = this->InitMap[i][j];
		}
	}

#ifdef TEST
	for (int i=0; i<MAPLENTH; i++)
	{
		for (int j=0; j<MAPLENTH; j++)
		{
			this->PeaMap[i][j] = false;
		}
	}

	if (BarrierMap[9][10] == false)
		BarrierMap[9][10] = true;
	PeaMap[9][10] = true; 
#endif

	this ->ShowPosition();
	InitColor();

	fileMap.close();

	return true;
}

void Stage::StageWin()
{
	m_iSta++;
}

unsigned int Stage::GetStage()
{
	return m_iSta;
}

//End
