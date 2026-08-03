#include "SelectScene.h"
#include "InputManager.h"
#include "Status.h"
#include "Master.h"
#include "Utility.h"
#include "Model.h"
#include "Camera.h"
#include "SkyBox.h"

SelectScene::SelectScene()
	: mfModelSpeed(0.1f)
	, mbDirection(false)
	, mfGraphAlpha(0.0f)
	, mTimer(0.0f)
{
	// フォント代入
	mnFontNormal = Master::mpFontManager->RetroFont();

	// 選択左上画像の読みこみ
	mnHandle_Select = LoadGraph("Resource/2D/Select.png");
	if (mnHandle_Select == -1)
	{
		printfDx("画像の読み込みに失敗");
	}

	// 選択していない左上画像の読みこみ
	mnHandle_NoSelect = LoadGraph("Resource/2D/NoSelect.png");
	if (mnHandle_NoSelect == -1)
	{
		printfDx("画像の読み込みに失敗");
	}

	// 基準を70にしてる
	SetFontSize(70);
}


SelectScene::~SelectScene()
{

}


void SelectScene::Initialize()
{
	Master::mpCamera->Reset(); // カメラ位置セット
	Master::mpCamera->SetEnableControl(false); // 選択画面ではカメラ操作を無効にする

	// SkyBox関係
	{
		std::string skyBox = "Resource/3D/SkyBox/SkyBox.x";

		// 遠いやつ
		SkyBox* pSkyBox = new SkyBox(skyBox, 0.00001f, 80);
		pSkyBox->SetScale(13.5f);
		pSkyBox->SetModelTexture("Resource/3D/SkyBox/StarFar.png"); // テクスチャかえる

		// 近いやつ
		SkyBox* pSkybox2 = new SkyBox(skyBox, 0.0002f, 255);
		pSkybox2->SetScale(5.0f);
		pSkybox2->SetModelTexture("Resource/3D/SkyBox/StarNear.png"); // テクスチャかえる
	}

	// 位置の修正した
	Master::mpCamera->SetPosition(VGet(-300.0f, 1000.0f, 0.0f));
	Master::mpCamera->SetLockAtPosition(VGet(-90.0f, 0.0f, 0.0f));


	// 初期モデル読み込み
	auto spec = Status::GetPlayerSpec(mnSelectNum);
	mpModel = std::make_unique<Model>(spec.modelName, VGet(0.0f, 0.0f, 0.0f));
	mModelPosY = MODEL_Y_POSITION; // 初期位置を1000にセット
}

void SelectScene::Update()
{
	// Enterが押されたらシーン移動
	if (InputManager::CheckDownKey(KEY_INPUT_RETURN))
	{
		// ステータスの作成
		// スペックに選択した数字を代入
		auto spec = Status::GetPlayerSpec(mnSelectNum);

		// Statusオブジェクトを動的に生成してMasterクラスの所有物（mpNextStatus）として管理させる。
		Master::mpNextStatus = std::make_unique<Status>(spec);

		mbDirection = true; // 演出ON
		mbSelect = true; // 選択をtrueにして文字表示
	}


	// 演出がONなら　決定されたらシーン簡易
	if (mbDirection)
	{
		mfGraphAlpha += 5.0f; //透過度を足す
		if (mfGraphAlpha >= 254.0f)
		{
			Master::mpSceneManager->SetNextScene(SceneManager::SCENE_TYPE::SCENE_3D);
		}
	}


	// ゲームシーンへの遷移演出がまだなら選択出来るようにする処理
	// フラグの管理を入れないと遷移中に変えることが出来てしまうため。
	if (!mbDirection)
	{
		// 下移動
		if (InputManager::CheckDownKey(KEY_INPUT_S))
		{
			mnSelectNum++;
			if (mnSelectNum > mnSelectMax)
			{
				mnSelectNum = 1;
			}
			auto spec = Status::GetPlayerSpec(mnSelectNum);
			mpModel = std::make_unique<Model>(spec.modelName, VGet(0.0f, 0.0f, 0.0f));
			mModelPosY = MODEL_Y_POSITION; // モデルが変わったらY座標を1000に戻す
		}
		// 上移動
		if (InputManager::CheckDownKey(KEY_INPUT_W))
		{
			mnSelectNum--;
			if (mnSelectNum < 1)
			{
				mnSelectNum = mnSelectMax;
			}
			auto spec = Status::GetPlayerSpec(mnSelectNum);
			mpModel = std::make_unique<Model>(spec.modelName, VGet(0.0f, 0.0f, 0.0f));
			mModelPosY = MODEL_Y_POSITION; // モデルが変わったらY座標を1000に戻す
		}

	}
	

	// モデルを目標位置に近づける
	// 今の位置　+= (目標位置 - 今の位) * 移動速さ
	mModelPosY += (MODEL_TARGET_POS - mModelPosY) * mfModelSpeed;

	if (mpModel)
	{
		// モデルを拡大
		mpModel->SetScale(1.5f);
		// 回転
		mpModel->SetRotation(VGet(-DX_PI_F / 2.0f, 0.0f, 0.0f));
		// 画面に表示する位置を指定
		mpModel->SetPosition(VGet(-300.0f, mModelPosY, 0.0f));
		mpModel->Update();
	}

	Scene::Update();
}

void SelectScene::Draw2D()
{
	int x = 300;
	int y = 100;
	int color = GetColor(255, 255, 255);

	if (!mbSelect)
	{
		DrawString(x, y + 100, "選択してください", color, TRUE);
	}
	else
	{
		DrawFormatString(x, y + 100, color, "選択済み %d""\nロード中", mnSelectNum);
	}

	auto spec = Status::GetPlayerSpec(mnSelectNum); // 選択中のステータスをゲット


	
	// 左の枠
	int SelectNow = mnSelectNum;
	for (int i = 1; i <= mnSelectMax; i++)
	{
		int posY = (y - 50) + (i - 1) * 76;
		int DarkColor = GetColor(150, 150, 150);

		// 使っている画像サイズ
		int imgW = 1024;
		int imgH = 512;

		// 描画位置
		int drawX;
		int drawY;

		float scale; // 画像でかすぎだから直す

		// 左上から中心に戻す
		int cx;
		int cy;

		// 左側の枠
		if (i == SelectNow) //選択しているやつだけ明るく　大きく
		{
			drawX = -450;
			drawY = posY - 230;
			cx = drawX + imgW / 2;
			cy = drawY + imgH / 2;
			scale = 0.50;

			DrawRotaGraph(
				cx,
				cy,
				scale,
				0.0,
				mnHandle_Select,
				TRUE
			);


			// 名前は少し大きめ
			DrawFormatStringToHandle(
				30,
				posY + 5,
				GetColor(255, 255, 255),
				mnFontNormal,
				"%s",
				spec.name.c_str()
			);
		}
		else // 選択していないやつ
		{
			drawX = -600;
			drawY = posY - 230;
			cx = drawX + imgW / 2;
			cy = drawY + imgH / 2;
			scale = 0.4f;


			// ここに画像を用意したい
			DrawRotaGraph(
				cx,
				cy,
				scale,
				0.0,
				mnHandle_NoSelect,
				TRUE
			);
		}
	}


	// 選択中の文字やゲージのやつ
	{   // 選択しているやつだけ表示

		int x = Utility::SCREEN_WIDTH / 2 + 100;
		int y = Utility::SCREEN_HEIGHT / 2 - 300;
		int color = GetColor(255, 255, 255);
		int stringY = y + 130;
		// ---------ステータス系--------------------------
		// HP
		DrawFormatStringToHandle(x, stringY, color, mnFontNormal,
			"体力          %d", (int)spec.hp);
		// 速さ
		DrawFormatStringToHandle(x, stringY + 60, color, mnFontNormal,
			"速さ          %d", (int)spec.speed);
		// 最大速度
		DrawFormatStringToHandle(x, stringY + 120, color, mnFontNormal,
			"最大速度      %d", (int)spec.maxSpeed);
		// 攻撃力
		DrawFormatStringToHandle(x, stringY + 180, color, mnFontNormal,
			"攻撃力        %d", (int)spec.normalAttack);
		// 追跡弾攻撃力
		DrawFormatStringToHandle(x, stringY + 240, color, mnFontNormal,
			"追跡弾攻撃力  %d", (int)spec.homingAttack);
		// スキルの説明
		DrawFormatStringToHandle(x, stringY + 300, color, mnFontNormal,
			"%s", spec.skillmemo.c_str());

		// 名前
		DrawFormatStringToHandle(
			x,
			y,
			GetColor(255, 255, 255),
			mnFontNormal,
			"%s",
			spec.name.c_str()
		);
	}


	if (mbDirection)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)mfGraphAlpha);
		DrawBox(0, 0, Utility::SCREEN_WIDTH, Utility::SCREEN_HEIGHT, GetColor(255,255,255), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
}


void SelectScene::Draw()
{
	// 3D用に設定
	SetUseZBufferFlag(TRUE);
	SetWriteZBufferFlag(TRUE);

	// モデルがあったらモデルを表示
	if (mpModel)
	{
		mpModel->Draw();
	}

	Scene::Draw();

	// ２D用に設定
	SetUseZBufferFlag(FALSE);
	SetWriteZBufferFlag(FALSE);

	Draw2D();
	
	// 3D用に設定
	SetUseZBufferFlag(TRUE);
	SetWriteZBufferFlag(TRUE);

}


void SelectScene::Finalize()
{
	// カメラ操作を有効に戻す
	Master::mpCamera->SetEnableControl(true);

	// グラフィックハンドルの削除
	DeleteGraph(mnHandle_Select);
	DeleteGraph(mnHandle_NoSelect);
}