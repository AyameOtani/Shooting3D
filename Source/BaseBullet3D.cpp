#include "BaseBullet3D.h"
#include "Model.h"
#include "Master.h"
#include "Enemy3D.h"
#include "Scene.h"
#include "ObjectManager.h"
#include "Unit.h"
#include "Player3D.h"

BaseBullet3D::BaseBullet3D(std::string filename, VECTOR initPos, VECTOR Direction, Unit* owner, float attack)
	:Object3D(initPos)
	, mvDirection(VNorm(Direction))
	, mvOldPosition(VGet(0.0f, 0.0f, 0.0f))
	, mpOwner(owner) // 誰が撃ったかの代入
{
	mpModel = new Model(filename, initPos);
	mfAttack = attack;
}

BaseBullet3D::~BaseBullet3D()
{
	delete mpModel;
}

void BaseBullet3D::Update()
{
	mvOldPosition = mvPosition;
	mfTime++;

	Move();
	Hit(); // あたり判定呼ぶ

	//// 重力みたいにしている
	//if (mfTime >= GRAVITY_DIST)
	//{
	//	mvDirection.y -= GRAVITY;
	//}

	mpModel->SetPosition(mvPosition);
	mpModel->SetRotation(mvRotation);
	mpModel->Update();
}

void BaseBullet3D::Draw()
{
	//// 当たり判定用
	//DrawSphere3D(
	//	mvPosition,
	//	GetHitBulletSize(),
	//	8,
	//	GetColor(255, 255, 255),
	//	GetColor(255, 255, 255),
	//	FALSE
	//);

	mpModel->Draw();
}

void BaseBullet3D::Move()
{
	mvPosition = VAdd(mvPosition, VScale(mvDirection, mfSpeed));

	// 移動距離を計算
	mfMoveSpeed += mfSpeed;
	if (mfMoveSpeed >= MOVE_DISTANCE)
	{
		SetDeleteFlag(true);
	}

	mpModel->SetPosition(mvPosition);
}


// 弾がぶつかった相手をpTarget代入運動
void BaseBullet3D::CollDamage(Object3D* pTarget)
{
	// 当たった敵がUnitを継承していたら
	Unit* pTargetObj = dynamic_cast<Unit*>(pTarget);
	if (!pTargetObj) return;


	// 相手がプレイヤー型かキャストを試みる
	Player3D* pPlayer = dynamic_cast<Player3D*>(pTargetObj);
	if (pPlayer != nullptr)
	{
		// プレイヤーであり かつ 無敵中ならダメージ処理をスキップ
		if (pPlayer->GetGodFlag())
		{
			return;
		}
	}


	if (pTargetObj)
	{
		bool isPlayer = IsPlayerBullet();
		bool isHoming = IsHomingBullet();

		pTargetObj->Damage(mfAttack, isPlayer, isHoming);
	}
}



void BaseBullet3D::Hit()
{
	// 指定されたタグリストを取得
	auto pTargetList = Master::mpSceneManager->GetCurrentScene()
		->GetObjectManager()->GetObject3DListByTag(mTargetTag);

	for (auto obj : pTargetList)
	{
		// 判定相手 Unitを継承しているはず
		Unit* pTargetUnit = dynamic_cast<Unit*>(obj);
		if (!pTargetUnit) continue;

		// 共通の球体判定
		bool isHit = HitCheck_Sphere_Sphere(
			mvPosition, GetHitBulletSize(),
			pTargetUnit->GetPosition(), pTargetUnit->GetHitSize()
		);

		if (isHit)
		{
			CollDamage(obj);    // ダメージ処理
			SetDeleteFlag(true); // 弾消去
			break;
		}
	}
}