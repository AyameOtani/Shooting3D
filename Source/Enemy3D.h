#pragma once

#include "DxLib.h"
#include <string>
#include "Unit.h"
#include "Model.h"


class Enemy3D : public Unit
{

public:

	// 敵のタイプ
	enum class EnemyType
	{
		TYPE_NORMAL,
		TYPE_SPEED,
		TYPE_HEAVY,
	};

	// 出現位置
	enum class SpawnSide
	{
		RIGHT,
		LEFT,
		TOP,
	};

	// 敵の出現位置
	// どっから出てどこまで移動して止まるかのやつ
	void SetPattern(SpawnSide side, float offset);

private:
	static constexpr float MOVE_MAX = 570.0f; // 最大移動XY距離
	EnemyType m_Type; // 自分のタイプ保存
	int m_ID; // ID保存

public:
	Enemy3D(std::string filename, VECTOR initPos, EnemyType type, int id, bool isSeparateAnim = false);
	~Enemy3D();

	void Update() override; // 更新  引き継ぐから override
	void Draw() override;   // 描画
	void MoveEx();			// 移動処理
	void RotationByMove();  // 回転処理の関数

	// 弾撃つ
	void Shot();

	// アニメーション追加
	// Modelクラスへの橋渡し関数
	void AddAnimation(AnimationState state, std::string filename);

	// フェード中かのゲッター
	bool GetFadeIn() const { return mbIsFade; }

private:
	// ポインタ関係
	Model* mpModel;    // モデルクラスのポインタ
	VECTOR mvOldPosition; // 古いポジション
	float mfNowSpeed = 0.0f; // 今のスピード

	// 回転関係
	float mfSpeed = 0.04f; // 速くなる速さ	mfSpeedに加算するやつ
	VECTOR mvMoveVec; // ベクトル

	// 左右回転
	const float ROLL_Z_MAX = DX_PI_F / 2.4f; // Z方向の傾き最大 90度ぐらい
	const float ROLL_Z_SPEED = 0.06f;  // 傾くスピード
	float mfTargetRollZ = 0.0f;      // Z方向の傾き 左右AD
	float mfRollZ = 0.0f;			 // 今の傾きZ

	// 上下回転
	const float ROLL_X_MAX = DX_PI_F / 4.5f; // X方向の傾き最大 60度ぐらい
	const float ROLL_X_SPEED = 0.02f;  // 傾くスピード
	float mfTargetRollX = 0.0f; // X方向の傾き 上下WS
	float mfRollX = 0.0f;			 // 今の傾きX

private: // 位置関係
	float mfPosZ = 800.0f; // Zは固定するため
	VECTOR mvTargetPos = (VGet(0.0f, 0.0f, 1000.0f)); // 目標地点を決める


private: // 弾関係
	static constexpr float BULLET_SHOT_SPEED = 10.0f; // 弾を撃てる間隔
	float mfBulletCounter = 0.0f; // カウンタ


private: // 登場シーンたち
	float mfFadeRatio = 0.0f; // 0.0(画面外) から 1.0(定位置) までの進捗
	bool mbIsFade= true; // 登場演出中かどうかのフラグ
	float mfStartX = 0.0f;  //　開始地点
	float mfStartY = 0.0f;  //　開始地点
	float mfTargetX = 0.0f; // ターゲット位置
	float mfTargetY = 0.0f; // ターゲット位置
	bool mbIsRamdam = false; // 一回ランダム移動が終わったか
	float mfFadeDist = 0.0045f; // fade scene speed



private: // ロックオンされたか
	int mLockBillboardHandle; // ロックオン用画像のハンドル
	
};


	//else
	//{
	//	// スペースを離した時にロックオン中だったら
	//	if (mbLockOn)
	//	{
	//		// 空じゃないなら
	//		if (!mvLockedTargets.empty())
	//		{
	//			// クールタイム
	//			// ロック数 * 時間で行ける
	//			mfLockOnCoolTime = (float)mvLockedTargets.size() * (60 * mfMaxCool);
	//			// ロックオン対象がいる場合一斉発射
	//			for (auto target : mvLockedTargets)
	//			{
	//				int bulletIndex = MV1SearchFrame(mpModel->GetHandle(), "BulletFrame");
	//				VECTOR muzzlePos = (bulletIndex != -1) ?
	//					MV1GetFramePosition(mpModel->GetHandle(), bulletIndex) : mvPosition;
	//				VECTOR dir = VSub(target->GetPosition(), muzzlePos);
	//				// ホーミング
	//				new HomingBullet3D(
	//					Object3D::T_Player3D,
	//					this,
	//					target,
	//					muzzlePos,
	//					"Resource/Bullet/Bullet.mqo",
	//					dir,
	//					10.0f
	//				);
	//			}
	//			mvLockedTargets.clear(); // リストクリア
	//		}
	//		else // 敵がいないだったら
	//		{
	//			// 少し減らすことにしたわ
	//			AddLockGauge(-10.0f);
	//			// 正面に1発撃つ
	//			int bulletIndex = MV1SearchFrame(mpModel->GetHandle(), "BulletFrame");
	//			VECTOR muzzlePos = (bulletIndex != -1) ?
	//				MV1GetFramePosition(mpModel->GetHandle(), bulletIndex) : mvPosition;
	//			// プレイヤーの正面方向を計算
	//			VECTOR forward;
	//			forward.x = sinf(mvRotation.y);
	//			forward.y = -sinf(mvRotation.x);
	//			forward.z = cosf(mvRotation.y);
	//			forward = VNorm(forward);
	//			// ターゲットを nullptr にしてホーミング弾を生成
	//			new HomingBullet3D(
	//				Object3D::T_Player3D,
	//				this,
	//				nullptr,     // ターゲットなし
	//				muzzlePos,
	//				"Resource/Bullet/Bullet.mqo",
	//				forward,     // 正面方向
	//				0.0f
	//			);
	//		}
	//	}
	//	mbLockOn = false; // ロックオン解除
	//	mfLockDelay = 0; // 次回のためにリセット
	//}
