#include "SkyBox.h"
#include "Model.h"
#include "Master.h"
#include "Scene.h"
#include "ObjectManager.h"
#include "DxLib.h"
#include "Player3D.h"


// コンストラクタ
SkyBox::SkyBox(std::string filename, float speed, float alpha)
	: Object3D(VGet(0.0f, 0.0f, 0.0f))
{
	// スカイボックスモデルの生成
	// note: スカイボックスの座標は、基本的には原点。
	//       ものによっては、座標を常にプレイヤーの座標にすることで、空が見切れないようにすることもある。
	mpModel = new Model(filename, VGet(0.0f, 0.0f, 0.0f));

	mfRotateSpeed = speed;
	mfAlpha = alpha;
}


// デストラクタ
SkyBox::~SkyBox()
{
	// モデルクラスの破棄
	if (mpModel != nullptr)
	{
		delete mpModel;
	}
}

// 更新処理
void SkyBox::Update()
{
	if (mpModel != nullptr)
	{
		mpModel->Update();


		// モデルをX軸で90度
		mvRotation.x = DX_PI_F / 2.0f;
		mpModel->SetRotation(mvRotation);

		int handle = mpModel->GetHandle();
		// 繰り返し設定  テクスチャのアドレスモードを設定する
		 MV1SetTextureAddressMode(handle, -1, DX_TEXADDRESS_WRAP, DX_TEXADDRESS_WRAP);

		// 爆速スクロール
		mfScroll -= mfRotateSpeed; // 速さ

		int frameNum = MV1GetFrameNum(handle);
		for (int i = 0; i < frameNum; i++)
		{
			// テクスチャの座標を変更する
			MV1SetFrameTextureAddressTransform(
				handle,        // ハンドル
				i,             // フレーム番号
				0.0f,          // 横方向のズレ0
				mfScroll,	   // 縦方向のズレ  奥から手前
				1.0f,          // 横の拡大率
				1.0f,          // 縦の拡大率
				0.5f,          // 回転中心U 0.5f はテクスチャの真ん中
				0.5f,          // 回転中心V 0.5f はテクスチャの真ん中
				0.0f           // テクスチャの回転角度
			);
		}
	}

}



// 描画処理
void SkyBox::Draw()
{
	if (mpModel != nullptr)
	{
		int handle = mpModel->GetHandle();

		// 0.0から1.0じゃないと使えん
		float alpha = mfAlpha / 255.0f;
		MV1SetOpacityRate(handle, alpha);

		// 一時的にライトの影響をOFFにして描画する
		SetUseLighting(FALSE);
		mpModel->Draw();
		SetUseLighting(TRUE);

		// 一応戻す
		 MV1SetOpacityRate(handle, 1.0f);
	}

}


// 拡大値（スケール値）の設定（Modelクラスへの橋渡し）
void SkyBox::SetScale(float scale)
{
	if (mpModel != nullptr)
	{
      mpModel->SetScale(scale);
	}

	//if (mnModelHandle != -1)
	//{
	//	// 直接スケールを設定
	//	MV1SetScale(mnModelHandle, VGet(scale, scale, scale));
	//}
}

// モデルのテクスチャ変更（Modelクラスへの橋渡し）
void SkyBox::SetModelTexture(std::string filename, int index)
{
	if (mpModel != nullptr)
	{
		mpModel->SetTexture(filename, index);
	}
}