#include "Bullet3D.h"
#include "Model.h"
#include "Master.h"
#include "Scene.h"
#include "SceneManager.h"
#include "ObjectManager.h"
#include "Player3D.h"
#include "Stage.h"
#include "Enemy3D.h"

Bullet3D::Bullet3D(
    bool isPlayerBullet,
    bool isEnemyBullet,
    bool isHomingBullet,
    std::string filename,
    VECTOR initPos,
    VECTOR direction,
    Unit* owner,
    float attack,
    Object3D* target
)
    : Object3D(initPos) // 基底クラスの初期化
    , mpModel(new Model(filename, initPos)) // モデルの生成
    , mvDirection(VNorm(direction)) // 方向ベクトルの正規化
    , mpOwner(owner) // オーナーの設定
    , mfAttack(attack) // 攻撃力の設定
    , mbIsPlayerBullet(isPlayerBullet) // プレイヤー弾フラグ
    , mbIsEnemyBullet(isEnemyBullet) // 敵弾フラグ
    , mbIsHomingBullet(isHomingBullet) // 誘導弾フラグ
    , mpTarget(target) // ターゲットの設定
{
    if (mbIsPlayerBullet) // プレイヤーの弾なら
    {
        SetTag(Object3D::T_PlayerBullet3D); // タグをプレイヤー弾に設定
        mTargetTag = Object3D::T_Enemy3D; // 敵をターゲットに設定
    }
    else if (mbIsEnemyBullet) // 敵の弾なら
    {
        SetTag(Object3D::T_EnemyBullet3D); // タグを敵弾に設定
        mTargetTag = Object3D::T_Player3D; // プレイヤーをターゲットに設定
    }
    else // それ以外（念のため）
    {
        SetTag(Object3D::T_PlayerBullet3D); // プレイヤー弾扱いにする
        mTargetTag = Object3D::T_Enemy3D; // 敵をターゲットに設定
    }

    if (mbIsHomingBullet) // 誘導弾の場合の初期化
    {
        if (mpTarget != nullptr) // ターゲットが存在すれば
        {
            mTargetTag = mpTarget->GetTag(); // タグを同期
        }
        else if (mbIsPlayerBullet) // プレイヤー弾なら
        {
            mTargetTag = Object3D::T_Enemy3D; // 敵をターゲットに
        }
        else if (mbIsEnemyBullet) // 敵弾なら
        {
            mTargetTag = Object3D::T_Player3D; // プレイヤーをターゲットに
        }

        mfSpeed = 15.0f; // 誘導弾用の速度設定
        float angleY = atan2f(GetDirection().x, GetDirection().z); // Y軸の回転角計算
        GetModel()->SetRotation(VGet(0.0f, angleY + DX_PI_F / 2.0f, 0.0f)); // モデルの向きを調整
    }
}

Bullet3D::~Bullet3D() // デストラクタ
{
    delete mpModel; // モデルのメモリ解放
}

void Bullet3D::Update() // 更新処理
{
    mvOldPosition = mvPosition; // 前フレームの位置を保存
    mfTime++; // 経過時間の更新

    if (mbIsHomingBullet) // 誘導弾なら
    {
        CalcDirection(); // 方向の再計算
    }

    Move(); // 移動
    Hit(); // 命中判定

    mpModel->SetPosition(mvPosition); // モデル位置の更新
    mpModel->SetRotation(mvRotation); // モデル回転の更新
    mpModel->Update(); // モデルの更新処理
}

void Bullet3D::Draw() // 描画処理
{
    mpModel->Draw(); // モデルを描画
}

void Bullet3D::Move() // 移動処理
{
    mvPosition = VAdd(mvPosition, VScale(mvDirection, mfSpeed)); // 位置を移動分加算

    mfMoveSpeed += mfSpeed; // 移動距離を蓄積
    if (mfMoveSpeed >= MOVE_DISTANCE) // 一定距離を超えたら
    {
        SetDeleteFlag(true); // 削除フラグを立てる
    }

    mpModel->SetPosition(mvPosition); // モデル位置を反映
}

void Bullet3D::CollDamage(Object3D* pTarget) // ダメージ処理
{
    Unit* pTargetObj = dynamic_cast<Unit*>(pTarget); // ユニットにキャスト
    if (!pTargetObj) return; // 失敗なら終了

    Player3D* pPlayer = dynamic_cast<Player3D*>(pTargetObj); // プレイヤーか確認
    if (pPlayer != nullptr) // プレイヤーの場合
    {
        if (pPlayer->GetGodFlag()) // 無敵状態なら
        {
            return; // 何もしない
        }
    }

    bool isPlayer = IsPlayerBullet(); // プレイヤーの弾か取得
    bool isHoming = IsHomingBullet(); // 誘導弾か取得

    pTargetObj->Damage(mfAttack, isPlayer, isHoming); // ダメージを与える
}

void Bullet3D::Hit() // 命中判定処理
{
    auto pTargetList = Master::mpSceneManager->GetCurrentScene() // シーンからターゲットリストを取得
        ->GetObjectManager()->GetObject3DListByTag(mTargetTag);

    for (auto obj : pTargetList) // リスト内の各オブジェクトに対して
    {
        Unit* pTargetUnit = dynamic_cast<Unit*>(obj); // ユニットか確認
        if (!pTargetUnit) continue; // 違うならスキップ

        bool isHit = HitCheck_Sphere_Sphere( // 球同士の当たり判定
            mvPosition, GetHitBulletSize(),
            pTargetUnit->GetPosition(), pTargetUnit->GetHitSize()
        );

        if (isHit) // 命中したら
        {
            CollDamage(obj); // ダメージ処理
            SetDeleteFlag(true); // 自身の削除フラグを立てる
            break; // 処理終了
        }
    }
}

void Bullet3D::CalcDirection() // 誘導方向の計算
{
    if (!mpTarget) // ターゲットがいない場合
    {
        SetDeleteFlag(true); // 削除
        return;
    }

    if (mpTarget->IsDeleteFlag()) // ターゲットが削除済みなら
    {
        mpTarget = nullptr; // ポインタを無効化
        SetDeleteFlag(true); // 削除
        return;
    }

    VECTOR targetPos = mpTarget->GetPosition(); // ターゲット位置取得
    targetPos.y += 20.0f; // 少し高さをオフセット
    VECTOR targetAngle = VSub(targetPos, GetPosition()); // 方向ベクトル計算
    SetDirection(VNorm(targetAngle)); // 方向をセット

    float angleY = atan2f(GetDirection().x, GetDirection().z); // モデルの向きを計算
    GetModel()->SetRotation(VGet(0.0f, angleY + DX_PI_F / 2.0f, 0.0f)); // 回転をセット
}