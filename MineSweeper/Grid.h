#pragma once
#include "Cell.h"

class Grid
{
public:
	Grid();
	~Grid();
	void Initialize(HWND hWnd, int asize, int numBees);
	void InitializeCells();
	void CountBees(int i, int j);
	void DistributeBees();
	void DeleteCells();

	void Draw();
	void DrawInformation(HDC hdc);
	RECT GetGridRect(long xx, long yy);
	RECT GetGridRectFromIndex(int indexI, int indexJ);
	void GetGridIndex(long xx, long yy, int& ii, int& jj);
	RECT GetRectToPrint() { return mGridToPrint; }
	RECT GetRectForText() { return mpTextRect; }
	void CompareGridRect(RECT lpA);
	bool OnClick(long ax, long ay);
	bool OnRightClick(long ax, long ay);
	void ExposeAllBees();
	void ExposeNeighboringZeros(int i, int j);
	bool CheckIfWon();

	int Top() const { return mTop; }
	int Left() const { return mLeft; }
	int PixelLength() const { return mPixelLength; }
	int Size() const { return mSize; }
	bool HasLost() const { return mbLost; }
	void DisplayMessage(LPCWSTR aMainMessage, LPCWSTR aMenuItem);

private:
	RECT mpTextRect;
	RECT mGridToPrint;
	int mSize; // = n = size of a n x n grid
	HWND mhWnd; // Window Instance
	int mTop;
	int mLeft;
	int mPixelLength; // pixel size of each rectangle
	int mNumBees;
	int mFlaggedBees;

	Cell** myCells;
	bool _CellInitialized;
	double mDifficultyPercentage;
	
	bool mbLost;
	bool mbWon;
};
