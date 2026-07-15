#pragma once

#include "Object3D.h"
#include <string>
#include "Status.h"


class Object3D;
class Unit : public Object3D
{
public:

	Unit(VECTOR initPos, Status* status = nullptr);
	virtual ~Unit();

	void Heal(); // 回復
	//virtual void Damage(float damage, Unit* atacker = nullptr);	//ダメージ
	virtual void Damage(float damage, bool isPlayerAttack, bool isHoming);	//ダメージ
	void LimitSpeed(); // 速さ制限

	// ロックオン関係
	virtual void SetLocked(bool lock) { mbIsLocked = lock; }
	virtual bool IsLocked() const { return mbIsLocked; }

	// 後からステータスを設定するためのやつ
	void SetStatus(Status* st) { mpStatus = st; }
	Status* GetStatus() { return mpStatus; }

	// ゲッター
	float GetHp() const { return mpStatus->GetBaseHp(); }
	float GetMaxHp() const { return mpStatus->GetBaseMaxHp(); }
	// 速さ
	float GetSpeed() const { return mpStatus->GetBaseSpeed(); }
	float GetMaxSpeed() const { return mpStatus->GetBaseMaxSpeed(); }
	// 攻撃力
	float GetAttack() const { return mpStatus->GetBaseAttack() + mfAddAttack; }
	float GetHomingAttack() const { return mpStatus->GetBaseHomingAttack(); }
	std::string GetName() const { return mpStatus->GetBaseName(); }
	// 足す分の攻撃力
	void SetAddAttack(float add) { mfAddAttack = add; }
	// スキルメモげット
	std::string GetSkillMemo() const { return mpStatus->GetBaseSkillMemo(); }


private:
	float mfAddAttack = 0.0f; // スキルで攻撃力足すやつ

private:
	bool mbIsLocked = false; // ロックオンされているフラグ

public: //当たり判定カプセルの大きさ
	 virtual float GetHitSize() const { return 40.0f; }

private:
	Status* mpStatus = nullptr;// ステータスのポインタ

};

