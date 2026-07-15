#pragma once

#include "DxLib.h"
#include <string>
#include "Model.h"
#include "Unit.h"
#include "Status.h"

// 前方宣言
class Model;
class HpBar;
class LockOnMark;
class Skill;

class Player3D : public Unit // 継承する
{
private:
	// float系
	static constexpr float MOVE_MAX_X = 590.0f; // 最大移動X距離
	static constexpr float MOVE_MAX_Y_U = 280.0f; // 最大移動Y上距離
	static constexpr float MOVE_MAX_Y_D = 200.0f; // 最大移動Y下距離
	// int系
	static const int BY_SPEED = 10; // 今のスピードに乗算する
	// ロックオン関係
	static constexpr float LOCK_GAUGE_MAX = 100.0f; // ロックゲージ最大値
	static constexpr float LOCK_DELAY_ENEMY = 110.0f; // 敵のロック間隔
	static constexpr float LOCK_DELAY_ROCK = 45.0f; // 岩のロック間隔
	static constexpr float LOCK_COOLTIME_MAX = 0.5f; // クールタイムMAX
	// 演出系
	static constexpr float FLASH_INTERVAL = 200.0f; // フラッシュ間隔



public:
	Player3D(VECTOR initPos, Status* status, bool isSeparateAnim = false);
	~Player3D(); // デストラクタ

	void Initialize();
	void Update() override; // 更新  引き継ぐから override
	void Draw() override;   // 描画

	// -------移動系----------------------
	void MoveEx();			// 移動処理
	void RotationByMove();  // 回転処理の関数

	// -------ショット・スキル関係--------
	void Shot(); // 弾攻撃
	void ActiveSkill(); // スキル発動
	void DrawSkillMark(); // スキルマーク
	void DrawAttackMark(); // 通常マーク

	// -------アニメーション追加 Modelクラスへの橋渡し関数
	void AddAnimation(AnimationState state, std::string filename);

	// -------ロックオン関係
	void LockOn(); // ロックオンの関数
	bool GetLockOn() const { return mbLockOn; } // 今ロックオンしているかのbool

	// -------ロックオン系のゲッターセッター
	int GetLockTargetCount() const { return (int)mvLockedTargets.size(); } // 何体の敵をロックオンしているかのゲッター
	float GetLockCoolTime() const { return mfLockOnCoolTime; } // ロックオンクールタイムの取得

	// &で参照してそんまま呼ぶ のぞき見  const ないとエラーなる
	const std::vector<Object3D*>& GetLockedTargets() const { return mvLockedTargets; }
	float GetLockDelay() const { return mfLockDelay; } // 間隔時間
	float GetMaxDelay() const { return LOCK_DELAY_ENEMY; }   // 最大間隔時間

	// --------無敵時間のゲッター------
	bool GetGodFlag() const { return mbGod; }
	void SetGodFalg(bool sg) { mbGod = sg; }

private:
	// ------プレイヤーの正面方向を計算
	VECTOR GetForwardDirection() const;

	// -----ポインタ関係
	Model* mpModel;			 // モデルクラスのポインタ
	HpBar* mpLockOn;		 // ロックオンポインタ
	LockOnMark* mpLockMark;  // 円ゲージマーク
	Skill* mpSkill;			 // スキルのポインタ

	// ------移動関係
	VECTOR mvOldPosition;    // 古いポジション
	float mfNowSpeed = 0.0f; // 今のスピード
	float mfSpeed = 0.04f;   // 速度の加速度
	VECTOR mvMoveVec;	     // ベクトル

	// ------回転関係
	// 左右回転
	static constexpr float ROLL_Z_MAX = DX_PI_F / 2.4f; // Z方向の傾き最大 90度ぐらい
	static constexpr float ROLL_Z_SPEED = 0.08f;  // 傾くスピード
	float mfTargetRollZ = 0.0f;      // Z方向の傾き 左右AD
	float mfRollZ = 0.0f;			 // 今の傾きZ

	// 上下回転
	static constexpr float ROLL_X_MAX = DX_PI_F / 4.5f; // X方向の傾き最大 60度ぐらい
	static constexpr float ROLL_X_SPEED = 0.02f;  // 傾くスピード
	float mfTargetRollX = 0.0f; // X方向の傾き 上下WS
	float mfRollX = 0.0f;			 // 今の傾きX


	// ------ 弾関係
	static constexpr float BULLET_SHOT_SPEED = 5.0f; // 弾を撃てる間隔
	static constexpr int STICK_DEAD_ZONE = 200; // パッドのデッドゾーン
	float mfBulletCounter = 0.0f; // カウンタ
	bool mbAttack = false; // 通常攻撃中はTRUE

	// ------ ロックオン関係
	float mfLockGauge = 0.0f;				 // ロックオンのゲージなう
	bool mbLockOn = false;					 // 今ロックオンチャージ中か SPACE押されているか
	std::vector<Object3D*> mvLockedTargets;  // ロックオン中の敵リスト
	Object3D* GetNearestTarget();			 // 敵とかターゲットリスト
	float mfLockDelay = 0.0f;				 // ロックオンの間隔管理
	float mfLockOnCoolTime = 0.0f;			 // 撃った後の硬直NOW

	// ------ 無敵
	bool mbGod = false; // 無敵かどうか

	// ------ 演出系
	float mCurrentPhoto = 0.0f;
	bool mFlushTimer = false;


	// ------プライベートヘルパー関数
	void UpdateLockCoolTime();
	void UpdateLockDelay();
	void CleanupDeadLockedTargets();
	void UnlockFailedTargets();
	void LockNewTarget();
	void FireLockedBullets();
	void LockOffTargets();
	
	// ------ 移動関連ヘルパー関数
	void ClampPosition();
	void UpdateRotationAxis(float& current, float target, float speed);

};


