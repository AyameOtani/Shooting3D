#pragma once

#include "DxLib.h"
#include <string>
#include "Unit.h"
#include "Model.h"
#include "BreakRock.h"
#include "Status.h"


class Rock3D : public Unit
{

	// 割れた岩の奴
private:
	BreakRock mBreak;			// 割れる岩
	bool mRockAlive = false;	// まだ割れていないか

	// 岩が割れるときの「飛び散る強さ」
	// 数値が大きいほど破片が勢いよく飛ぶ
	const float  ROCK_BREAK_POWER = 600.0f;
	const float  FIXED_DT = 1.0f / 60.0f;


private:
	static constexpr float ROTATE_SEED = 1.2f; // 回転スピード
	
public:
	Rock3D(Model* rockModel, Model* breakModel,  VECTOR initPos);
	~Rock3D();

	bool Initialize(); /// 初期化
	void Update();
	void Draw();

	void Move(); // 動きと回転
	void Size(); // 大きさ

	void ToPlayerHit(); // プレイヤーと当たったら
	void ToBulletHit(); // プレイヤーの弾と当たったら


	// 速さのゲッター
	float GetSpeed() const { return mfSpeed; }

	// 割れているかのやつ
	bool IsBrokenFlag() const { return mBreak.IsBroken(); }

	// 上書き
	//void Damage(float damage, Unit* pOwner = nullptr) override;
	void Damage(float damage, bool isPlayerAttack = false, bool isHoming = false) override;


private:
	Model* mpModel;				// モデルポインタ
	VECTOR mvOldPosition;		// 古い位置
	VECTOR mvDirection;			// 向き
	float mfSpeed;				// 速さ ランダムにしたい

	static Model* s_sharedModel; // 静的に1つだけ保持


	// 回転
	float mfAngle; // 回転させたい
	int prevTime = 0; // 前の時間
	VECTOR mvAxis; // 回転ランダム

	// 大きさ
	float mfSize = 0.0f; // 大きさ
	float mfHitSize = 80.0f; // 当たり大きさ

private: // ロックオンされたか
	int mLockBillboardHandle; // ロックオン用画像のハンドル

};