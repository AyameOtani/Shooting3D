#pragma once

#include "Object3D.h" // 親クラスのヘッダー読み込み
#include <string>     // 文字列クラスの読み込み

class Model; // 前方宣言
class Unit;  // 前方宣言

class Bullet3D : public Object3D // Bullet3Dクラスの定義
{
public:
    Bullet3D( // コンストラクタ
        bool isPlayerBullet, // プレイヤーの弾かどうか
        bool isEnemyBullet,  // 敵の弾かどうか
        bool isHomingBullet, // 誘導弾かどうか
        std::string filename, // モデルファイル名
        VECTOR initPos,      // 初期位置
        VECTOR direction,    // 移動方向
        Unit* owner,         // 発射元のユニット
        float attack = 10.0f, // 攻撃力
        Object3D* target = nullptr // 誘導対象
    );
    ~Bullet3D() override; // デストラクタ

    void Update(); // 更新処理
    void Draw();   // 描画処理
    void Move();   // 移動処理
    void CollDamage(Object3D* pTarget); // 衝突時のダメージ処理
    void Hit();    // 命中時の処理

    float GetSpeed() const { return mfSpeed; } // 速度の取得
    void SetSpeed(float sp) { mfSpeed = sp; } // 速度の設定

    float GetHitBulletSize() const { return mfHitBulletSize; } // 当たり判定サイズの取得
    void SetHitBulletSize(float bsz) { mfHitBulletSize = bsz; } // 当たり判定サイズの設定

    VECTOR GetDirection() const { return mvDirection; } // 方向ベクトルの取得
    void SetDirection(VECTOR dir) { mvDirection = dir; } // 方向ベクトルの設定
    Model* GetModel() { return mpModel; } // モデルの取得

    void SetTargetTag(Object3D::Tag3D tag) { mTargetTag = tag; } // ターゲットタグの設定
    bool IsPlayerBullet() const { return mbIsPlayerBullet; } // プレイヤー弾判定
    bool IsHomingBullet() const { return mbIsHomingBullet; } // 誘導弾判定

private:
    void CalcDirection(); // 方向の計算

private:
    Model* mpModel = nullptr; // モデルポインタ
    VECTOR mvDirection = VGet(0.0f, 0.0f, 0.0f); // 移動方向ベクトル
    float mfAngle = 0.0f; // 角度
    float mfMoveSpeed = 0.0f; // 移動速度
    float mfSpeed = 50.0f; // 基本速度
    float mfTime = 0.0f; // 経過時間
    Unit* mpOwner = nullptr; // 所有者ポインタ
    float mfAttack = 10.0f; // 攻撃力
    float mfHitBulletSize = 10.0f; // 当たり判定の大きさ
    Object3D::Tag3D mTargetTag = Object3D::None3D; // ターゲットのタグ
    Object3D* mpTarget = nullptr; // ターゲットポインタ
    bool mbIsPlayerBullet = false; // プレイヤー弾フラグ
    bool mbIsEnemyBullet = false; // 敵弾フラグ
    bool mbIsHomingBullet = false; // 誘導弾フラグ
    VECTOR mvOldPosition = VGet(0.0f, 0.0f, 0.0f); // 前フレームの位置
    const float MOVE_DISTANCE = 3000.0f; // 消滅距離
    const float GRAVITY_DIST = 10.0f; // 重力距離
    const float GRAVITY = 0.001f; // 重力値
};