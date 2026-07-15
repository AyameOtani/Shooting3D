#include "HpBar.h"
#include "Player3D.h"

#include "Utility.h"
#include "Master.h"
#include "Scene.h"
#include "ObjectManager.h"

HpBar::HpBar(Player3D* player)
{
	mpPlayer = player;

	lateHp = mpPlayer->GetMaxHp();
}

HpBar::~HpBar()
{

}

void HpBar::UpdateBar()
{
	float diff = lateHp - mpPlayer->GetHp();
	if (diff > 0.0f) { lateHp -= diff * lateDist; }
	else { lateHp = mpPlayer->GetHp(); }
}

// 平行四辺形のHPバー
void HpBar::DrawBar()
{
	// プレイヤー取得 シーンマネージャー経由でプレイヤーオブジェクトを取得
	auto pPlayerList = Master::mpSceneManager->GetCurrentScene()
		->GetObjectManager()->GetObject3DListByTag(Object3D::T_Player3D);
	if (pPlayerList.empty()) return;

	for (auto obj : pPlayerList)
	{
		Player3D* pPlayer = dynamic_cast<Player3D*>(obj);

		// バーの配置とサイズの定義
		const int x = Utility::SCREEN_WIDTH - 1870;		// 位置X
		const int y = Utility::SCREEN_HEIGHT - 80;		// 位置Y
		const int width = Utility::SCREEN_WIDTH / 4;	// 幅
		const int height = Utility::SCREEN_HEIGHT / 42; // 高さ

		// 傾き量の設定
		const int slantOuter = 10;

		// Hpバーの詳細設定
		float rate = mpPlayer->GetHp() / mpPlayer->GetMaxHp(); // 割合
		if (rate <= 0.0f) rate = 0.0f;
		if (rate >= 1.0f) rate = 1.0f;

		// 遅延HPバーの詳細設定
		float lateRate = lateHp / mpPlayer->GetMaxHp();
		if (lateRate <= 0.0f) lateRate = 0.0f;
		if (lateRate >= 1.0f) lateRate = 1.0f;

		// 今のHPバーの横幅
		int currentWidth = (int)(width * rate);
		// 遅延HPバーの横幅
		int lateWidth = (int)(width * lateRate);


		// 背景描画 全体のベースとなる平行四辺形
		DrawQuadrangle(x + slantOuter, y, x + width + slantOuter, y, x + width - slantOuter, y + height, x - slantOuter, y + height, GetColor(70, 70, 70), TRUE);

		// 遅延HPバーの部分
		DrawQuadrangle(x + slantOuter, y, x + lateWidth + slantOuter, y, x + lateWidth - slantOuter, y + height, x - slantOuter, y + height, GetColor(255, 120, 20), TRUE);
		
		// HPバーの部分
		DrawQuadrangle(x + slantOuter, y, x + currentWidth + slantOuter, y, x + currentWidth - slantOuter, y + height, x - slantOuter, y + height, GetColor(77, 208, 211), TRUE);
		
		// 全体の外枠を描画
		DrawQuadrangle(x + slantOuter, y, x + width + slantOuter, y, x + width - slantOuter, y + height, x - slantOuter, y + height, GetColor(255, 255, 255), FALSE);

		// デバッグ表示 今のHP
		DrawFormatString((x + (width / 2)) - 30, y + 5, GetColor(255, 255, 255), "HP %d", (int)mpPlayer->GetHp());
	}
}