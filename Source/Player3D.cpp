#include "Player3D.h"
#include "Model.h"
#include "Master.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "ObjectManager.h"
#include "Utility.h"
#include "Object3D.h"
#include <algorithm>
#include "Bullet3D.h"
#include "Status.h"
#include "HpBar.h"
#include "Enemy3D.h"
#include "LockOnMark.h"
#include "Rock3D.h"
#include "Skill.h"
#include <string>


// 普通にゲームscene用の奴
Player3D::Player3D(VECTOR initPos, Status* status, bool isSeparateAnim)
	: Unit(initPos, status)
	, mvOldPosition(initPos)
	, mvMoveVec(VGet(0.0f, 0.0f, 0.0f))
{
	SetTag(Object3D::T_Player3D); // タグ

	// モデルの作成
	mpModel = new Model(status->GetModelName(), initPos, isSeparateAnim);
	// ロックオンのnew
	mpLockOn = new HpBar(this);
	mpLockMark = new LockOnMark(this);

	// ID取得してスキルやる
	mpSkill = new Skill(status->GetSkillType());

	// 初期行列の設定
	// ここで設定する名称は、mixamoからダウンロードしたモデルによって変わる可能性が高いので、
	// ModelViewer などで確認すること（おそらく、***:Hips となっているフレームがあればそれ。）
	mpModel->SetupInitializeMatrix("mixamorig:Hips");

	// ラグなったからこっちに移動
	SetFontSize(15); // 文字の大きさ
}

// Scene3Dで呼びだしている
void Player3D::Initialize()
{
	
}


// アニメーション追加
void Player3D::AddAnimation(AnimationState state, std::string filename)
{
	mpModel->AddAnimation(state, filename);
}

Player3D::~Player3D()
{
	// モデルの削除
	delete mpModel;
	delete mpLockOn;
	delete mpLockMark;
	delete mpSkill;
}

void Player3D::Update()
{
	// 回復debug用
	if (CheckHitKey(KEY_INPUT_K))
	{
		Heal();
	}

	mvOldPosition = mvPosition;

	// ロックオン処理
	LockOn();
	mpLockOn->UpdateBar();
	mpLockMark->UpdateMark();


	// 移動と回転処理
	MoveEx();
	RotationByMove();

	// スキル処理
	ActiveSkill();
	mpSkill->MainSkill(this);

	// 攻撃処理 ロックオン宙でなければ撃てるようにする
	if (!mbLockOn)
	{
		Shot();
	}

	// 点滅エフェクトの更新
	mCurrentPhoto += 10.0f;
	if (mCurrentPhoto > FLASH_INTERVAL)
	{
		mFlushTimer = !mFlushTimer;
		mCurrentPhoto = 0;
	}

	// モデルの更新と発光
	mpModel->SetPosition(mvPosition);
	mpModel->Update();

	// スキル中の発光エフェクト
	COLOR_F color = mpSkill->GetSkillFlag() ? 
		COLOR_F{3.0f, 3.0f, 3.0f, 1.0f} :    // 光らせる
		COLOR_F{1.0f, 1.0f, 1.0f, 1.0f};    // 元に戻す
	MV1SetEmiColorScale(mpModel->GetHandle(), color);

	// 親クラスの更新
	Object3D::Update();
}

void Player3D::Draw()
{
	// UI描画
	DrawSkillMark(); // スキルマーク
	DrawAttackMark(); // 通常攻撃マーク

	// デバッグ表示
	{
		int color = GetColor(255, 255, 255);
		DrawFormatString(20, 60,  color, "X: %f, Y: %f, Z: %f ", mvPosition.x, mvPosition.y, mvPosition.z);
		DrawFormatString(20, 80,  color, "スピード: %f", mfNowSpeed);
		DrawFormatString(20, 120, color, "HP %f, SP %f, MSP %f", GetHp(), GetSpeed(), GetMaxSpeed());
		DrawFormatString(20, 150, color, "Cool: %.1f", GetLockCoolTime());
		DrawFormatString(20, 170, color, "攻撃力 %.1f", GetAttack());
	}

	// ロックオン関連の描画
	mpLockOn->DrawBar();
	mpLockMark->DrawMark();
	mpSkill->SkillDebugDraw(); // デバック用

	// モデル描画
	mpModel->Draw();
}

void Player3D::DrawSkillMark()
{
	SetUseZBuffer3D(FALSE);
	SetWriteZBuffer3D(FALSE);

	constexpr int BOX_X = 150;
	constexpr int BOX_Y = 850;
	constexpr int BOX_SIZE = 100;
	constexpr int BOX_RADIUS = 20;
	constexpr int TEXT_OFFSET_X = 2;
	constexpr int TEXT_OFFSET_Y = 33;
	const int TEXT_COLOR = GetColor(255, 255, 255);

	// スキルの状態に応じて色を決定
	int r = mpSkill->GetSkillFlag() ? 230 : 150;
	int g = 0;
	int b = 0;

	// テキストを作成
	char skillText[64];
	if (mpSkill->GetSkillFlag())
	{
		sprintf_s(skillText, "  発動中\nあと%.1f秒", mpSkill->GetSkillTime());
	}
	else
	{
		sprintf_s(skillText, " %s\n  Bキー", GetSkillMemo().c_str());
	}

	int color = GetColor(r, g, b);

	// 四角モドキ描画
	DrawRoundRect(BOX_X, BOX_Y, BOX_X + BOX_SIZE, BOX_Y + BOX_SIZE, BOX_RADIUS, BOX_RADIUS, color, TRUE);
	DrawFormatString(BOX_X + TEXT_OFFSET_X, BOX_Y + TEXT_OFFSET_Y, TEXT_COLOR, "%s", skillText);

	SetUseZBuffer3D(TRUE);
	SetWriteZBuffer3D(TRUE);
}

void Player3D::DrawAttackMark()
{
	SetUseZBuffer3D(FALSE);
	SetWriteZBuffer3D(FALSE);

	constexpr int BOX_X = 40;
	constexpr int BOX_Y = 850;
	constexpr int BOX_SIZE = 100;
	constexpr int BOX_RADIUS = 20;
	constexpr int TEXT_OFFSET_X = 2;
	constexpr int TEXT_OFFSET_Y = 33;
	const int TEXT_COLOR = GetColor(255, 255, 255);

	// 攻撃中かどうかで色を決定
	int r = 0;
	int g = mbAttack ? 230 : 150;
	int b = 0;

	// テキストを作成
	const char* attackText = mbAttack ? "  通常\n  攻撃中" : "  通常\n  Fキー";
	int color = GetColor(r, g, b);

	// 四角モドキ描画
	DrawRoundRect(BOX_X, BOX_Y, BOX_X + BOX_SIZE, BOX_Y + BOX_SIZE, BOX_RADIUS, BOX_RADIUS, color, TRUE);
	DrawFormatString(BOX_X + TEXT_OFFSET_X, BOX_Y + TEXT_OFFSET_Y, TEXT_COLOR, "%s", attackText);

	SetUseZBuffer3D(TRUE);
	SetWriteZBuffer3D(TRUE);
}



// 移動処理（ステージとのあたり判定用）
void Player3D::MoveEx()
{
	// 移動方向の入力
	VECTOR inputVec = VGet(0.0f, 0.0f, 0.0f);
	VECTOR upMoveVector = VGet(0.0f, 0.0f, 0.0f);    // カメラの奥方向ベクトル
	VECTOR leftMoveVector = VGet(0.0f, 0.0f, 0.0f);  // カメラの左方向ベクトル

	// カメラの向きから移動ベクトルを求める
	{
		// 上方向への移動ベクトルは、カメラの視点方向からY成分を抜いたもの
		upMoveVector = VSub(Master::mpCamera->GetLookAtPosition(), Master::mpCamera->GetPosition());
		upMoveVector.y = 0.0f;

		// 左方向への移動ベクトルは、上方向への移動ベクトルと、Y軸のプラス方向へのベクトルに垂直な方向（外積）
		leftMoveVector = VCross(upMoveVector, VGet(0.0f, 1.0f, 0.0f));
		leftMoveVector.y = 0.0f;

		// 移動ベクトルを正規化（ベクトルの長さを1にする）
		upMoveVector = VNorm(upMoveVector);
		leftMoveVector = VNorm(leftMoveVector);
	}

	// 回転目標を初期化
	mfTargetRollZ = 0.0f;
	mfTargetRollX = 0.0f;

	// キー入力による移動方向の決定
	if (CheckHitKey(KEY_INPUT_S)) // 上移動（下に傾く）
	{
		mfTargetRollX = ROLL_X_MAX;
		inputVec = VAdd(inputVec, VGet(0.0f, -1.0f, 0.0f));
	}

	if (CheckHitKey(KEY_INPUT_W)) // 下移動（上に傾く）
	{
		mfTargetRollX = -ROLL_X_MAX;
		inputVec = VAdd(inputVec, VGet(0.0f, 1.0f, 0.0f));
	}

	if (CheckHitKey(KEY_INPUT_A)) // 左移動（左に傾く）
	{
		mfTargetRollZ = ROLL_Z_MAX;
		inputVec = VAdd(inputVec, leftMoveVector);
	}

	if (CheckHitKey(KEY_INPUT_D)) // 右移動（右に傾く）
	{
		mfTargetRollZ = -ROLL_Z_MAX;
		inputVec = VAdd(inputVec, VScale(leftMoveVector, -1.0f));
	}


	float input = VSize(inputVec);
	bool isMove = (input > 0.0f);

	if (isMove) // 動いていたら移動
	{
		mvMoveVec = VNorm(inputVec);

		// ダッシュ状態なら速度を速くする処理
		float targetSpeed;

		if (CheckHitKey(KEY_INPUT_LSHIFT))
		{
			mfNowSpeed += (mfSpeed * BY_SPEED);
			targetSpeed = GetMaxSpeed();
		}
		else
		{
			mfNowSpeed += mfSpeed;
			targetSpeed = GetSpeed();
		}
		
		// ロックオン中なら速度を指定した%にするため
		if (mbLockOn) { targetSpeed *= LOCK_SPEED_RATE; }

		if (mfNowSpeed >= targetSpeed)
		{
			mfNowSpeed = targetSpeed;
		}
	}
	else
	{
		mfNowSpeed--;
		if (mfNowSpeed <= 0.0f)
		{
			mfNowSpeed = 0.0f;
			mvMoveVec = VGet(0.0f, 0.0f, 0.0f);
		}
	}
	mvPosition = VAdd(mvPosition, VScale(mvMoveVec, mfNowSpeed));

	// 移動位置を画面内にクランプ
	ClampPosition();
}

// プレイヤーの位置を移動制限内にクランプする
void Player3D::ClampPosition()
{
	// X方向の移動上限
	if (mvPosition.x >= MOVE_MAX_X) mvPosition.x = MOVE_MAX_X;
	if (mvPosition.x <= -MOVE_MAX_X) mvPosition.x = -MOVE_MAX_X;
	
	// Y方向の移動上限
	if (mvPosition.y >= MOVE_MAX_Y_U) mvPosition.y = MOVE_MAX_Y_U;
	if (mvPosition.y <= -MOVE_MAX_Y_D) mvPosition.y = -MOVE_MAX_Y_D;
}


// 移動による回転処理
void Player3D::RotationByMove()
{
	// Z方向とX方向の回転を更新
	UpdateRotationAxis(mfRollZ, mfTargetRollZ, ROLL_Z_SPEED);
	UpdateRotationAxis(mfRollX, mfTargetRollX, ROLL_X_SPEED);

	// 回転値を設定してモデルに伝える
	mvRotation.z = mfRollZ;
	mvRotation.x = mfRollX;
	mpModel->SetRotation(mvRotation);
}

// 回転軸の更新（Z方向とX方向で共通化）
void Player3D::UpdateRotationAxis(float& current, float target, float speed)
{
	if (current < target)
	{
		current += speed;
		if (current > target) current = target;
	}
	else if (current > target)
	{
		current -= speed;
		if (current < target) current = target;
	}
}

void Player3D::Shot()
{
	mfBulletCounter++; // カウンタ進める

	// 通常攻撃フラグON
	if (CheckHitKey(KEY_INPUT_F))
	{
		mbAttack = true;
	}
	else
	{
		mbAttack = false;
	}

	// 押されたらまたはスキル中だったら
	if (mbAttack || mpSkill->GetAttackSkill())
	{
		if (mfBulletCounter > BULLET_SHOT_SPEED)
		{
			mfBulletCounter = 0.0f;

			VECTOR dir = GetForwardDirection();

			int bulletIndex = MV1SearchFrame(mpModel->GetHandle(), "BulletFrame");
			VECTOR framePos;
			if (bulletIndex == -1)
			{
				framePos = mvPosition;
			}
			else
			{
				framePos = MV1GetFramePosition(mpModel->GetHandle(), bulletIndex);
			}

			Bullet3D* bullet = new Bullet3D(true, false, false, "Resource/Bullet/PlayerBullet.mqo", framePos, dir, nullptr, this->GetAttack());
			bullet->SetRotation(mvRotation); // たまに回転渡す
		}
	}
}

// プレイヤーの正面方向を計算
VECTOR Player3D::GetForwardDirection() const
{
	VECTOR forward;
	forward.x = sinf(mvRotation.y);
	forward.y = -sinf(mvRotation.x);
	forward.z = cosf(mvRotation.y);
	return VNorm(forward);
}


// クールタイムの更新
void Player3D::UpdateLockCoolTime()
{
	if (mfLockOnCoolTime > 0)
	{
		mfLockOnCoolTime -= 1.0f;
		// 硬直中はスペースを押しても何もさせん
		if (CheckHitKey(KEY_INPUT_SPACE)) return;
	}
}

// ロック間隔の更新
void Player3D::UpdateLockDelay()
{
	if (mfLockDelay > 0)
	{
		mfLockDelay -= 1.0f;
	}
}

// 死んでいるロック対象を削除
void Player3D::CleanupDeadLockedTargets()
{
	for (auto it = mvLockedTargets.begin(); it != mvLockedTargets.end(); )
	{
		// ターゲットが削除フラグを持っている場合
		if ((*it)->IsDeleteFlag())
		{
			it = mvLockedTargets.erase(it); // リストから除外
		}
		else
		{
			++it; // 消されなかったときは隣の要素へ
		}
	}
}

// ロック失敗した敵のマークを消す
void Player3D::UnlockFailedTargets()
{
	// ターゲットリスト
	auto pObjManager = Master::mpSceneManager->GetCurrentScene()->GetObjectManager();
	auto pEnemyList = pObjManager->GetObject3DListByTag(Object3D::T_Enemy3D);
	auto pRockList = pObjManager->GetObject3DListByTag(Object3D::T_Rock3D);

	std::vector<Object3D*> allPotentialTargets = pEnemyList;
	allPotentialTargets.insert(allPotentialTargets.end(), pRockList.begin(), pRockList.end());

	// マークの強制お掃除処理
	for (auto obj : allPotentialTargets)
	{
		// プレイヤーのロック名簿に入っているか確認
		auto it = std::find(mvLockedTargets.begin(), mvLockedTargets.end(), obj);
		// 名簿にいないのにマークがついているなら強制的に消す
		if (it == mvLockedTargets.end())
		{
			// リストにいないオブジェクトのロック状態を解除
			if (Unit* pUnit = dynamic_cast<Unit*>(obj))
			{
				pUnit->SetLocked(false);
			}
		}
	}
}

// 新しいターゲットをロック
void Player3D::LockNewTarget()
{
	// クールタイム中はロックオン禁止
	if (mfLockOnCoolTime > 0.0f) { return; }


	mbLockOn = true; // ロックオンtrue

	// 連続クールタイムが0か かつ ロック対象数に達していないか
	if (mfLockDelay <= 0)
	{
		Object3D* pTarget = GetNearestTarget(); // いっちゃん近い敵のやつ
		if (pTarget) // 敵がいたら
		{
			// ロックした瞬間に20引く
			mvLockedTargets.push_back(pTarget); // 発射リストに登録

			// 次のロックまで待つ
			// 岩だったらロックオン間隔短く
			if (pTarget->GetTag() == Object3D::T_Rock3D)
			{
				mfLockDelay = LOCK_DELAY_ROCK;
			}
			else
			{
				mfLockDelay = LOCK_DELAY_ENEMY;
			}
		}
	}

	// チャージが完了した敵すべてにマークを表示させる
	for (size_t i = 0; i < mvLockedTargets.size(); i++)
	{
		// リストの最後より前の敵 = 既にチャージ待ち時間を経過している
		// リストの最後の敵、かつ mfLockDelay が 0 になった = 今チャージが終わった
		if (i < mvLockedTargets.size() - 1 || mfLockDelay <= 0.0f)
		{
			if (Unit* pUnit = dynamic_cast<Unit*>(mvLockedTargets[i]))
			{
				pUnit->SetLocked(true);
			}
		}
	}
}

// ロック中の敵に発射
void Player3D::FireLockedBullets()
{
	// ターゲット候補者たち
	std::vector<Object3D*> readyToShoot;

	// ループで全員みている
	for (auto target : mvLockedTargets)
	{
		// Unitを継承しているのに入るから敵　プレイヤー　岩だけ
		Unit* pUnit = dynamic_cast<Unit*>(target);
		if (pUnit && pUnit->IsLocked())
		{
			readyToShoot.push_back(target); // ちゃーじ完了してたらリスト追加
		}
		else if (pUnit)
		{
			pUnit->SetLocked(false); // 間に合わなかった敵のマークを消す
		}
	}

	if (!readyToShoot.empty())
	{
		// チャージ完了した敵へ一斉発射
		// ロック数 * 時間で行ける
		mfLockOnCoolTime = (float)readyToShoot.size() * (60 * LOCK_COOLTIME_MAX);
		for (auto target : readyToShoot)
		{
			// ターゲットが存在しなかったら打たない
			if (!target || target->IsDeleteFlag()) continue;

			// プレイヤーのフレームとって弾が出る位置を調整
			int bulletIndex = MV1SearchFrame(mpModel->GetHandle(), "BulletFrame");
			// ポジションがなかったら真ん中から出るように
			VECTOR muzzlePos = (bulletIndex != -1) ?
				MV1GetFramePosition(mpModel->GetHandle(), bulletIndex) : mvPosition;
			VECTOR dir = VSub(target->GetPosition(), muzzlePos);

			new Bullet3D(true, false, true, "Resource/Bullet/PlayerBullet.mqo", muzzlePos, dir, nullptr, GetHomingAttack(), target);
		}
	}

	mvLockedTargets.clear(); // リストクリア
}


// ロックオン対象を解除
void Player3D::LockOffTargets()
{
	mbLockOn = false; // ロックオン解除
	mfLockDelay = 0;  // リセット

}



// メインのロックオン関数 ヘルパー関数を統合管理
void Player3D::LockOn()
{
	// ホーミング弾が飛んでいる時にロックオンをさせない処理
	// ロックオンが二重になり、無限ホーミングが可能なため。
	auto pBulletList = Master::mpSceneManager->GetCurrentScene()
		->GetObjectManager()->GetObject3DListByTag(Object3D::T_PlayerBullet3D);

	for (auto obj : pBulletList)
	{
		Bullet3D* pBullet = dynamic_cast<Bullet3D*>(obj);
		if (pBullet && pBullet->IsHomingBullet())
		{
			// ホーミング弾が飛んでいるのでロックオン禁止
			return;
		}
	}


	// 時間管理の更新
	UpdateLockCoolTime();
	UpdateLockDelay();

	// クールタイム中はロックオン禁止
	if (mfLockOnCoolTime > 0.0f) { return; }

	// ロック対象の整理
	CleanupDeadLockedTargets();
	UnlockFailedTargets();

	// スペースキーの入力判定
	if (CheckHitKey(KEY_INPUT_SPACE))
	{
		LockNewTarget();
	}
	else if (mbLockOn)
	{
		// スペースキーを離した時の処理
		FireLockedBullets();
		LockOffTargets();
	}
}
Object3D* Player3D::GetNearestTarget()
{
	// 敵リスト取得
	auto pTargetList = Master::mpSceneManager->GetCurrentScene()->GetObjectManager();
	
	auto pEnemyList = pTargetList->GetObject3DListByTag(Object3D::T_Enemy3D);
	auto pRockList = pTargetList->GetObject3DListByTag(Object3D::T_Rock3D);

	std::vector<Object3D*> targets = pEnemyList;
	// 二つのリストを１つにまとめる
	targets.insert(targets.end(), pRockList.begin(), pRockList.end());
	// ターゲットがいなかったら
	if (targets.empty()) return nullptr;

	Object3D* pNearEnemy = nullptr;
	float maxDist = FLT_MAX; // くそでか入れてる

	// ターゲット全部まわす
	for (auto pTarget : targets)
	{
		// すでに死んでいるまたは削除待ちのオブジェクトは無視する
		if (pTarget->IsDeleteFlag()) continue;

		if (pTarget->GetTag() == Object3D::T_Rock3D)
		{
			Rock3D* pRock = dynamic_cast<Rock3D*>(pTarget);
			if (pRock && pRock->IsBrokenFlag()) continue;
		}
		
		// すでにリストに入っているのはスキップ
		auto it = std::find(mvLockedTargets.begin(), mvLockedTargets.end(), pTarget);
		// endは最後までいった...見つからなかっただから途中で見つかったら
		if (it != mvLockedTargets.end()) continue; // 見つかったらスキップすりっぷ

		// 距離
		float dist = VSize(VSub(pTarget->GetPosition(), mvPosition));
		if (dist < maxDist)
		{
			maxDist = dist;
			pNearEnemy = pTarget;
		}
	}
	return pNearEnemy;
}

void Player3D::ActiveSkill()
{
	if (InputManager::CheckDownKey(KEY_INPUT_B))
	{
		mpSkill->Execute(this);
	}
}