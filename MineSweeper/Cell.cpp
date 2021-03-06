#include "stdafx.h"
#include "Cell.h"
#include <Windows.h>
#include <windowsx.h>
#include <sstream>

Cell::Cell()
{
	mbHasBee = false;
	mCellState = NOT_EXPOSED;
	mNeighbors = 0;
	//mCellState = EXPOSED;
}

void Cell::Draw(HDC hdc, int aLeft, int aTop, int aRight, int aBottom, bool abLost)
{
	HBRUSH hNewBrush, hOldBrush;

	hNewBrush = CreateSolidBrush(RGB(200, 200, 200));   // to fill with grey
	hOldBrush = (HBRUSH)SelectObject(hdc, hNewBrush);

	if (CurrState() == EXPOSED || abLost)
		SelectObject(hdc, hOldBrush);
	else
		SelectObject(hdc, hNewBrush);

	Rectangle(
		hdc,
		aLeft,
		aTop,
		aRight,
		aBottom
		);

	SelectObject(hdc, hOldBrush);
	DeleteObject(hNewBrush);
	//if (mbHasBee) DrawBee(hdc, aLeft, aTop, aRight, aBottom);
	if (mbHasBee && IsExposed()) DrawBee(hdc,aLeft,aTop,aRight,aBottom);
	else if (IsFlagged()) DrawFlag(hdc, aLeft, aTop, aRight, aBottom);
	else if (Neighbors() > 0 && IsExposed()) WriteText(hdc, aLeft, aTop, aRight, aBottom);

}

void Cell::DrawBee(HDC hdc, int aLeft, int aTop, int aRight, int aBottom)
{
	int len = (aRight - aLeft);
	double Ratio = 0.3;
	double lenRatio = ((1 - Ratio)*len) / 2;

	HBRUSH hNewBrush, hOldBrush;

	hNewBrush = CreateSolidBrush(RGB(0, 0, 0));   // to fill with black
	hOldBrush = (HBRUSH)SelectObject(hdc, hNewBrush);

	Ellipse(hdc, (long)(aLeft + lenRatio), (long)(aTop + lenRatio), (long)(aRight - lenRatio), (long)(aBottom - lenRatio));

	SelectObject(hdc, hOldBrush);
	DeleteObject(hNewBrush);
}

void Cell::DrawFlag(HDC hdc, int aLeft, int aTop, int aRight, int aBottom)
{
	int len = (aRight - aLeft);
	double Ratio = 0.08;
	double width = Ratio*len;
	
	double xx = (double)(aLeft)+((double)len * (double)0.25);
	double yy = (double)(aTop)+((double)len * (double) 0.25);
	double yy_end = yy + ((double)len / (double)2.0) + 1;
	double xx_end = xx + ((double)len / (double)2.4);
	double xx_extreme = xx_end + ((double)len / (double)10.0);
	double yy_mid = yy + ((double)len / (double)6.0);
	double yy_flag_end = yy + ((double)len * 0.3);

	POINT polPoints[10];
	polPoints[0] = { (int)xx, (int)yy };
	polPoints[1] = { (int)xx_end, (int)yy };
	polPoints[2] = { (int)((xx_extreme+xx_end+1)*0.5),(int)((yy_mid + yy + 1)*0.5) };
	polPoints[3] = { (int)(xx_extreme), (int)(yy_mid) };
	polPoints[4] = { (int)((xx_extreme + xx_end+ 1) *0.5), (int)((yy_mid + yy_flag_end + 1) *0.5) };
	polPoints[5] = { (int)xx_end, (int)(yy_flag_end+width/2.0) };
	polPoints[6] = { (int)xx, (int)yy_flag_end };
	polPoints[7] = { (int)(xx + ((double)len / (double)20.0)), (int)((yy_mid + yy_flag_end) * 0.5) };
	polPoints[8] = { (int)(xx + ((double)len / (double)10.0)), (int)(yy_mid) };
	polPoints[9] = { (int)(xx + ((double)len / 15.0)), (int)((yy + yy_mid) / 2.0) };

	HBRUSH hOldBrush, hRedBrush;

	hRedBrush = CreateSolidBrush(RGB(254, 0, 0));   // to fill with red
	hOldBrush = SelectBrush(hdc, hRedBrush);

	HPEN hNewPen = CreatePen(PS_SOLID, (int)(width), RGB(0,0,0));
	HPEN hOldPen = SelectPen(hdc, hNewPen);
	
	LPPOINT lpPoint = new POINT();
	
	MoveToEx(hdc, (int)xx, (int)(yy - width / 2.0), lpPoint);
	LineTo(hdc,(int)xx,(int)yy_end);
	
	SelectObject(hdc, hNewPen);
	Polygon(hdc, polPoints, 10);

	// Ellipse(hdc, (long)(aLeft + lenRatio), (long)(aTop + lenRatio), (long)(aRight - lenRatio), (long)(aBottom - lenRatio));
	
	SelectPen(hdc, hOldPen);
	SelectBrush(hdc, hOldBrush);
	DeleteObject(hRedBrush); 
	DeleteObject(hNewPen);
	
}

void Cell::WriteText(HDC hdc, int aLeft, int aTop, int aRight, int aBottom)
{
	RECT rc;
	rc.left = aLeft;
	rc.right = aRight;
	rc.top = aTop;
	rc.bottom = aBottom;

	int len = aLeft - aRight;
	double ratio = 0.5;
	int textsize = (int)((double)(len*ratio));

	//Convert int to LPCSTR (wchar_t*)
	std::string str = std::to_string(Neighbors());
	const char* result = str.c_str();
	wchar_t* wString = new wchar_t[5];
	MultiByteToWideChar(CP_ACP, 0, result, -1, wString, 5);
	
	
	SetTextColor(hdc, RGB(0, 0, 0));
	HFONT hFont = CreateFont(textsize, textsize/2, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0/*textsize/3*/, 0, LPCWSTR("SYSTEM_FIXED_FONT"));
	HFONT hTmp = (HFONT)SelectObject(hdc, hFont);
	SetBkMode(hdc, TRANSPARENT);
	
	DrawText(hdc, wString, -1, &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	DeleteObject(SelectObject(hdc, hTmp));
}