#include "Skill.h"
#include "Player3D.h"

Skill::Skill(Status::SkillType type)
	: mSkillType(type)
{

}

Skill::~Skill()
{

}

void Skill::SkillDebugDraw()
{
	if (mbSkill)
	{
		DrawFormatString(600, 600, GetColor(255, 0, 0), "発動中  %.1f", mfSkillTime);
	}
	else
	{
		DrawFormatString(600, 600, GetColor(255, 0, 0), "スキルCT  %.1f", mfSkillCool);
	}
}

void Skill::MainSkill(Player3D* player)
{
	// 発動中でない場合
	if (!mbSkill)
	{
		// 減らす
		if (mfSkillCool > 0.0f)
		{
			mfSkillCool -= 1.0f;
		}

		return;
	}

	// スキル時間減らす
	mfSkillTime -= 1.0f;

	// もし時間が0以上なら
	if (mfSkillTime > 0.0f)
	{
		// 発動中処理
		switch (mSkillType)
		{	 
			// 自動攻撃攻撃力アップ
		case Status::SkillType::SKILL_ATTACK:
			player->SetAddAttack(10.0f);
			mbAttckSkill = true;
			break;

			// 無敵にする
		case Status::SkillType::SKILL_GOD:
			player->SetGodFalg(true);
			break;

		}
	}
	else
	{
		// 終了した時のリセット処理
		switch (mSkillType)
		{
		case Status::SkillType::SKILL_ATTACK:
			player->SetAddAttack(0.0f);
			mbAttckSkill = false;
			break;

		case Status::SkillType::SKILL_GOD:
			player->SetGodFalg(false);
			break;

		}

		mbSkill = false;
		mfSkillCool = mfMaxCool;
	}
}


// スキル発動するやつ
void Skill::Execute(Player3D* player)
{
	// スキル中 またはクールタイム中は発動不可
	if (mbSkill || mfSkillCool > 0.0f) return;

	mbSkill = true; // スキル発動
	mfSkillTime = mfMaxSkillTime; // スキル時間セット


	// 単発系スキルはこっちでやってる
	if (mSkillType == Status::SkillType::SKILL_HEAL)
	{
		player->Heal();
	}
}

void Skill::AttackSkill(Player3D* player)
{
	// クールタイム中ならRETURN
	if (mfSkillCool > 0.0f) return;

	// スキル時間が残っていたら
	if (mfSkillTime > 0.0f)
	{
		player->SetAddAttack(10.0f);
	}
	else if (mfSkillTime == 0.0f) // スキル時間0なら
	{
		player->SetAddAttack(0.0f);
	}
}

void Skill::GodTimeSkill(Player3D* player)
{

}
