#pragma once

#include "DxLib.h"
#include "Object3D.h"
#include <vector>

class Effect : public Object3D
{
public:

	// エフェクトの種類
	enum EffectType
	{
		TYPE_NONE = 0, // タイプなし

		TYPE_DAMAGE,  // ダメージタイプ
		TYPE_DEATH,   // ダメージ2個目

		TYPE_MAX,     // 最大タイプ
	};


	// エフェクトの設定とかを個別でやりたい
	struct EffectSetting
	{
		EffectType type = TYPE_NONE; // エフェクトのタイプ
		int particlNum = 128;  // パーティクルの最大数
		int r = 255;		   // 色R
		int g = 13;			   // 色G
		int b = 13;			   // 色B

		// 割るやつ
		float size = 100;			// 大きさ
		float visibleTime = 80;  // 表示時間
		float speed = 100;		// 速度
	};


	// パーティクル構造体
	// 粒１つ１つのデータ
	struct ParticleInfo
	{
		VECTOR pos;        // 座標
		VECTOR dir;        // 進行方向
		float speed;       // 速度
		float size;        // 大きさ
		float visibleTime; // 表示時間
		float alpha;       // 不透明度
		float angle;	   // 角度
	};


	// エフェクト情報
	// 粒をまとめて１つのエフェクトとしてのデータ
	struct EffectInfo
	{
		COLOR_U8 color; // エフェクト色
		std::vector<ParticleInfo> particle; // パーティクル情報
	};


	// エフェクトの詳細を返すやつ
	static EffectSetting GetEffectDate(EffectType effect);

	// 透明のモデルにするため
	DrawType GetDrawType() const { return DrawType::ToumeiModel; }


public:
	Effect(VECTOR initPos, std::string filename, Effect::EffectType type);
	~Effect();

	void Update() override;
	void Draw() override;

private:

	int mnGraphHandle;     // 画像ハンドル
	EffectInfo* mpEffect;  // エフェクトデータ 構造体をポインタとして持つ

	const int SPEED_RAND_MAX = 100;      // 速度の最大乱数値
	const int SPEED_RAND_MIN = 50;       // 速度の最小乱数値

	const int SIZE_RAND_MAX = 800;       // サイズの最大乱数値
	const int SIZE_RAND_MIN = 400;       // サイズの最小乱数値

	const int VISIBLE_TIME_RAND_MAX = 100; // 表示時間の最大乱数値
	const int VISIBLE_TIME_RAND_MIN = 50;  // 表示時間の最小乱数値


private:
	// スキル種類
	EffectType mType = EffectType::TYPE_NONE;

	// 代入するやつ
	int mnParticlNum; // パーティクル最大数
	// 色
	int mnR;
	int mnG;
	int mnB;
	float mnSize;		 // 大きさ   100
	float mnVisibleTime; // 表示時間 100
	float mfSpeed;		 // 速さ	  80

};
