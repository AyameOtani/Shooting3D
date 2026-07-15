#pragma once

#include "Object3D.h"

class Model;
class Unit;

// 弾の基底クラス作った
class BaseBullet3D : public Object3D
{
public:
	BaseBullet3D(std::string filename, VECTOR initPos, VECTOR Direction, Unit* owner, float attack = 10.0f);
	virtual ~BaseBullet3D();

	void Update();
	void Draw();
	void Move();
	void CollDamage(Object3D* pTarget); // Unitのやつを呼び出す

	// 速さのゲッターセッター
	float GetSpeed() const { return mfSpeed; }
	void SetSpeed(float sp) { mfSpeed = sp; }



	// あたり判定のゲッターセッター
	// 弾用
	float GetHitBulletSize() { return mfHitBulletSize; }
	void SetHitBulletSize(float bsz) { mfHitBulletSize = bsz; }
	


public:
	// 方向を取得・設定する
	VECTOR GetDirection() const { return mvDirection; }
	void SetDirection(VECTOR dir) { mvDirection = dir; }
	// モデルを取得する
	Model* GetModel() { return mpModel; }


public:
	void SetTargetTag(Object3D::Tag3D tag) { mTargetTag = tag; }
	virtual bool IsPlayerBullet() const { return false; }
	virtual bool IsHomingBullet() const { return false; }
	virtual void Hit(); // 共通の当たり判定
	Object3D::Tag3D mTargetTag; // 当たり判定の対象タグ

private:
	Model* mpModel;				// ポインタ
	VECTOR mvDirection;			// 向き
	float mfAngle = 0.0f;		// 角度
	float mfMoveSpeed = 0.0f;	// 動く速さ
	float mfSpeed = 50.0;		// 速さ
	float mfTime = 0.0f;		// 生存時間
	Unit* mpOwner;               // 誰が撃ったかをポインタで持つ

	float mfAttack = 10.0f; // デフォルト攻撃力


private: //当たり判定
	float mfHitBulletSize = 10.0f;  // 弾が当たる半径 弾用

private:
	VECTOR mvOldPosition;
	const float MOVE_DISTANCE = 3000.0f; // 消える距離
	const float GRAVITY_DIST = 10.0f; // 重力をかけ始める距離
	const float GRAVITY = 0.001f; // 重力
};