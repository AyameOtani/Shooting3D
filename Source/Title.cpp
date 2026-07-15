#include "InputManager.h"
#include "Master.h"
#include "Title.h"
#include "Utility.h"
#include "SkyBox.h"
#include "Status.h"
#include "Player3D.h"

Title::Title()
{
	SetFontSize(100);
}

Title::~Title()
{

}

void Title::Initialize()
{
	Master::mpCamera->Reset();

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


void Title::Update()
{

	if (InputManager::CheckDownKey(KEY_INPUT_RETURN))
	{
		Master::mpSceneManager->SetNextScene(SceneManager::SCENE_TYPE::SELECT_3D);
	}

	Scene::Update();
}

void Title::Draw()
{
	int x = Utility::SCREEN_WIDTH / 2 - 600;
	int y = Utility::SCREEN_HEIGHT / 2;
	int dist = 4;
	int color = GetColor(255, 255, 255);

	//二十文字
	DrawString(x + dist, y + dist,
		"スペースシューティング 3D"
		, GetColor(0, 180, 220), TRUE);

	DrawString(x, y,
		"スペースシューティング 3D"
		, color, TRUE);

	Scene::Draw();
}

void Title::Finalize()
{

}