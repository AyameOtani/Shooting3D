#include "EnemyBullet3D.h"
#include "Model.h"
#include "Master.h"
#include "Player3D.h"
#include "Scene.h"
#include "ObjectManager.h"

EnemyBullet3D::EnemyBullet3D(std::string filename, VECTOR initPos, VECTOR Direction, Unit* owner, float attack)
	:BaseBullet3D(filename, initPos, Direction, owner, attack)
{
	SetTag(Object3D::T_EnemyBullet3D);

	// ターゲットを教える
	mTargetTag = Object3D::T_Player3D;
}

EnemyBullet3D::~EnemyBullet3D()
{

}
