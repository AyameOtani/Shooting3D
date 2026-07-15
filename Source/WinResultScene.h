#pragma once
#include "Scene.h"
#include "DxLib.h"

class WinResultScene : public Scene
{
public:
	WinResultScene();
	virtual ~WinResultScene();

	virtual void Initialize() override;

	virtual void Update() override;
	 
	virtual void Draw() override;

	virtual void Finalize() override;

private:


};
