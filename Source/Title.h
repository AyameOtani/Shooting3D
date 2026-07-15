#pragma once

#include "Scene.h"
#include <DxLib.h>

class Title : public Scene
{

public:
	Title();
	~Title();

	void Initialize() override;
	void Update() override;
	void Draw() override;

	void Finalize() override;

private:

};
