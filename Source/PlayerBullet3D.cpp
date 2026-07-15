#include "PlayerBullet3D.h"
#include "Model.h"
#include "Master.h"
#include "Enemy3D.h"
#include "Scene.h"
#include "ObjectManager.h"
#include "Unit.h"

PlayerBullet3D::PlayerBullet3D(std::string filename, VECTOR initPos, VECTOR Direction, Unit* owner, float attack)
	:BaseBullet3D(filename, initPos, Direction, owner, attack)
{
	SetTag(Object3D::T_PlayerBullet3D);

	// ターゲットを教える
	mTargetTag = Object3D::T_Enemy3D;
}

PlayerBullet3D::~PlayerBullet3D()
{

}


