#include "Rock3D.h"
#include "Scene.h"
#include "ObjectManager.h"
#include "Master.h"
#include "Player3D.h"
#include "Bullet3D.h"



Rock3D::Rock3D(Model* rockModel, Model* breakModel, VECTOR initPos)
	: Unit(initPos)
	, mvDirection(VGet(0.0f, 0.0f, 0.0f))
	, mvOldPosition(VGet(0.0f, 0.0f, 0.0f))
	, mfAngle(0.0f)
{
	SetTag(Object3D::T_Rock3D); // tag

	// モデル作成
	mpModel = new Model(MV1DuplicateModel(rockModel->GetHandle()), initPos); // モデル代入
	// 割れえる岩
	mBreak.Load(breakModel->GetHandle());


	// 速さランダム
	int speed = GetRand(10) + 7; // 7から17
	mfSpeed = (float)speed;

	//　回転をランダムにさせるやつ
	mvAxis = VGet(
		(float)GetRand(200) - 100.0f, // -100から100
		(float)GetRand(200) - 100.0f,
		(float)GetRand(200) - 100.0f
	);
	mvAxis = VNorm(mvAxis); // 正規化


	// ロックオン画像
	mLockBillboardHandle = LoadGraph("Resource/2D/RockOn.png");
	if (mLockBillboardHandle == -1)
	{
		printfDx("ロックオン画像ない");
	}


	prevTime = GetNowCount();
}



bool Rock3D::Initialize()
{
	// バリアの表示位置を設定
	mBreak.SetPosition(mvPosition);
	// バリアの大きさ（倍率）
	mBreak.SetScale(1.0f);
	// バリア開始時は必ず元の状態
	mBreak.Reset();
	mBreak.SetPosition(mvPosition);

	return true;
}

Rock3D::~Rock3D()
{

	DeleteGraph(mLockBillboardHandle);

	 delete mpModel;

	mBreak.Finalize();
}


void Rock3D::Update()
{
	Move();
	Size();
	ToPlayerHit();
	ToBulletHit();

	const float dt = FIXED_DT;
	mBreak.Update(dt, mfSpeed);

	//  割れていない時だけ追従させる
	if (!mBreak.IsBroken())
	{
		mBreak.SetPosition(mvPosition);
	}


	// 0.001から0.008のランダム
	mfSize += 0.001f + ((float)GetRand(10000) / 10000.0f) * 0.008f;
	if (mfSize >= 1.2f)
	{
		mfSize = 1.2f;
	}

	// Z-1000だったら消える
	if (mvPosition.z <= -1000)
	{
		SetDeleteFlag(true);
	}

	mpModel->Update();
}

void Rock3D::Draw()
{
	////  割れていない時だけカプセル描画
	//if (!mBreak.IsBroken())
	//{
	//	// 当たり判定用
	//	DrawSphere3D(
	//		mvPosition,
	//		mfHitSize,
	//		8,
	//		GetColor(255, 255, 255),
	//		GetColor(255, 255, 255),
	//		FALSE
	//	);
	//}

	
	// 割れてないなら岩を描画
	if (!mBreak.IsBroken())
	{
		mpModel->Draw(); // 描画する
	}
	else
	{
		// 割れた後        破片モデルを描画
		mBreak.Draw();
	}

	// ロックオンされていたら
	if (IsLocked())
	{
		if (mBreak.IsBroken())	  return; // 割れてたらスルー

		// ロックマーク描画
		SetUseZBuffer3D(FALSE);

		DrawBillboard3D(
			VGet(GetPosition().x, GetPosition().y + 0.0f, GetPosition().z),
			0.5f, 0.5f,
			180.0f,
			0.0f,
			mLockBillboardHandle,
			TRUE        // 透過あり
		);
		SetUseZBuffer3D(TRUE); // 元に戻す
		DrawString(0, 0, "LOCK ON", GetColor(0, 255, 0));
	}


}

void Rock3D::Move()
{
	mvPosition = VAdd(mvPosition, VScale(mvDirection, mfSpeed));

	// ずらしている
	mvPosition.z -= mfSpeed;
	mpModel->SetPosition(mvPosition);

	// 回転
	int now = GetNowCount();
	float dt = (now - prevTime) / 1000.0f;
	prevTime = now;
	mfAngle += ROTATE_SEED * dt;


	// 軸と角度をかける
	// 方向 * 速さ = 移動量
	mvRotation = VScale(mvAxis, mfAngle);
	mpModel->SetRotation(mvRotation);
}

void Rock3D::Size()
{
	mpModel->SetScale(mfSize);
}

void Rock3D::ToPlayerHit()
{
	if (mBreak.IsBroken())	  return; // 割れてたらスルー

	auto pPlayerList = Master::mpSceneManager->GetCurrentScene()->GetObjectManager()
		->GetObject3DListByTag(Object3D::T_Player3D);

	for (auto player : pPlayerList)
	{
		Player3D* pPlayer = dynamic_cast<Player3D*>(player);

		bool isHit = HitCheck_Sphere_Sphere(
			mvPosition,
			mfHitSize,
			pPlayer->GetPosition(),
			pPlayer->GetHitSize()
		);

		// 当たったかつ割れてないなら割る
		if (isHit && !mBreak.IsBroken())
		{
			// 無敵の時も割りたい
			mBreak.Break(mvPosition, ROCK_BREAK_POWER);

			// 無敵だったらスルー
			if (pPlayer->GetGodFlag()) return;

			// プレイヤーの攻撃かのフラグはfalse 岩の攻撃だから
			// ホーミングじゃないからfalse
			pPlayer->Damage(5.0f, false, false);    // ダメージ処理
		}
	}
}

void Rock3D::ToBulletHit()
{
	if (mBreak.IsBroken())	  return; // 割れてたらスルー

	// プレイヤーの弾とる
	auto pPlayerBullet = Master::mpSceneManager->GetCurrentScene()->GetObjectManager()
		->GetObject3DListByTag(Object3D::T_PlayerBullet3D);

	for (auto bullet : pPlayerBullet)
	{
		Bullet3D* pPlayerBullet = dynamic_cast<Bullet3D*>(bullet);
		if (!pPlayerBullet) continue;

		bool isHit = HitCheck_Sphere_Sphere(
			mvPosition,
			mfHitSize,
			pPlayerBullet->GetPosition(),
			pPlayerBullet->GetHitBulletSize()
		);

		if (isHit)
		{
			pPlayerBullet->SetDeleteFlag(true);
			mBreak.Break(mvPosition, ROCK_BREAK_POWER);
		}
	}
}

void Rock3D::Damage(float damage, bool isPlayerAttack, bool isHoming)
{
	// すでに割れていたら何もしない
	if (mBreak.IsBroken()) return;

	// 岩を割る処理を実行
	mBreak.Break(mvPosition, ROCK_BREAK_POWER);
}