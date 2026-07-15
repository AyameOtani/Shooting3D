#pragma once
#include "DxLib.h"

// フォルダにあるフォントを適用させためのもの

class FontManager
{
public:

	FontManager();
	~FontManager();
	void Initialize();
	void Finalize();

	// フォントの設定
	int RetroFont() const { return mnRetroFont; }

private:
	// フォントのメンバ
	int	mnRetroFont; // デラマルフォント
};