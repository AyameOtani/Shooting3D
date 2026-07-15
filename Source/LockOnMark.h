#pragma once
#include <DxLib.h>
#include <vector>

class Player3D;
class LockOnMark
{
public:
	LockOnMark(Player3D* player);
	~LockOnMark();


	void InitializeMark(); // プレイヤーの初期化で呼び出してScene3Dでリセット
	void UpdateMark();
	void DrawMark();
	void ResetMark(); // リセット


	// 扇形を法線上に書く円形ゲージ関数
	void DrawRadialGauge(
		float cx,			  // X
		float cy,			  // Y
		float r,			  // 半径
		int filledSeg,		  // 今埋まっている枚数
		float partial,		  // 一枚の埋まり具合
		int segments,		  // 最大数
		unsigned int colFill, // ゲージ色
		unsigned int colBack, // 背景色
		float startDeg        // ゲージ始まりの角度
	);

	// 2D版の代わりに、3D版を定義する
	void DrawRadialGauge3D(VECTOR centerPos,
		float r,
		int filledSeg,
		float partial,
		int segments,
		unsigned int colFill,
		float startDeg
	);


private:
	// 受取ったdを度数からラジアンに変更する
	static inline float DegToRad(float d) { return d * (float)DX_PI / 180.0f; }


private:
	Player3D* mpPlayer;


	// 360を分ける処理
	float stepDeg = 0.0f;
	// 時間にするため
	const int SEGMENTS = 12;  // 三角形の枚数   300でもまだ線が分かるから調整必要
	float TotalTime = 180.0f;   // 時間秒数 三分ぐらいにしたいから180秒ぐらい
	int filledSeg = 0;			// 分ける数　減らすやつ
	float partial = 0.0f;

	// 時間
	int prevTime = 0;
	int mnFontBig = 0; // おおきく表示するため
	int mnRemainTime = 0; // 残り秒数のint版

	int mnChengeCount = -1;  // 前フレームのカウント
};