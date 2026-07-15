#include "Model.h"
#include "Master.h"
#include "SceneManager.h"
#include "ObjectManager.h"
#include "Scene.h"
#include "Enemy3D.h"
#include "Stage.h"
#include "Player3D.h"
#include "HomingBullet3D.h"
#include "BaseBullet3D.h"

HomingBullet3D::HomingBullet3D(
	Object3D::Tag3D shotManTag, // 誰が撃ったかのタグ
	Unit* owner,                // 誰が打ったか特定  追加
	Object3D* target,           // ホーミングするターゲット
	VECTOR initPos,             // 位置
	std::string filename,       // モデル名前　弾のやつ
	VECTOR Direction,           // 角度　ホーミングのため
	float attack                // 攻撃力をいれるため
)
	: BaseBullet3D(filename, initPos, Direction, owner, attack)
	, mtShotMon(shotManTag) // 誰が撃ったかの代入
	, mpOwner(owner) // 誰が撃ったかの代入
	, mpTarget(target) // ターゲット代入

{

	SetTag(Object3D::T_HomingBullet3D);

	if (mpTarget != nullptr)
	{
		// 狙っている相手（敵か岩か）のタグをそのまま自分の当たり判定対象にする！
		mTargetTag = mpTarget->GetTag();
	}
	else
	{
		// ターゲットがいない（正面に飛ばすだけ）ならデフォルトで敵を狙う
		mTargetTag = Object3D::T_Enemy3D;
	}

	// 初期の弾の向き
	float angleY = atan2(GetDirection().x, GetDirection().z); // XZ平面上で弾がどっちを向いているか
	GetModel()->SetRotation(VGet(0.0f, angleY + DX_PI / 2.0f, 0.0f));

	SetSpeed(mfSpeed); // 速さ
}

HomingBullet3D::~HomingBullet3D()
{

}


void HomingBullet3D::Update()
{
	CalcDirection(); // 追跡するやつ

	//  Move() -> Hit() -> モデル位置更新
	BaseBullet3D::Update();

}

// カメラがロックオンかは敵とかプレイヤーで見る
void HomingBullet3D::CalcDirection()
{
	// ホーミングじゃない　かつターゲットがnullだったら弾を消して抜ける
	if (!mpTarget)
	{
		SetDeleteFlag(true);
		return;
	}

	// 削除済みチェック
	if (mpTarget->IsDeleteFlag())
	{
		mpTarget = nullptr;
		SetDeleteFlag(true);
		return;
	}

	VECTOR targetPos = mpTarget->GetPosition();
	targetPos.y += 20; // 胴体らへんを狙うように
	VECTOR targetAngle = VSub(targetPos, GetPosition());

	// 向き更新
	SetDirection(VNorm(targetAngle));

	// 弾の向きを合わせる
	float angleY = atan2f(GetDirection().x, GetDirection().z);
	GetModel()->SetRotation(VGet(0.0f, angleY + DX_PI_F / 2.0f, 0.0f));
}
