#include"Status.h"
#include <string>


Status::Status(const StatusData& data)
	: msName(data.name)
	, mfMaxHp(data.hp)
	, mfHp(data.hp)
	, mfMaxSpeed(data.maxSpeed)
	, mfSpeed(data.speed)
	, mfNormalAttack(data.normalAttack)
	, mfHomingAttack(data.homingAttack)
	, mnNumber(data.num)
	, msModelName(data.modelName)
	, mSkillType(data.skillType)
	, msSkillmemo(data.skillmemo)
{

}

Status::~Status()
{
	 
}

void Status::ApplyDamage(float applyDamage)
{
	mfHp -= applyDamage;
	if (mfHp <= 0.0f)
	{
		mfHp = 0.0f;
	}
}

void Status::ApplyHeal()
{
	// 最大HPの3分の2回復
	float heal = mfMaxHp * (2.0f / 3.0f);
	mfHp += heal;

	if (mfHp > mfMaxHp)
	{
		mfHp = mfMaxHp;
	}
	// DrawQuadrangle
}


void Status::LimitSpeed()
{
	// 最大を超えないように
	if (mfSpeed >= mfMaxSpeed)
	{
		mfSpeed = mfMaxSpeed;
	}
}

Status::StatusData Status::GetPlayerSpec(int player)
{
	switch (player)
	{
	case 1:
		return
		{
			1,
			"赤初号機",	 // 機体名前
			"Resource/3D/Player/Red.mqo",
			50.0f,		 // HP
			5.0f,		 // SPEED
			15.0f,		 // MAXSPEED
			10.0f,		 // Attack
			30.0f,		 // HomingAttack
			SkillType::SKILL_ATTACK, // 持っているスキル
			"自動攻撃" // スキル効果
		};

	case 2:
		return
		{
			2,
			"青二号機",	   // 機体名前
			"Resource/3D/Player/Blue.mqo",
			20.0f,		   // HP
			3.0f,		   // SPEED
			10.0f,		   // MAXSPEED
			10.0f,		   // Attack
			20.0f,		   // HomingAttack
			SkillType::SKILL_HEAL,
			"HPを回復" // スキル効果
		};

	case 3:
		return
		{
			2,
			"黄色三号機",	   // 機体名前
			"Resource/3D/Player/Yellow.mqo",
			100.0f,		   // HP
			8.0f,		   // SPEED
			16.0f,		   // MAXSPEED
			50.0f,		   // Attack
			50.0f,		   // HomingAttack
			SkillType::SKILL_GOD,
			"無敵状態" // スキル効果
		};

	default:
		return { 9999,"例外", "例外",1.0f, 1.0f, 1.0f,  10.0f, 20.0f, SkillType::SKILL_HEAL, "なし"};
	}
}


Status::StatusData Status::GetEnemySpec(int enemy)
{
	switch (enemy)
	{
	case 1: // 敵1
		return
		{
			1,
			"雑魚1",  // 機体名前
			"Resource/3D/Player/Red.mqo",
			30.0f,	  // HP
			5.0f,	  // SPEED
			15.0f,	  // MAXSPEED
			10.0f,	  // Attack
			20.0f,	  // HomingAttack
			SkillType::SKILL_NONE,
			"なし" // スキル効果
		};

	case 2: // 敵2
		return
		{
			2,
			"雑魚2",	  // 機体名前
			"Resource/3D/Player/Red.mqo",
			50.0f,		  // HP
			7.0f,		  // SPEED
			10.0f,		  // MAXSPEED
			10.0f,		  // Attack
			20.0f,		  // HomingAttack
			SkillType::SKILL_NONE,
			"なし" // スキル効果
		};

	case 3: // 敵3
		return
		{	
			3,
			"雑魚3",	  // 機体名前
			"Resource/3D/Player/Red.mqo",
			20.0f,		  // HP
			3.0f,		  // SPEED
			10.0f,		  // MAXSPEED
			10.0f,		  // Attack
			20.0f,		  // HomingAttack
			SkillType::SKILL_NONE,
			"なし" // スキル効果
		};


	default: // 例外
		return { 9999,"未定義エネミー", "例外", 1.0f, 1.0f, 1.0f,  10.0f, 20.0f, SkillType::SKILL_NONE, "なし"};
	}
}

