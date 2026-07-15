#include "Unit.h"
#include "Object3D.h"
#include "ObjectManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Player3D.h"
#include "Master.h"
#include "Effect.h"


Unit::Unit(VECTOR initPos, Status* status)
	:Object3D(initPos)
	, mpStatus(status)
{

}

Unit::~Unit()
{
	// 自分でnewしてないからdeleteしない
}

void Unit::Heal()
{
	mpStatus->ApplyHeal();
}

void Unit::Damage(float damage, bool isPlayerAttack, bool isHoming)
{
	// nullptrまたは削除フラグが立っているまたはステータスがないならreturnする
	if (this == nullptr || IsDeleteFlag() || mpStatus == nullptr) return;

	mpStatus->ApplyDamage(damage); // ダメージを与える

	// 状態によって 出すエフェクトを追加
	// 死んでいるならエフェクトDEATH
	if (mpStatus->GetBaseHp() <= 0.0f)
	{
		new Effect(this->GetPosition(), "Resource/2D/Damage2.png", Effect::EffectType::TYPE_DEATH);

		SetDeleteFlag(true);
	}
	else // ダメージならDAMGEエフェクト
	{
		new Effect(this->GetPosition(), "Resource/2D/Damage.png", Effect::EffectType::TYPE_DAMAGE);
	}
}

// スタミナが上限を超えないように
void Unit::LimitSpeed()
{
	mpStatus->LimitSpeed();
}


