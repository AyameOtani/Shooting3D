#pragma once

#include <DxLib.h>
#include "Status.h"

class Player3D;
class Skill
{
public:
	Skill(Status::SkillType type);
	~Skill();


	void Execute(Player3D* player);
	void MainSkill(Player3D* player); // スキルクールタイムアップデート
	void SkillDebugDraw(); // デバッグ用


	// ゲッターセッター
	bool GetSkillFlag() const { return mbSkill; } // スキル中華の取得
	bool GetAttackSkill() const { return mbAttckSkill; } // 攻撃スキル中か

	float GetSkillTime() const { return mfSkillTime / 60; }



private: // スキル演出関係
	bool mbAttckSkill = false; // 攻撃スキルの時に自動で発射するやつ



private:
	float mfSkillCool = 0.0f;	 // スキルのクールタイム
	float mfMaxCool = 5.0f;		// クールタイム 10秒
	bool mbSkill = false;		 // スキル発動中か
	float mfSkillTime = 0.0f;	 // スキル時間
	float mfMaxSkillTime = 300.0f; // スキルの最大時間  5秒

	Status::SkillType mSkillType;


	// キャラごとのスキルの関数
	void AttackSkill(Player3D* player);  // 攻撃力アップ大弾 ＋ 自動発射 3 秒間
	void GodTimeSkill(Player3D* player); // バリア + 敵弾反射
	
};
