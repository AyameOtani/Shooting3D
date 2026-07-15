#pragma once
#include "BaseBullet3D.h"
#include <string>
#include <vector>

class Unit;

class HomingBullet3D : public BaseBullet3D
{
public:

	HomingBullet3D(
		Object3D::Tag3D shotManTag, // 誰が撃ったかのタグ
		Unit* owner,                // 誰が打ったか特定  追加
		Object3D* target,           // ホーミングするターゲット
		VECTOR initPos,             // 位置
		std::string filename,       // モデル名前　弾のやつ
		VECTOR Direction,           // 角度　ホーミングのため
		float attack                // 攻撃力をいれるため
	);

	~HomingBullet3D();

	void Update() override;

	void CalcDirection(); // ホーミング

private:
	float mfAngle = 0.0f; // 目標角度
	float mfMoveSpeed = 0.0f; // 動いた量
	float mfSpeed = 15.0f; // 速さ


private:
	Object3D* mpTarget = nullptr;   // ターゲット
	Object3D::Tag3D mtShotMon;      // 誰が撃ったか
	Unit* mpOwner;                  // 撃った人のポインタ

};