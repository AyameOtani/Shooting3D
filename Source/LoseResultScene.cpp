#include "LoseResultScene.h"
#include "InputManager.h"
#include "Master.h"
#include "Utility.h"
#include "SkyBox.h"

LoseResultScene::LoseResultScene()
{
	SetFontSize(100);
}

LoseResultScene::~LoseResultScene()
{

}

void LoseResultScene::Initialize()
{
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
}

void LoseResultScene::Update()
{
	if (InputManager::CheckDownKey(KEY_INPUT_RETURN))
	{
		Master::mpSceneManager->SetNextScene(SceneManager::SCENE_TYPE::TITLE_3D);
	}
	Scene::Update();
}

void LoseResultScene::Draw()
{
	DrawString(Utility::SCREEN_WIDTH / 2 - 100, Utility::SCREEN_HEIGHT / 2, "敗北", GetColor(255, 255, 255));
	Scene::Draw();
}

void LoseResultScene::Finalize()
{

}



