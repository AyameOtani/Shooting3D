#include "Enemy3D.h"
#include "Model.h"
#include "Master.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "ObjectManager.h"
#include "Wall.h"
#include "Stage.h"
#include "Utility.h"
#include "Object3D.h"
#include <algorithm>
#include "Floor.h"
#include "Bullet3D.h"

// 普通にゲームscene用の奴
Enemy3D::Enemy3D(std::string filename, VECTOR initPos, EnemyType type, int id, bool isSeparateAnim)
	: Unit(initPos, nullptr) // 一旦ステータスなし
	, mvOldPosition(initPos)
	, mvMoveVec(VGet(0.0f, 0.0f, 0.0f))
	, m_Type(type)
	, m_ID(id)
{
	SetTag(Object3D::T_Enemy3D); // タグ

	// ステータス
	Status* newStatus = nullptr;
	switch (type)
	{
	case EnemyType::TYPE_SPEED:
		newStatus = new Status(Status::GetEnemySpec(1));
		break;


	case EnemyType::TYPE_HEAVY:
		newStatus = new Status(Status::GetEnemySpec(2));
		break;


	case EnemyType::TYPE_NORMAL:
		newStatus = new Status(Status::GetEnemySpec(3));
		break;

	default:
		newStatus = new Status(Status::GetEnemySpec(1));
		break;
	}
	SetStatus(newStatus); // ステーつをセット


	// ロックオン画像
	mLockBillboardHandle = LoadGraph("Resource/2D/RockOn.png");
	if (mLockBillboardHandle == -1)
	{
		printfDx("ロックオン画像ない");
	}

	mvTargetPos = VGet(
		GetRand(static_cast<int>(MOVE_MAX * 2.0f)) - MOVE_MAX,
		GetRand(static_cast<int>(MOVE_MAX * 2.0f)) - MOVE_MAX,
		mfPosZ
	);

	// モデルの作成
	mpModel = new Model(filename, initPos, isSeparateAnim);
	// 初期行列の設定
	// ここで設定する名称は、mixamoからダウンロードしたモデルによって変わる可能性が高いので、
	// ModelViewer などで確認すること（おそらく、***:Hips となっているフレームがあればそれ。）
	mpModel->SetupInitializeMatrix("mixamorig:Hips");

	// ラグなったからこっちに移動
	SetFontSize(20); // 文字の大きさ
}


// スライド
// offset...ずらす用 重なんないように
void Enemy3D::SetPattern(SpawnSide side, float offset)
{
	mbIsFade = true; // 登場ON
	mfFadeRatio = 0.0f; // 進歩リセット

	switch (side)
	{
	case SpawnSide::RIGHT: // 右タイプ
		mfStartX = 5000.0f;
		mfTargetX = MOVE_MAX;
		mfStartY = offset;
		mfTargetY = offset;
		break;
	case SpawnSide::LEFT: // 左タイプ
		mfStartX = -5000.0f;
		mfTargetX = -MOVE_MAX;
		mfStartY = offset;
		mfTargetY = offset;
		break;
	case SpawnSide::TOP: // 上タイプ
		mfTargetX = offset;
		mfStartX = offset;
		mfStartY = 5000.0f;
		mfTargetY = MOVE_MAX;
		break;
	}

	mvPosition = VGet(mfStartX, mfStartY, mfPosZ); // 初期位置をセット
}

// アニメーション追加
void Enemy3D::AddAnimation(AnimationState state, std::string filename)
{
	mpModel->AddAnimation(state, filename);
}

Enemy3D::~Enemy3D()
{
	// モデルの削除
	delete mpModel;
}

void Enemy3D::Update()
{
	mvOldPosition = mvPosition;

	// 移動処理
	MoveEx();
	// 回転処理の呼び出し
	RotationByMove();

	// 搭乗中は弾撃たない
	if (!mbIsFade)
	{
		// 弾
		 Shot();
	}

	// モデルの更新
	mpModel->SetPosition(mvPosition);
	mpModel->Update();

	Object3D::Update();
}

void Enemy3D::Draw()
{
	int color = GetColor(255, 255, 255);
	int line_color = GetColor(0, 255, 0); // 可視化用の線の色

	//{
	//	// 4つの頂点の座標を計算
	//	VECTOR topLeft = VGet(-MOVE_MAX, MOVE_MAX, mfPosZ);
	//	VECTOR topRight = VGet(MOVE_MAX, MOVE_MAX, mfPosZ);
	//	VECTOR bottomLeft = VGet(-MOVE_MAX, -MOVE_MAX, mfPosZ);
	//	VECTOR bottomRight = VGet(MOVE_MAX, -MOVE_MAX, mfPosZ);

	//	// 4本の線で四角形を作る
	//	DrawLine3D(topLeft, topRight, line_color);      // 上辺
	//	DrawLine3D(topRight, bottomRight, line_color);  // 右辺
	//	DrawLine3D(bottomRight, bottomLeft, line_color); // 下辺
	//	DrawLine3D(bottomLeft, topLeft, line_color);     // 左辺
	//}


	// 敵情報の取得
	auto pEnemyList = Master::mpSceneManager->GetCurrentScene()->GetObjectManager()
		->GetObject3DListByTag(Object3D::T_Enemy3D);
	if (!pEnemyList.empty())
	{
		for (int i = 0; i < pEnemyList.size(); i++)
		{
			auto pEnemy = pEnemyList[i];
			if (pEnemy != nullptr)
			{

				Enemy3D* pTarget = dynamic_cast<Enemy3D*>(pEnemy);

				int id = pTarget->m_ID;
				DrawFormatString(
					20,
					320 + 20 * id,
					color,
					"敵%d "
					"%s"
					"位置  X_%.0f, Y_%.0f, Z_%.0f "
					"今速_%.0f, HP_%.0f, SP_%.0f",
					id,
					pTarget->GetName().c_str(),
					pTarget->mvPosition.x, pTarget->mvPosition.y, pTarget->mvPosition.z,
					pTarget->mfNowSpeed,
					pTarget->GetHp(),
					pTarget->GetSpeed());

			}

		}
	}

	// 敵カプセル
	// DrawSphere3D(mvPosition, GetHitSize(), 8, color, color, FALSE);

	if (m_Type == EnemyType::TYPE_HEAVY || !mbIsRamdam)
	{
		// ターゲット位置
		// DrawSphere3D(mvTargetPos, 20, 8, GetColor(0, 255, 0), GetColor(255, 0, 0), FALSE);
	}

	// ロックオンされていたら
	if (IsLocked())
	{
		// ロックマーク描画
		SetUseZBuffer3D(FALSE);

		DrawBillboard3D(
			GetPosition(),
			0.5f, 0.5f,
			150.0f,
			0.0f,
			mLockBillboardHandle,
			TRUE        // 透過あり
		);
		SetUseZBuffer3D(TRUE); // 元に戻す
		DrawString(0, 0, "LOCK ON", GetColor(255, 0, 0));
	}


	mpModel->Draw();
}


void Enemy3D::MoveEx()
{
	float speed = GetSpeed(); // はやさ取得

	if (mbIsFade)
	{
		// フェードイン
		mfFadeRatio += mfFadeDist;	// 増やす
		// スタート地点から見てドンくらい進んだか ＋ ( 目標地点までの全距離 ) * 今は何%まで進むか 
		mvPosition.x = mfStartX + (mfTargetX - mfStartX) * mfFadeRatio;
		mvPosition.y = mfStartY + (mfTargetY - mfStartY) * mfFadeRatio;

		if (mfFadeRatio >= 1.0f) // 到達したら
		{
			mvPosition.x = mfTargetX; // 位置更新
			mvPosition.y = mfTargetY; // 位置更新
			mbIsFade = false; // 登場終了フラグを折る
		}
	}
	else if (!mbIsRamdam)
	{
		// 一回だけランダム地点へ移動
		VECTOR toTarget = VSub(mvTargetPos, mvPosition);
		float distance = VSize(toTarget);

		// 近くなったら
		if (distance < 10.0f)
		{
			// 目標についたら準備完了
			mbIsRamdam = true;

			// 方向をリセット
			if (m_Type == EnemyType::TYPE_NORMAL) mvMoveVec = VGet(1.0f, 0.0f, 0.0f); //　真横にだけ動くベクトル
			else if (m_Type == EnemyType::TYPE_SPEED)  mvMoveVec = VGet(0.0f, 1.0f, 0.0f); // 真上下にだけ動くベクトル
			else mvMoveVec = VGet(1.0f, 1.0f, 0.0f); // 適当なとこからいけるように
		}
		else
		{
			// 目標に向かって移動
			mvMoveVec = VNorm(toTarget);
			mvPosition = VAdd(mvPosition, VScale(mvMoveVec, speed));
		}
	}
	else
	{
		float speed = GetSpeed(); // 速さゲット

		switch (m_Type)
		{
		case EnemyType::TYPE_NORMAL: // 左右に移動

			mvPosition.x += mvMoveVec.x * speed;
			if (mvPosition.x > MOVE_MAX) { mvMoveVec.x = -1.0f; }
			if (mvPosition.x < -MOVE_MAX) { mvMoveVec.x = 1.0f; }
			break;


		case EnemyType::TYPE_SPEED: // 上下に移動

			mvPosition.y += mvMoveVec.y * speed;
			if (mvPosition.y > MOVE_MAX) { mvMoveVec.y = -1.0f; }
			if (mvPosition.y < -MOVE_MAX) { mvMoveVec.y = 1.0f; }
			break;


		case EnemyType::TYPE_HEAVY: // ランダム移動

			VECTOR toTarget = VSub(mvTargetPos, mvPosition);
			float distance = VSize(toTarget);

			if (distance < 20.0f) // 近くなったらもう一度ランダム決める
			{
				mvTargetPos = VGet(
					(float)GetRand((int)MOVE_MAX * 2) - MOVE_MAX,
					(float)GetRand((int)MOVE_MAX * 2) - MOVE_MAX,
					mfPosZ
				);
				mfTargetRollZ = 0.0f;
				mfTargetRollX = 0.0f;
			}
			// 移動計算
			mvMoveVec = VNorm(toTarget);
			mvPosition = VAdd(mvPosition, VScale(mvMoveVec, speed));
			break;
		}

	}

	// 回転処理に渡すためのスピード
	mfNowSpeed = speed;

	// 傾き計算
	mfTargetRollZ = -mvMoveVec.x * ROLL_Z_MAX;
	mfTargetRollX = mvMoveVec.y * ROLL_X_MAX;
}


//void Enemy3D::MoveEx()
//{
//	VECTOR toTarget = VSub(mvTargetPos, mvPosition);
//	float distance = VSize(toTarget);
//
//	// 近くなったらもう一回設定
//	if (distance < 20.0f)
//	{
//		mvTargetPos = VGet(
//			(float)GetRand((int)MOVE_MAX * 2) - MOVE_MAX,
//			(float)GetRand((int)MOVE_MAX * 2) - MOVE_MAX,
//			mfPosZ
//		);
//
//		// 傾きリセット
//		mfTargetRollZ = 0.0f;
//		mfTargetRollX = 0.0f;
//	}
//
//	// 移動
//	mvMoveVec = VNorm(toTarget);
//	mvPosition = VAdd(mvPosition, VScale(mvMoveVec, SPEED_MAX));
//
//	// 傾き
//	mfTargetRollZ = -mvMoveVec.x * ROLL_Z_MAX;
//	mfTargetRollX = mvMoveVec.y * ROLL_X_MAX;
//}


// 移動による回転処理
void Enemy3D::RotationByMove()
{
	// Z方向の回転
	if (mfRollZ < mfTargetRollZ)
	{
		mfRollZ += ROLL_Z_SPEED;
		if (mfRollZ > mfTargetRollZ)
		{
			mfRollZ = mfTargetRollZ;
			mfRollZ = mfTargetRollZ;
		}
	}
	if (mfRollZ > mfTargetRollZ)
	{
		mfRollZ -= ROLL_Z_SPEED;
		if (mfRollZ < mfTargetRollZ)
		{
			mfRollZ = mfTargetRollZ;
		}
	}

	// X方向の回転
	if (mfRollX < mfTargetRollX)
	{
		mfRollX += ROLL_X_SPEED;
		if (mfRollX > mfTargetRollX)
		{
			mfRollX = mfTargetRollX;
		}
	}
	if (mfRollX > mfTargetRollX)
	{
		mfRollX -= ROLL_X_SPEED;
		if (mfRollX < mfTargetRollX)
		{
			mfRollX = mfTargetRollX;
		}
	}


	// 回転値を設定
	// 行きたい方向＋円周率
	//mvRotation.y = mfAngle + DX_PI_F;
	mvRotation.z = mfRollZ;
	mvRotation.x = mfRollX;
	mvRotation.y = DX_PI_F;
	// モデルに伝える
	// これがないと変らない
	mpModel->SetRotation(mvRotation);

}


void Enemy3D::Shot()
{
	mfBulletCounter++; // カウンタ進める
	if (mfBulletCounter > BULLET_SHOT_SPEED)
	{
		mfBulletCounter = 0.0f;

		VECTOR dir{};
		dir.x = sinf(mvRotation.y);
		dir.y = -sinf(mvRotation.x);
		dir.z = cosf(mvRotation.y);
		dir = VNorm(dir);

		int bulletIndex = MV1SearchFrame(mpModel->GetHandle(), "BulletFrame");
		VECTOR framePos;
		if (bulletIndex <= -1) // なんか実行したらフレームないとき-2だったから変更
		{
			framePos = mvPosition;
		}
		else
		{
			framePos = MV1GetFramePosition(mpModel->GetHandle(), bulletIndex);
		}
		Bullet3D* bullet = new Bullet3D(false, true, false, "Resource/Bullet/Bullet.mqo", framePos, dir, this, this->GetAttack());
		bullet->SetRotation(mvRotation); // たまに回転渡す
	}
}