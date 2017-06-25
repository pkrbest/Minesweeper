#pragma once
#include "Cell.h"
#define DIFFICULY_PERC 0.1

class Grid
{
public:
	Grid();
	~Grid();
	void Initialize(HWND hWnd, int asize);
	void InitializeCells();
	void CountBees(int i, int j);
	void DeleteCells();

	void Draw();
	bool OnClick(long ax, long ay);
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
	int mSize; // = n = size of a n x n grid
	HWND mhWnd; // Window Instance
	int mTop;
	int mLeft;
	int mPixelLength; // pixel size of each rectangle

	Cell** myCells;
	bool _CellInitialized;
	double mDifficultyPercentage;
	
	bool mbLost;
	bool mbWon;
};