#pragma once
#include <DxLib.h>
#include "Scene.h"
#include <memory>

class Model; // 前方宣言

class SelectScene : public Scene
{
public:
	// コンストラクタ　デストラクタ
	SelectScene();
	~SelectScene();

	// sceneから引き継いだ者たち
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;

	void Draw2D(); // テキストの描画とか2D系

private:

	bool mbSelect = false; // 選択されたか
	int mnSelectNum = 1; // 選択された数字の保存
	int mnSelectMax = 3; // 選択の最大

	// フォント
	int mnFontNormal;

	// 画像関係
	int mnHandle_Select;   // 選択の枠の画像
	int mnHandle_NoSelect; // 選択してない枠の画像

	std::unique_ptr<Model> mpModel; // 表示用モデル

private:
	float mTimer; // 演出用タイマー
	const float MODEL_Y_POSITION = -1000.0f; // モデルのYの位置
	const float MODEL_TARGET_POS = 60.0f; // モデルの目標位置

	float mModelPosY = MODEL_Y_POSITION; // モデルのY座標
	float mfModelSpeed; // モデルを近づける速度

	// 画面演出用
	float mfGraphAlpha; // 青BOX透過度
	bool mbDirection; // 演出をONにするか

};