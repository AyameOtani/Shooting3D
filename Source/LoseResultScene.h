#pragma once
#include "Scene.h"
#include "DxLib.h"

class LoseResultScene : public Scene
{
public:
	LoseResultScene();
	virtual ~LoseResultScene();

	virtual void Initialize() override;

	virtual void Update() override;

	virtual void Draw() override;

	virtual void Finalize() override;

private:


};
