#include "BreakRock.h"
#include <cmath>
#include "Master.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "ObjectManager.h"
#include "Rock3D.h"

BreakRock::BreakRock() {}

BreakRock::~BreakRock()
{
	Finalize(); // 解放
}

bool BreakRock::Load(int modelHandle)
{
	// 既にロード済みなら解放している
	Finalize();

	// ファイルから読み込まず、メモリ上のハンドルを複製する
	mShards = MV1DuplicateModel(modelHandle);

	if (mShards == -1)
	{
		return false;
	}

	// intactに設定されている位置とサイズを反映
	ApplyTransformToIntact();
	// 破片一覧
	BuildPieces();
	// 破壊前に戻す
	Reset();

	return true;
}


void BreakRock::Finalize()
{
	// 破片に複製したモデルを解放
	for (auto& p : mPieces)
	{
		if (p.model != -1)
		{
			MV1DeleteModel(p.model);
			p.model = -1;
		}
	}

	mPieces.clear();


	// shardsモデル解放
	if (mShards != -1)
	{
		MV1DeleteModel(mShards);
		mShards = -1;
	}

	// 状態を初期化
	mBroken = false;
}

void BreakRock::Reset()
{
	// 割れていない状態に戻す
	mBroken = false;
	
	// intactを位置サイズ合わせる
	for (auto& p : mPieces)
	{	
		p.active = false;			// 破片は描画/更新しない
		p.pos = mPos;				// 岩の位置
		p.vel = VGet(0, 0, 0);		// 速度0
		p.rot = VGet(0, 0, 0);		// 回転0
		p.rotVel = VGet(0, 0, 0);	// 回転速度

		if (p.model != -1)
		{
			// モデルに反映
			MV1SetPosition(p.model, p.pos);
			MV1SetRotationXYZ(p.model, p.rot);
			MV1SetScale(p.model, VGet(mScale, mScale, mScale));
		}
	}
}

float BreakRock::Rand01() const
{
	// 0.0から1.0の乱数
	return (float)(GetRand(10000) / 10000.0f);
}

float BreakRock::RandRange(float a, float b) const
{
	// aからbの範囲の乱数を作る
	return a + (b - a) * Rand01();
}

void BreakRock::ApplyTransformToIntact()
{
}

void BreakRock::ApplyScaleToPieces()
{
	// 岩の破片に大きさ反映したい時のため
	for (auto& p : mPieces)
	{
		if (p.model == -1) return;
		MV1SetScale(p.model, VGet(mScale, mScale, mScale));
	}
}

void BreakRock::BuildPieces()
{
	// 破片を作り直す

	// shardsモデルがないから作れん
	if (mShards == -1) return;

	// MV1のフレーム数
	const int frameNum = MV1GetFrameNum(mShards);

	// 破片フレームの列挙
	// 0番目はスキップ

	for (int fi = 0; fi < frameNum; ++fi)
	{
		if (fi == 0) continue;

		Piece p;
		p.frameIndex = fi; // この破片が担当するフレーム番号

		// ローカル座標を保存
		p.local = MV1GetFramePosition(mShards, fi);

		mPieces.push_back(p);
	}

	// 破片をp独立して動かすために各ピースごとに複製して持つ

	for (auto& p : mPieces)
	{
		p.model = MV1DuplicateModel(mShards);
		if (p.model != -1)
		{
			// 初期位置/サイズを合わせる
			MV1SetScale(p.model, VGet(mScale, mScale, mScale));
			MV1SetPosition(p.model, mPos);
		}
	}
}

void BreakRock::Break(const VECTOR& impactPos, float power)
{
	// 既に割れていたら二重で割れないように
	if (mBroken) return;

	// 割れたフラグ
	mBroken = true;


	// 各破片に初速を加える
	for (auto& p : mPieces)
	{
		if (p.model == -1) continue;

		// この破片の位置を取得
		VECTOR local = MV1GetFramePosition(p.model, p.frameIndex);

		// 破片の初期座標はその破片の位置
		p.pos = mPos;

		// 衝突点->破片方向のベクトル
		VECTOR dir = VGet(RandRange(-1.0f, 1.0f), RandRange(0.0f, 1.0f), RandRange(-1.0f, 1.0f));

		// 長さ（0に近いと正規化できないので例外処理）
		float len = VSize(dir);
		if (len < 0.0001f)
		{
			// ほぼ同一点だった場合はランダム方向に飛ばす
			dir = VGet(RandRange(-1.0f, 1.0f), RandRange(0.2f, 1.0f), RandRange(-1.0f, 1.0f));
			len = VSize(dir);
			if (len < 0.0001f) len = 1.0f;
		}

		// 正規化（長さ1の方向ベクトルにする）
		dir = VScale(dir, 1.0f / len);

		// 飛ぶ速度：powerにランダム係数をかける
		float sp = power * RandRange(0.6f, 1.25f);

		// 速度ベクトル
		p.vel = VScale(dir, sp);

		// 回転はゼロからスタート
		p.rot = VGet(0, 0, 0);

		// 回転速度をランダムで与える（ラジアン想定）
		p.rotVel = VGet(
			RandRange(-7.0f, 7.0f),
			RandRange(-7.0f, 7.0f),
			RandRange(-7.0f, 7.0f)
		);

		p.active = true;



		// モデルに反映（いきなり描画しても正しい位置）
		MV1SetPosition(p.model, p.pos);
		MV1SetRotationXYZ(p.model, p.rot);
		MV1SetScale(p.model, VGet(mScale, mScale, mScale));
	}
}

void BreakRock::Update(float dt, float sp)
{

	// 割れていないときは intact を位置に追従させるだけ
	if (!mBroken)
	{
		ApplyTransformToIntact();
		return;
	}

	// 割れている時：破片を物理更新
	for (auto& p : mPieces)
	{
		// activeじゃない破片は更新しない
		if (!p.active || p.model == -1) continue;

		// 重力（加速度）を速度へ加える: v += g * dt
		p.vel = VAdd(p.vel, VScale(mGravity, dt));

		// 減衰（空気抵抗っぽい）: v *= damping
		// ※mDampingが 0.98 みたいな値だと徐々に遅くなる
		p.vel = VScale(p.vel, mDamping);

		// 位置更新: pos += v * dt
		p.pos = VAdd(p.pos, VScale(p.vel, dt));

		//--回転しながら壊れるなら下記処理使う
		// 回転更新: rot += rotVel * dt
		 p.rot = VAdd(p.rot, VScale(p.rotVel, dt));
		 //回転速度も減衰させる
		 p.rotVel = VScale(p.rotVel, mDamping);

		 p.pos.z -= sp;

		// モデルへ反映
		MV1SetPosition(p.model, p.pos);
		//MV1SetRotationXYZ(p.model, p.rot);
		MV1SetScale(p.model, VGet(mScale, mScale, mScale));
	}
}

void BreakRock::Draw() const
{
	// 割れている場合：破片だけ描画
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, mShardAlpha);

	for (const auto& p : mPieces)
	{
		if (!p.active || p.model == -1) continue;

		// 「この破片フレームだけ描画」する
		// ※モデルは複製しているけど、フレーム単位で描画して破片っぽく見せる方式
		MV1DrawFrame(p.model, p.frameIndex);
	}

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}