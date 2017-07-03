#include "stdafx.h"
#include "Grid.h"
#include <stdlib.h>
#include <time.h>
#include <string>
#include <sstream>

Grid::Grid()
{
	myCells = nullptr;
	_CellInitialized = false;
	mbLost = false;
	mbWon = false;
	mNumBees = -1;
	mFlaggedBees = 0;
}

Grid::~Grid()
{
	DeleteCells();
}

void Grid::Initialize(HWND hWnd, int aGridSize, int numBees)
{
	mhWnd = hWnd;
	mSize = aGridSize;
	_CellInitialized = false;
	mbLost = false;
	mbWon = false;
	mNumBees = numBees;
	mFlaggedBees = 0;

	RECT lpRect;
	GetClientRect(mhWnd, &lpRect);
	int x = lpRect.right - lpRect.left;
	int y = lpRect.bottom - lpRect.top - 25;
	double lGridPixelSize = min(x, y);

	double lOffset = 0.01 * lGridPixelSize;
	double lLeftOffset = ((x - lGridPixelSize) / 2.0) + (lOffset);
	double lTopOffset = ((y - lGridPixelSize) / 2.0) + (lOffset);
	double lLength = (lGridPixelSize - (2.0*lOffset)) / mSize;

	mTop = (int)(lTopOffset);
	mLeft = (int)(lLeftOffset);
	mPixelLength = (int)(lLength);

	mpTextRect.top = (LONG)(mTop + lGridPixelSize);
	mpTextRect.left = (LONG)(mLeft);
	mpTextRect.right = (LONG)(mLeft + lGridPixelSize);
	mpTextRect.bottom = (LONG)(mpTextRect.top + 15);

	InitializeCells();
}

void Grid::InitializeCells()
{
	if (_CellInitialized)
		DeleteCells();
	
	myCells = new Cell* [mSize];
	for (int i = 0; i < mSize; ++i)
		myCells[i] = new Cell[mSize];

	DistributeBees();

	for (int i = 0; i < mSize; ++i)
	{
		for (int j = 0; j < mSize; ++j)
		{
			if (myCells[i][j].HasBee()) continue;
			CountBees(i, j);
		}
	}

	_CellInitialized = true;
}

void Grid::DistributeBees()
{
	int iix(-1), jjy(-1);
	srand(time(NULL));

	for (int i = 0; i < mNumBees; ++i)
	{
		int randNumx = (long)rand() % 1000;
		int randNumy = (long)rand() % 1000;

		int ii = (int)(min((double)randNumx / 1000.0 * mSize, mSize - 1));
		int jj = (int)(min((double)randNumy / 1000.0 * mSize, mSize - 1));

		while (myCells[ii][jj].HasBee() || (ii == iix && jj == jjy))
		{
			int randNum = (long)rand() % 1000;
			if (randNum < 245)
			{
				if (ii < mSize - 1) ii++;
				else ii--;
			}
			else if (randNum < 495)
			{
				if (ii > 0) ii--;
				else ii++;
			}
			else if (randNum < 745)
			{
				if (jj < mSize - 1) jj++;
				else jj--;
			}
			else
			{
				if (jj > 0) jj--;
				else jj++;
			}
		}

		iix = ii;
		jjy = jj;
		myCells[ii][jj].AddBee();
	}

}

void Grid::CountBees(int i, int j)
{
	for (int ii = i - 1; ii <= i + 1; ++ii)
	{
		if (ii < 0 || ii >= mSize) continue;
		for (int jj = j - 1; jj <= j + 1; ++jj)
		{
			if (jj < 0 || jj >= mSize) continue;

			if (myCells[ii][jj].HasBee())
				myCells[i][j].Neighbors()++;
		}
	}
}

void Grid::DeleteCells()
{
	if (!_CellInitialized) return;

	for (int i = 0; i < mSize; ++i)
	{
		delete[] myCells[i]; myCells[i] = nullptr;
	}

	delete[] myCells; myCells = nullptr;
	_CellInitialized = false;
}

bool Grid::OnRightClick(long xx, long yy)
{
	if (mbWon) return false;

	int lengthX = xx - mLeft;
	int lengthY = yy - mTop;

	if (lengthX < 0 || lengthY < 0) return false;

	int indexI = lengthX / mPixelLength;
	int indexJ = lengthY / mPixelLength;	
	
	if (indexI < mSize && indexJ < mSize)
	{
		if (myCells[indexI][indexJ].IsExposed())
			return false;

		if (myCells[indexI][indexJ].IsFlagged())
		{
			myCells[indexI][indexJ].UnFlagCell();
			--mFlaggedBees;
		}
		else
		{
			myCells[indexI][indexJ].FlagCell();
			++mFlaggedBees;
		}
		return true;
	}
	return false;
}

bool Grid::OnClick(long xx, long yy)
{
	if (mbWon) return false;

	int lengthX = xx - mLeft;
	int lengthY = yy - mTop;

	if (lengthX < 0 || lengthY < 0) return false;

	int indexI = lengthX / mPixelLength;
	int indexJ = lengthY / mPixelLength;

	if (indexI < mSize && indexJ < mSize)
	{
		if (myCells[indexI][indexJ].IsExposed()) return false;
		if (myCells[indexI][indexJ].IsFlagged()) return false;

		myCells[indexI][indexJ].ExposeCell();

		if (myCells[indexI][indexJ].HasBee())
			ExposeAllBees();
		else if (myCells[indexI][indexJ].Neighbors() == 0)
			ExposeNeighboringZeros(indexI, indexJ);

		CheckIfWon();

		return true;
	}
	return false;
}

bool Grid::CheckIfWon()
{
	for (int i = 0; i < mSize; ++i){
		for (int j = 0; j < mSize; ++j)
		{
			if (!myCells[i][j].HasBee() && !myCells[i][j].IsExposed())
			{
				mbWon = false;
				return mbWon;
			}
		}
	}

	mbWon = true;
	return mbWon;
}

void Grid::ExposeAllBees()
{
	mbLost = true;
	for (int i = 0; i < mSize; ++i)
		for (int j = 0; j < mSize; ++j)
			if (myCells[i][j].HasBee()) myCells[i][j].ExposeCell();

}

void Grid::ExposeNeighboringZeros(int i, int j)
{
	for (int ii = i - 1; ii <= i + 1; ++ii)
	{
		if (ii < 0 || ii >= mSize) continue;
		for (int jj = j - 1; jj <= j + 1; ++jj)
		{
			if (jj < 0 || jj >= mSize) continue;
			if (ii == i && jj == j) continue;
			if (myCells[ii][jj].HasNonBeeNeighbor())
			{
				myCells[ii][jj].ExposeCell();
				if (myCells[ii][jj].Neighbors() == 0)
				{
					ExposeNeighboringZeros(ii, jj);
				}
			}
		}
	}
}

void Grid::DisplayMessage(LPCWSTR aMainMessage, LPCWSTR aMenuItem)
{
	int msgboxID = MessageBox(mhWnd, aMainMessage, aMenuItem, MB_ICONINFORMATION | MB_RETRYCANCEL);
	switch (msgboxID)
	{
	case IDRETRY:
	{
		Initialize(mhWnd, mSize,mNumBees);
		InvalidateRect(mhWnd, NULL, true);
		break;
	}
	case IDCANCEL:
		// TODO: add code
		break;
	}
}

void Grid::Draw()
{
	if (!mhWnd) return;

	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(mhWnd, &ps);
	HBRUSH hRedBrush = CreateSolidBrush(RGB(255, 0, 0));
	HBRUSH hOldBrush;

	if (mbLost) hOldBrush = (HBRUSH)SelectObject(hdc, hRedBrush);

	for (int i = 0; i < mSize; ++i)
	{
		for (int j = 0; j < mSize; ++j)
		{
			myCells[i][j].Draw(
				hdc,
				mLeft + i*mPixelLength,
				mTop + j*mPixelLength,
				mLeft + (i + 1)*mPixelLength,
				mTop + (j + 1)*mPixelLength,
				mbLost
				);
		}
	}

	DrawInformation(hdc);

	EndPaint(mhWnd, &ps);


	if (mbWon)
		DisplayMessage((LPCWSTR)L"Congratulations. You have Won. Please play again.", (LPCWSTR)L"Congratulations...");
	else if (mbLost)
		DisplayMessage((LPCWSTR)L"Sorry. You have Lost. Please try again.", (LPCWSTR)L"Sorry..");

}

void Grid::DrawInformation(HDC hdc)
{
	std::string TotalBees = "TOTAL BEES = " + std::to_string(mNumBees);
	std::string TotalFlagged = "TOTAL FLAGGED = " + std::to_string(mFlaggedBees);

	const char* resultTotalBees = TotalBees.c_str();
	const char* resultTotalFlagged = TotalFlagged.c_str();

	wchar_t* wStringTotalBees = new wchar_t[25];
	wchar_t* wStringTotalFlagged = new wchar_t[25];

	MultiByteToWideChar(CP_ACP, 0, resultTotalBees, -1, wStringTotalBees,25);
	MultiByteToWideChar(CP_ACP, 0, resultTotalFlagged, -1, wStringTotalFlagged, 25);
	
	SetTextColor(hdc, RGB(0, 0, 0));
	
	HFONT font = CreateFont(18, 7, 0, 0, FW_BOLD, false, false,false,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH,
		(LPCWSTR)L"Arial");
	
	HFONT hFontOld = (HFONT)SelectObject(hdc, font);

	DrawText(hdc, wStringTotalBees, -1, &mpTextRect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	DrawText(hdc, wStringTotalFlagged, -1, &mpTextRect, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);

	SelectObject(hdc, hFontOld);
	DeleteObject(font);
}