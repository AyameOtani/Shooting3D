#pragma once

#include "BaseBullet3D.h"

class Model;
class Unit;

class EnemyBullet3D : public BaseBullet3D
{
public:
	EnemyBullet3D(std::string filename, VECTOR initPos, VECTOR Direction, Unit* owner, float attack);
	~EnemyBullet3D();

};