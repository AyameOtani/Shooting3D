#include "FontManager.h"

FontManager::FontManager()
	: mnRetroFont(-1)
{

}

FontManager::~FontManager()
{

}

// フォントの初期化　Mainで呼ぶ
void FontManager::Initialize()
{
	// フォントのダウンロード
	AddFontResourceEx(
		"Resource/Font/tetsubin-gothic/tetsubin-gothic.ttf",
		FR_PRIVATE,
		NULL
	);

	// フォントの作成
	{
		mnRetroFont = CreateFontToHandle(
			"07鉄瓶ゴシック",
			40,
			5
		);
	}
}

void FontManager::Finalize()
{
	DeleteFontToHandle(mnRetroFont);
}