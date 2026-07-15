#pragma once

#include "SceneManager.h"
#include "SoundManager.h"
#include "Camera.h"
#include "FontManager.h"
#include "Status.h"

// リソースやポインタ系
#include <memory> // unique_ptr を使うため
#include "ResourceManager.h" // 追加



//
// ゲーム内でひとつだけしか存在しないものを管理するクラス
// 例えば、
// SceneManager 　　だったり、（シーン管理クラス）
// SoundManager 　　だったり、（サウンド管理クラス）
// ResourceManager  だったり、（リソース（画像やモデル）管理クラス）
// を、どこからでも参照できるようにしておきたいので
// こういったクラスを作る
// 

class Master
{
public:  // 公開しているから使える　クラスをまたいで使える

	static SoundManager* mpSoundManager;	   // サウンドマネージャーのポインタ
	static SceneManager* mpSceneManager;	   // シーンマネージャーのポインタ
	static ResourceManager* mpResourceManager; // リソースマネージャーのポインタ
	static Camera* mpCamera;                   // カメラのポインタ
	static FontManager* mpFontManager;         // フォントマネージャーのポインタ

	// --------------3D用-----------------------
	static std::unique_ptr<Status> mpNextStatus;

};



// ---------------メモ---------------------------------------------1231-------------------
// unique_ptr ... uniqueが勇逸のという意味  +   ptrはポインタという意味らしい
// 所有者が必ず一つだけのポインタという意味
// メリットは、同じものを複数の場所がdeleteしないから前みたいに二重にならぬ
// 所有者がMasterに固定される方メモリリークが防げる
// すこーぷを抜けると自動でdeleteだから安全 

// 前は、MasterもUnitもdeleteしていたから持ってるのが２人だたから二重に消えていたからエラー
