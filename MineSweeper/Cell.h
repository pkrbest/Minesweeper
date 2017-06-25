#pragma once
class Cell
{
public:
	enum State{EXPOSED, NOT_EXPOSED, PROCESSING};

	Cell();

	void ExposeCell(){ mCellState = EXPOSED; }
	bool IsExposed() { return mCellState == EXPOSED; }
	bool IsNotProcessing(){ return mCellState != PROCESSING; }
	void StartProcessing(){ mCellState = PROCESSING; }
	void AddBee() { mbHasBee = true; }
	void Draw(HDC hdc, int aLeft, int aTop, int aRight, int aBottom, bool abLost);
	void DrawBee(HDC hdc, int aLeft, int aTop, int aRight, int aBottom);
	void WriteText(HDC hdc, int aLeft, int aTop, int aRight, int aBottom);
	State CurrState(){ return mCellState; }
	int& Neighbors() { return mNeighbors; }
	bool HasNonBeeNeighbor(){ return (!mbHasBee && IsNotProcessing() && !IsExposed()); }
	bool HasBee(){ return mbHasBee; }

private:
	State mCellState;
	bool mbHasBee;
	int mNeighbors;
};

