#pragma once
#include "DxLib.h"
#include <vector>

class Object3D;
class Enemy3D;

class Camera
{
public:
	Camera();
	~Camera();

	void Initialize();      // 初期化
	void Update();          // 更新
	void UpdateRotation(); // 回転処理

	// 位置関係
	VECTOR GetPosition() { return mvPosition; } // 座標の取得
	VECTOR GetLookAtPosition() { return mvLookAtPosition; }// 注視点取得
	void SetPosition(VECTOR pos) { mvPosition = pos; } // 座標の設定
	void SetLockAtPosition(VECTOR look) { mvLookAtPosition = look; } // 注視点の設定

    // 画面揺れ
	void Shake();
	void SetupShake(float time, float width, float angleSpeed, float stepTime = 1.0f);
	VECTOR GetShakePosition() { return mvShakePosition; } // 揺れのゲッター

	void LookRange(); // 視野範囲のやつ
	// リセット
	void Reset();

	// 背景の色を変更するやつ 0408追加
	void ChangeBackColor(int r, int g, int b, int alpha = 0);
	// デフォルトに戻すやつ
	void DefaultBackColor();

	// カメラ操作の有効と無効を切り替える
	void SetEnableControl(bool enable) { mbEnableControl = enable; }

	VECTOR GetRightVector();
	VECTOR GetUpVector();

private:
	float mfHorizontalAngle;  // 水平方向アングル  ここでもカメラの座標が変わる（縦座標はそのまま）
	float mfVerticalAngle;    // 垂直方向アングル  ここでもカメラの座標が変わる（横方向はそのまま）

	VECTOR mvPosition;        // カメラ座標
	VECTOR mvLookAtPosition;  // カメラの注視点座標
	Object3D* mpTarget;		  // カメラを向ける対象


	// 画面振れ
	int mnShakeTime;
	int mnShakeTimeCount;
	float mfShakeAngle;
	float mfShakeTimeCounter;
	float mfShakeTime;
	float mfShakeWidth;
	float mfShakeAngleSpeed;
	float mfStepTime;
	VECTOR mvShakePosition;


	float mfCurrentSpeedH = 0.0f; // 現在の水平回転速度
	float mfCurrentSpeedV = 0.0f; // 現在の垂直回転速度
	const float MAX_SPEED = 5.0f;     // 最大速度
	const float ADD_SPEED = 0.32f;   // 加速度
	const float SUB_SPEED = 0.32f;  // 減速速度


private:
	float mfTargetHorizontalAngle = 0.0f; // 目標の水平角度
	float mfTargetVerticalAngle = 11.0f;  // 目標の垂直角度
	float mfTargetDistance = 210.0f;      // 目標の距離
	float mfCurrentDistance = 210.0f;     // 現在の距離（じわじわ変化させる用）

	bool mbIsFixedMode = false;      // 固定モードフラグ
	bool mbIsInterpolating = false; // ★これ追加：スライド移動中かどうか
	bool mbEnableControl = true;    // カメラ操作有効フラグ

private:
	// 背景色の変更　メンバにした
	int mnR = 0;
	int mnG = 30;
	int mnB = 50;

};



//------------------------------------------//
//
//        
//
//           カメラ
//- - - - ( VECTOR mvPosition ) - - - - - - -水平方向アングル (点線の軌道)
//
//
//                     物体
//        ( VECTOR mvLookAtPosition )
// 
//------------------------------------------//
//
//                    カメラ
//            ( VECTOR mvPosition )
//                      |
//                                    垂直方向アングル (点線の軌道)
//                      |
//
//                     物体
//        ( VECTOR mvLookAtPosition )
//------------------------------------------//
//
//            カメラ
//     ( VECTOR mvPosition )
//
//
//
//
//                     物体
//        ( VECTOR mvLookAtPosition )

