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
		float homingAttack;    // ホーミング弾の攻撃力
		
		SkillType skillType;   // スキルの種類
		std::string skillmemo; // スキル説明テキスト
	};


	// ステータスを返す関数
	static StatusData GetPlayerSpec(int player); // プレイヤーのステータスを返すやつ
	static StatusData GetEnemySpec(int enemy); // 敵のステータス返すやつ

	// コンストラクタとデストラクタ
	Status(const StatusData& data);
	~Status();

	// ゲッターセッター関係 Playerから取得するため
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

	// モデルの名前の取得
	const std::string& GetModelName() const { return msModelName; }

	// IDの取得
	int GetID() const { return mnNumber; }
	// 名前の取得
	const std::string& GetBaseName() const { return msName; }

	// スキル種類のゲッター
	SkillType GetSkillType() const { return mSkillType; }
	// スキル名前
	const std::string& GetBaseSkillMemo() const { return msSkillmemo; }


	// 処理の関数
	void ApplyHeal(); // 回復するための関数
	void LimitSpeed(); // 最大速度を超えないようにするための関数
	void ApplyDamage(float applyDamage); // ダメージをさせる関数


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
	// モデルの名前の保存メンバ
	std::string msModelName;
	// スキル説明
	std::string msSkillmemo;

	// スキル種類
	SkillType  mSkillType = SkillType::SKILL_NONE;
};