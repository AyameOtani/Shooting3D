#pragma once
#include <DxLib.h>
#include <vector>

//============================================================
// BreakRock クラス
//
// ・intact(通常モデル) と shards(破片モデル) を読み込む
// ・Break() を呼ぶとガラスが割れて破片が飛び散る
// ・Update() で物理計算（重力・減衰・寿命管理）
// ・Draw() で描画（半透明ガラス or 破片表示）
//============================================================

class BreakRock
{
public:
	BreakRock();
	~BreakRock();

	// 割れた状態のモデル
	bool Load(int modelHandle);

	// モデルの後始末
	void Finalize();

	 // 元の状態に戻す
	void Reset();


	// 割る
	// impactPos...衝突した位置
	// power... 飛び散る強さ
    void Break(const VECTOR& impactPos, float power);

	// 毎フレーム更新
	void Update(float dt, float sp);

	// 描画
	void Draw() const;


	// 割れているかどうか
	bool IsBroken() const { return mBroken; }

	// 位置を設定
	void SetPosition(const VECTOR& pos)
	{
		mPos = pos;
		ApplyTransformToIntact(); // intactモデルにも反映
	}

	// 現在位置の取得
	const VECTOR& GetPosition() const { return mPos; }

	// 拡大縮小率を設定
	void SetScale(float s)
	{
		mScale = s;
		ApplyTransformToIntact(); // intact側に反映
		ApplyScaleToPieces();     // 破片側にも反映
	}


    // 重力の強さ
    void SetGravity(const VECTOR& g) { mGravity = g; }


    // 減衰率（空気抵抗みたいなもの）
    // 1.0fに近いほど止まりにくい
    void SetDamping(float d)
    {
        // 0～1の範囲に制限
        mDamping = (d < 0.0f) ? 0.0f : (d > 1.0f ? 1.0f : d);
    }


private:
    // 1つの破片データ
    struct Piece
    {
        int model = -1;          // 破片用の複製モデル
        int frameIndex = -1;     // 描画するフレーム番号

        VECTOR pos = VGet(0, 0, 0); // 現在位置
        VECTOR vel = VGet(0, 0, 0); // 移動速度

        VECTOR rot = VGet(0, 0, 0);     // 現在回転（XYZラジアン）
        VECTOR rotVel = VGet(0, 0, 0);  // 回転速度

        bool active = false; // 表示・更新するかどうか

        VECTOR local = VGet(0, 0, 0); // <- 追加: フレームのローカル座標

    };


    // α値を 0～255 に収める
    static int ClampAlpha(int a)
    {
        if (a < 0) return 0;
        if (a > 255) return 255;
        return a;
    }

    // 0.0～1.0 の乱数
    float Rand01() const;

    // a～b の範囲の乱数
    float RandRange(float a, float b) const;

    // shardsモデルから破片一覧を作る
    void BuildPieces();

    // intactモデルに位置・スケールを反映
    void ApplyTransformToIntact();

    // 破片モデルすべてにスケールを反映
    void ApplyScaleToPieces();


    int mShards = -1; // 破片モデル

    VECTOR mPos = VGet(0, 0, 0); // 岩の位置
    float  mScale = 1.0f;       // 拡大縮小率

    // 割れているかどうか
    bool mBroken = false;

    int mIntactAlpha = 140; // intact時の透明度
    int mShardAlpha = 200;  // 破片時の透明度

    VECTOR mGravity = VGet(0.0f, -30.0f, 0.0f); // 重力
    float  mDamping = 0.99f; // 減衰率

    // 破片配列
    std::vector<Piece> mPieces;

};