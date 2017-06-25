#include "stdafx.h"
#include "Cell.h"
#include <Windows.h>
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