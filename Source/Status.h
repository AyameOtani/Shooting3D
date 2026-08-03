#pragma once

#include <string>

class Status
{
public:

	// 機体によってスキルのタイプの列挙型
	enum class SkillType
	{
		SKILL_NONE,

		SKILL_ATTACK, // 自動攻撃+攻撃力アップ
		SKILL_HEAL,   // HPを最大HPの3/2まで回復
		SKILL_GOD,    // 5秒間無敵

		SKILL_MAX,
	};

	struct StatusData
	{
		int num;               // 番号
		std::string name;      // 機体の名前
		std::string modelName; // 機体のモデルの名前
		float hp;              // HP
		float speed;           // 速さ
		float maxSpeed;        // 最大速度

		float normalAttack;    // 攻撃力
		float homingAttack;    // ホーミング攻撃力
		
		SkillType skillType;   // スキルの種類
		std::string skillmemo; // スキル説明
	};


	// ステータス返すやつ
	static StatusData GetPlayerSpec(int player); // プレイヤーのステータスを返すやつ
	static StatusData GetEnemySpec(int enemy); // 敵のステータス返すやつ

	Status(const StatusData& data);
	~Status();

	// ゲッターセッター
	// HP関係
	float GetBaseMaxHp() const { return mfMaxHp; } // 最大HP
	void SetBaseMaxHp(float mhp) { mfMaxHp = mhp; }
	float GetBaseHp() const { return mfHp; } // HP
	// SPPED関係
	float GetBaseMaxSpeed() const { return mfMaxSpeed; } // 最大SPEED
	void SetBaseMaxSpeed(float msp) { mfMaxSpeed = msp; }
	float GetBaseSpeed() const { return mfSpeed; } // SPEED
	// ATTACK関係
	float GetBaseAttack() const { return mfNormalAttack; } // 通常
	float GetBaseHomingAttack() const { return mfHomingAttack; } // ホーミング
	// FILENAME
	const std::string& GetFileName() const { return msModelName; }
	// ID関係
	int GetID() const { return mnNumber; }
	// NAME関係
	const std::string& GetBaseName() const { return msName; }
	// Hp関係
	void ApplyDamage(float applyDamage); // ダメージ
	void ApplyHeal(); // 回復
	// SPEED関係
	void LimitSpeed(); // 最大速度を超えないように
	// スキル種類のゲッター
	SkillType GetSkillType() const { return mSkillType; }
	// スキル名前
	const std::string& GetBaseSkillMemo() const { return msSkillmemo; }

private:
	// ID
	int mnNumber;
	// HP 
	float mfMaxHp;
	float mfHp;
	// SPEED
	float mfMaxSpeed;
	float mfSpeed;
	// ATTACK
	float mfNormalAttack;
	float mfHomingAttack;
	// NAME
	std::string msName;
	// FILENAME
	std::string msModelName;
	// スキル説明
	std::string msSkillmemo;

	// スキル種類
	SkillType  mSkillType = SkillType::SKILL_NONE;
};