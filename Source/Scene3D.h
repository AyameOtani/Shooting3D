#pragma once
#include "Scene.h" // シーン参照したいからインクルード
#include "DxLib.h"
#include <string>
#include "Enemy3D.h"


class Baria;
class Object3D;
class Model;
class Scene3D : public Scene
{

private:
	float mfRockTime = 20.0f; // 岩が出現し始めるやつ

public:
	// コンストラクタ
	Scene3D();
	// デストラクタ
	virtual ~Scene3D();
	// 初期化
	virtual void Initialize() override;
	// 更新
	virtual void Update() override;
	// 描画
	virtual void Draw() override;
	// 終了処理
	virtual void Finalize() override;


	// 敵をまとめて出す関数
	void SpawnWave(Enemy3D::SpawnSide side, int count, int type);

private:
	int mnEnemyCount = 0;    // 敵に振るIDのカウンター
	float mfNextDist = 2000.0f; // 最初は3000mで出すようにセット

	bool mbLastEnemys = false; // 9000mの敵を出したか
	bool mbAllEnemyEmpty = false;    // 9000の敵が滅したか

private:

	Model* mpRockModel = nullptr;		// 岩のモデル
	Model* mpBreakRockModel = nullptr;	// 割れた岩w
	// 追加
	bool mbDrawFlag = false; // オブジェクトが配置されたか (敵が出てくる前のシーン還移を防ぐため)

	int mnFontBig; // 大きいフォント　カウントで使う
	int mnFontNormal; // ふつうのフォント
	int mnFontSmall; // 小さいやつ

	float mfStageDistance = 0.0f; // ステージ走行距離
	const float GOAL_DIST = 1000000.0f; // ゴールのＺ座標
	const float STAGE_SPEED = 1.0f;

	std::string rockname = "Resource/3D/Rock/Rock.mqo";
	int mnWaveStep = 0;	  // ステップみたいな
};
	 


