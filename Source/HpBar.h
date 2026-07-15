#pragma once
#include <DxLib.h>
#include <vector>

class Player3D;

class HpBar
{
public:
	HpBar(Player3D* player); // Player3Dの変数をとりたい
	~HpBar();

	void UpdateBar();
	void DrawBar();

private:
	Player3D* mpPlayer; // プレイヤークラスのポインタ

	float lateHp; // 遅延HPバーのため
	float lateDist = 0.02f; // 遅延させるfloat
};
