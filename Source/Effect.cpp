#include "Effect.h"
#include "Master.h"

Effect::Effect(VECTOR initPos, std::string filename, Effect::EffectType type)
	: Object3D(initPos)
{
	// 設定を取得
	EffectSetting effect = GetEffectDate(type);
	mType = effect.type;
	mnParticlNum = effect.particlNum;
	mnR = effect.r;
	mnG = effect.g;
	mnB = effect.b;
	mnSize = effect.size;
	mnVisibleTime = effect.visibleTime;
	mfSpeed = effect.speed;

	mpEffect = new EffectInfo();

	// パーティクルの最大数
	mpEffect->particle.resize(mnParticlNum);

	// 色
	mpEffect->color = GetColorU8(mnR, mnG, mnB, 255);

	for (int i = 0; i < mnParticlNum; i++) // 回数を回してほしい
	{
		// 初期座標
		mpEffect->particle[i].pos = initPos;

		// 進行方向
		mpEffect->particle[i].dir.x = ((float)GetRand(200) - 100.0f) / 50.0f;
		mpEffect->particle[i].dir.y = ((float)GetRand(200) - 100.0f) / 50.0f;
		mpEffect->particle[i].dir.z = ((float)GetRand(200) - 100.0f) / 50.0f;
	
		// 速度
		mpEffect->particle[i].speed = ((float)GetRand(SPEED_RAND_MAX) + SPEED_RAND_MIN) / mfSpeed;

		// 不透明度 
		mpEffect->particle[i].alpha = 1.0f;

		// 大きさ
		mpEffect->particle[i].size = ((float)GetRand(SIZE_RAND_MAX) + SIZE_RAND_MIN) / mnSize;

		// 表示時間
		mpEffect->particle[i].visibleTime = ((float)GetRand(VISIBLE_TIME_RAND_MAX) + VISIBLE_TIME_RAND_MIN) / mnVisibleTime;

		// 角度
		mpEffect->particle[i].angle = GetRand(360) * DX_PI_F / 180.0f;
	}

	// 画像の読み込み
	// mnGraphHandle = LoadGraph(filename.c_str()); // 変更
	mnGraphHandle = Master::mpResourceManager->LoadGraphics(filename);
}


Effect::~Effect()
{
	delete mpEffect;

	//DeleteGraph(mnGraphHandle);// リソースマネー入れたから消す
}

void Effect::Update()
{
	bool isEnd = true; // パーティクルが全消えたかどうか
	float stepTime = 1.0f / 60.0f; // １フレームにどれだけ進めるかの基本値

	for (int i = 0; i < mnParticlNum; i++)
	{
		// アルファ値が０以下になったらそのパーティクルは処理しない
		if (mpEffect->particle[i].alpha <= 0.0f)
		{
			continue;
		}

		// ここまで来るということは少なくともひとつはパーティクルが存在しているので終了させない
		isEnd = false;

		// 速度が残っている場合
		if (mpEffect->particle[i].speed > 0.0f)
		{
			// 座標を移動させる
			mpEffect->particle[i].pos = VAdd(mpEffect->particle[i].pos,
				VScale(mpEffect->particle[i].dir,
					mpEffect->particle[i].speed));

			// 速度を徐々に落としていく
			mpEffect->particle[i].speed -= 2.0f * stepTime;

			// 速度が０以下になった場合
			if (mpEffect->particle[i].speed <= 0.0f)
			{
				// 表示時間を強制的に０にする
				mpEffect->particle[i].visibleTime = 0.0f;
			}
		}

		// 表示時間が残っている場合
		if (mpEffect->particle[i].visibleTime > 0.0f)
		{
			// 表示時間を減らしていく
			mpEffect->particle[i].visibleTime -= 0.75f * stepTime;
		}
		else
		{
			// アルファ値を落としていく(徐々に透明にしていく)
			mpEffect->particle[i].alpha -= 12.0f * stepTime;
		}
		//mpEffect->particle[i].dir.y -= 0.1f; // 重力

	}
	// パーティクルが１つも存在していないのであれば削除する
	if (isEnd)
	{
		SetDeleteFlag(true);
	}
}

void Effect::Draw()
{
	// Zバッファを使用する設定にする
	SetUseZBufferFlag(TRUE);

	// Zバッファへの書き込みはしない
	SetWriteZBufferFlag(FALSE);

	// 描画輝度の設定
	SetDrawBright(mpEffect->color.r, mpEffect->color.g, mpEffect->color.b);

	// 描画していく
	// DX_BLENDMODE_INVSRC    描画元の色を反転する
	for (int i = 0; i < mnParticlNum; i++)
	{
		// アルファ値が０以下なら設定しない
		if (mpEffect->particle[i].alpha <= 0.0f)
		{
			continue;
		}

		// 反転ブレンドを行いつつパーティクルを描画する
		// 描画の色が反転する
		// ビルボード　　カメラの方向に常に向いてくれる３Dポリゴン ★重要
		SetDrawBlendMode(DX_BLENDMODE_INVSRC, (int)mpEffect->particle[i].alpha * 255);
		DrawBillboard3D(
			mpEffect->particle[i].pos, 0.5f, 0.5f,
			mpEffect->particle[i].size * mpEffect->particle[i].alpha,
			mpEffect->particle[i].angle,
			mnGraphHandle,
			true
		);
	}


	// DX_BLENDMODE_ADD_X4  になっている
	// 加算ブレンドの描画元の輝度を最大４倍にできるモード
	for (int i = 0; i < mnParticlNum; i++)
	{
		// アルファ値が０以下なら設定しない
		if (mpEffect->particle[i].alpha <= 0.0f)
		{
			continue;
		}

		// 加算ブレンドを行いつつパーティクルを描画する
		// 加算合成をしている ポリゴンが重なっている所が白に近づく描画方法
		// 輝度は最大４倍になる
		// ビルボード　　カメラの方向に常に向いてくれる３Dポリゴン ★重要
		SetDrawBlendMode(DX_BLENDMODE_ADD_X4, (int)mpEffect->particle[i].alpha * 255);
		
		DrawBillboard3D(
			mpEffect->particle[i].pos, 0.5f, 0.5f,
			mpEffect->particle[i].size * mpEffect->particle[i].alpha,
			mpEffect->particle[i].angle,
			mnGraphHandle,
			true
		);
	}


	// 変えたものを元に戻す処理をする
	// ブレンドモードを元に戻す
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

	// 描画輝度を元に戻す
	SetDrawBright(255, 255, 255);

	// Zバッファを使用する
	SetUseZBufferFlag(TRUE);

	// Zバッファへの書き込みをする
	SetWriteZBufferFlag(TRUE);
}

Effect::EffectSetting Effect::GetEffectDate(EffectType effect)
{
	// エフェクトをタイプごとに分けている
	switch (effect)
	{
	case TYPE_DAMAGE:
		return
		{
			EffectType::TYPE_DAMAGE, // タイプ
			128,		// パーティクル最大数
			0, 255, 255,  // RGB
			40,		// サイズ割るfloat
			80,			// 表示時間割るfloat
			70			// 速度割るfloat
		};

	case TYPE_DEATH:
		return
		{
			EffectType::TYPE_DEATH, // タイプ
			128,						  // パーティクル最大数
			255, 20, 0,			  // RGB
			100,						  // サイズ割るfloat
			80,						  // 表示時間割るfloat
			60						  // 速度割るfloat
		};

	default:
		return
		{
			EffectType::TYPE_DAMAGE, // タイプ
			32,						 // パーティクル最大数
			0, 0, 0,				 // RGB
			50,						 // サイズ割るfloat
			80,						 // 表示時間割るfloat
			80						 // 速度割るfloat
		};

	}
}