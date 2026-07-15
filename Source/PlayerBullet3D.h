#pragma once

#include "BaseBullet3D.h"

class Model;
class Unit;

class PlayerBullet3D : public BaseBullet3D
{
public:
	PlayerBullet3D(std::string filename, VECTOR initPos, VECTOR Direction, Unit* owner, float attack);
	~PlayerBullet3D();

};

