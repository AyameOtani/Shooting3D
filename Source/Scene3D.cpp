#include "Scene3D.h"
#include "Wall.h"
#include "Master.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "ObjectManager.h"
#include "Stage.h"
#include "SkyBox.h"
#include <memory>
#include "Utility.h"
#include "ModelUtility.h"
#include "Player3D.h"
#include "Enemy3D.h"
#include "Rock3D.h"

Scene3D::Scene3D()
	:Scene()
{
	// 文字の大きさ関係
	mnFontBig = CreateFontToHandle(
		NULL,
		300, // すごく大きく
		3
	);

	mnFontNormal = CreateFontToHandle(
		NULL,
		45,   // 今のフォントサイズ
		3
	);

	mnFontSmall = CreateFontToHandle(
		NULL,
		35,   // 小さめのフォントサイズ
		3
	);
}

Scene3D::~Scene3D()
{

}

void Scene3D::Initialize() // 初期化 
{
	// カメラのリセットと背景色のリセット
	Master::mpCamera->Reset();
	Master::mpCamera->DefaultBackColor();


	// SkyBox関係
	{
		std::string skyBox = "Resource/3D/SkyBox/SkyBox.x";

		// 遠いやつ
		SkyBox* pSkyBox = new SkyBox(skyBox, 0.00001f, 80);
		pSkyBox->SetScale(13.5f);
		pSkyBox->SetModelTexture("Resource/3D/SkyBox/StarFar.png"); // テクスチャかえる

		// 中くらい
		SkyBox* pSkyBox1 = new SkyBox(skyBox, 0.00008f, 150);
		pSkyBox1->SetScale(10.5f);
		pSkyBox1->SetModelTexture("Resource/3D/SkyBox/StarMedium.png"); // テクスチャかえる

		// 近いやつ
		SkyBox* pSkybox2 = new SkyBox(skyBox, 0.0002f, 255);
		pSkybox2->SetScale(5.0f);
		pSkybox2->SetModelTexture("Resource/3D/SkyBox/StarNear.png"); // テクスチャかえる
	}



	// プレイヤーの生成
	auto player = new Player3D(VGet(0.0f, 0.0f, 0.0f),
		Master::mpNextStatus.get(), false);





	// 敵の生成
	auto pEnemy1 = new Enemy3D("Resource/3D/Player/Red.mqo", VGet(0, 0, 800.0f), Enemy3D::EnemyType::TYPE_NORMAL, mnEnemyCount++, false);
	pEnemy1->SetPattern(Enemy3D::SpawnSide::RIGHT, 0.0f); // 右敵

	auto pEnemy2 = new Enemy3D("Resource/3D/Player/Red.mqo", VGet(0, 0, 800.0f), Enemy3D::EnemyType::TYPE_NORMAL, mnEnemyCount++, false);
	pEnemy2->SetPattern(Enemy3D::SpawnSide::LEFT, 0.0f); // 左敵


	// 岩のモデル関係
	mpRockModel = new Model("Resource/3D/Rock/Rock.mqo", VGet(0,0,0));
	mpBreakRockModel = new Model("Resource/3D/Rock/BreakRock.mqo", VGet(0, 0, 0));	//割れた

	Master::mpSoundManager->PlayBGM(SoundManager::BGM_GAME, true, 0); // BGM
	mbDrawFlag = true; // 敵が出たかのフラグをONにする
}


void Scene3D::Update() // 更新
{
	// player情報の取得
	auto pPlayerList =Master::mpSceneManager->GetCurrentScene()->GetObjectManager()->GetObject3DListByTag(Object3D::T_Player3D);
	// Enemy情報の取得
	auto pEnemyList =Master::mpSceneManager->GetCurrentScene()->GetObjectManager()->GetObject3DListByTag(Object3D::T_Enemy3D);


	// 敗北と勝利
	if (pPlayerList.empty())
	{
		Master::mpSceneManager->SetNextScene(SceneManager::SCENE_TYPE::LOSERESULT_3D);
		return;
	}
	if (mbLastEnemys && pEnemyList.empty())
	{
		Master::mpSceneManager->SetNextScene(SceneManager::SCENE_TYPE::WINRESULT_3D);
		return;
	}


	// デバック用
	if (InputManager::CheckDownKey(KEY_INPUT_1))
	{
		Master::mpSceneManager->SetNextScene(SceneManager::SCENE_TYPE::LOSERESULT_3D);
	}
	if (InputManager::CheckDownKey(KEY_INPUT_2))
	{
		Master::mpSceneManager->SetNextScene(SceneManager::SCENE_TYPE::WINRESULT_3D);
	}


	// 敵の出現管理
	// 敵が全滅している時だけ次のWaveの判定をする
	if (pEnemyList.empty())
	{
		switch (mnWaveStep)
		{
		case 0:
			if (mfStageDistance >= 3000.0f)
			{
				SpawnWave(Enemy3D::SpawnSide::RIGHT, 3, 1);
				mnWaveStep++; // 次のWaveへ
			}
			break;

		case 1:
			if (mfStageDistance >= 6000.0f)
			{
				SpawnWave(Enemy3D::SpawnSide::LEFT, 4, 2);
				mnWaveStep++;
			}
			break;

		case 2:
			if (mfStageDistance >= 9000.0f)
			{
				SpawnWave(Enemy3D::SpawnSide::TOP, 5, 3);
				mnWaveStep++;
			}
			break;

		case 3: // 11000m Wave
			if (mfStageDistance >= 12000.0f)
			{
				SpawnWave(Enemy3D::SpawnSide::TOP, 5, 4);
				mbLastEnemys = true; // これで最後の通知
				mnWaveStep++;
			}
			break;
		}
	}
	

	// 岩の生成  敵がいない時だけ生成する
	if (mfStageDistance >= mfRockTime && pEnemyList.empty() && !mbLastEnemys)
	{
		// 位置XYランダム
		float RockX = (float)GetRand(700) - 350.0f;
		float RockY = (float)GetRand(700) - 350.0f;
		// 岩の生成
		auto rock = new Rock3D(mpRockModel, mpBreakRockModel, VGet(RockX, RockY, 4000.0f));
		rock->Initialize(); // 初期化呼ぶ
		mfRockTime = mfStageDistance + GetRand(200) + 50; // 50から250にしてランダムに
	}


	// ここで敵をだしたりする
	mfStageDistance += STAGE_SPEED;
	Scene::Update();
}

void Scene3D::Draw() // 描画
{
	DrawFormatString(20, 20, GetColor(255, 255, 0), "進行距離 %f", mfStageDistance);
	DrawFormatString(20, 40, GetColor(0, 255, 255), "累計出現数: %d 体", mnEnemyCount);

	Scene::Draw();
}

void Scene3D::Finalize() // 終了処理
{
	// 岩を解放
	if (mpRockModel != nullptr)
	{
		delete mpRockModel;
	}
	if (mpBreakRockModel != nullptr)
	{
		delete mpBreakRockModel;
	}


	//  フォントハンドルの削除
	DeleteFontToHandle(mnFontBig);
	DeleteFontToHandle(mnFontNormal);
	DeleteFontToHandle(mnFontSmall);
}

// 敵の出現ずらすやつ
void Scene3D::SpawnWave(Enemy3D::SpawnSide side, int count, int type)
{
	// 設定した数まで生成繰り返す
	for (int i = 0; i < count; i++)
	{
		// タイプをバラけさせる今は3分の1
		Enemy3D::EnemyType enemyType;

		switch (type)
		{
		case 1:	// 左右移動
			enemyType = Enemy3D::EnemyType::TYPE_NORMAL;
			break;

		case 2: // 上下移動
			enemyType = Enemy3D::EnemyType::TYPE_SPEED;
			break;

		case 3:	 // ランダム
			enemyType = Enemy3D::EnemyType::TYPE_HEAVY;
			break;

		case 4:	// ランダム
			enemyType = (Enemy3D::EnemyType)(i % 3);
			break;

		default:
			enemyType = Enemy3D::EnemyType::TYPE_NORMAL;
			break;
		}

		// 生成	とりま位置は000
		auto enemy = new Enemy3D("Resource/3D/Player/Red.mqo", VGet(0, 0, 0), enemyType, mnEnemyCount++);

		//  散らすための計算
		// (i - 2.0f) * 400.0f だと 5体ので [-800, -400, 0, 400, 800] になるで
		// (i - (count / 2.0f))...全個数 / 2で真ん中を0にしている
		float offset = (i - (count / 2.0f)) * 400.0f;

		// 配置ルールセット
		enemy->SetPattern(side, offset);
	}
}