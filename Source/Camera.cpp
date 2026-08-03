#include "Camera.h"
#include "Master.h"
#include "ObjectManager.h"
#include "Scene3D.h"
#include "InputManager.h"
#include "Utility.h"
#include <cmath> // cosなどを使うためのインクルード

Camera::Camera()
	: mfHorizontalAngle(0.0f)
	, mfVerticalAngle(11.0f)
	, mvPosition(VGet(0.0f, 0.0f, -120.0f))     // カメラの座標
	, mvLookAtPosition(VGet(0.0f, 0.0f, 0.0f)) // カメラが見ている方向
	, mpTarget(nullptr)                        // まずは空っぽ
	, mnShakeTime(0)
	, mnShakeTimeCount(0)
	, mfShakeAngle(0.0f)
	, mfShakeTimeCounter(0.0f)
	, mfShakeTime(0.0f)
	, mfShakeWidth(0.0f)
	, mfShakeAngleSpeed(0.0f)
	, mfStepTime(0.0f)
	, mvShakePosition(VGet(0.0f, 0.0f, 0.0f))
{

}
Camera::~Camera()
{
}

void Camera::Initialize()
{
	// カメラのクリッピング距離の設定----カメラの映す距離（どこからどこまでを映すか）
	SetCameraNearFar(100.0f, 50000.0f); // 100 ～ 50000までオブジェクトを映す
	// 背景色の設定
	SetBackgroundColor(mnR, mnG, mnB);
	// カメラ設定を反映  上方向はベクトルYだから
	SetCameraPositionAndTarget_UpVecY(mvPosition, mvLookAtPosition);

	LookRange();


	// 更新処理を一度行っておく
	Update();
}

// 背景の色を変えるやつ
void Camera::ChangeBackColor(int r, int g, int b, int alpha)
{
	SetBackgroundColor(r, g, b, alpha);
}

void Camera::DefaultBackColor()
{
	SetBackgroundColor(mnR, mnG, mnB, 0);
}


void Camera::LookRange()
{
	// カメラ配置の「後」で比率と視野角を設定する
	// アスペクト比：1200で描いて1920に伸ばす際の理論値 (0.625f)
	// ※真ん中の形を保つための守り神
	float aspect = (float)Utility::RENDER_WIDTH / (float)Utility::SCREEN_WIDTH;
	SetCameraDotAspect(aspect);
	// 視野角：広角にすることで「端っこ」の流れる速度をブーストする
	SetupCamera_Perspective(DX_PI_F / 1.5f);

}

void Camera::Update()
{
	if (mpTarget != nullptr) // ターゲットがいなかったら
	{
		// 基準座標を対象の座標にする
		mvLookAtPosition = mpTarget->GetPosition(); // ターゲットからポジションをとってくる
		mvLookAtPosition.y += 90.0f;
	}
	else
	{
		// 注視点を少し上にずらす
		mvLookAtPosition.y = 40.0f;
	}
	

	// 画面揺れ処理
	Shake();

	//この中で処理は完結する　分かりやすいように
	{
		VECTOR temp;   // 作業用変数

		// 球面上の座標を求める
		// 250.0f は注視点からどれだけ離れているかという意味
		// 距離は変えないで回転したい
		const float distance = 210.0f;
		const float radius = 180.0f;
	    temp.x = distance * cosf(mfVerticalAngle / radius * DX_PI_F) * sinf(mfHorizontalAngle / radius * DX_PI_F);    // X座標
		temp.y = distance * sinf(mfVerticalAngle / radius * DX_PI_F);                                                 // Y座標
		temp.z = -(distance * cosf(mfVerticalAngle / radius * DX_PI_F) * cosf(mfHorizontalAngle / radius * DX_PI_F)); // Z座標

		//if (!IsSlideCamera()) // 追加 1117 いらない？ 0115
		{
			// 求めた座標と注視点の座標を足した位置がカメラ座標になる
			mvPosition = VAdd(temp, mvLookAtPosition);
		}

		// カメラ設定を反映
		SetCameraPositionAndTarget_UpVecY(mvPosition, mvLookAtPosition);
	}

	if (mbEnableControl)
	{
		UpdateRotation();
	}


	// 画面揺れの分を加算するように変更
	// 画面揺れの分を加算するように変更
	// カメラ設定を反映
	SetCameraPositionAndTarget_UpVecY(VAdd(mvPosition, mvShakePosition), VAdd(mvLookAtPosition, mvShakePosition));


	LookRange();
}

// 画面揺れ
void Camera::Shake()
{
	if (mfShakeTimeCounter < mfShakeTime)
	{
		// sinf を利用して揺らし座標を算出
		// note: 一旦Y座標だけを揺らしてみる
		mvShakePosition.y = sinf(mfShakeAngle) * (1.0f - (mfShakeTimeCounter / mfShakeTime)) * mfShakeWidth;
		mvShakePosition.x = 0.0f;
		mvShakePosition.z = 0.0f;

		// 揺らし処理に使用する sinf に渡す角度の変更処理
		mfShakeAngle += mfShakeAngleSpeed * mfStepTime;

		// 揺らす時間を経過させる
		mfShakeTimeCounter += mfStepTime;
	}
	else
	{
		// 揺らされていない場合は揺らし処理による加算座標を０にする
		mvShakePosition = VGet(0.0f, 0.0f, 0.0f);
	}
}


// 画面揺れ設定
void Camera::SetupShake(float time, float width, float angleSpeed, float stepTime)
{
	mfShakeTimeCounter = 0.0f;
	mfShakeTime = time;
	mfShakeWidth = width;
	mfShakeAngleSpeed = angleSpeed;
	mfStepTime = stepTime;
}

void Camera::UpdateRotation()
{
	
	int StickX, StickY;
	GetJoypadAnalogInputRight(&StickX, &StickY, DX_INPUT_PAD1);
	const int stickDeadZone = 50;

	// 水平方向
	if (CheckHitKey(KEY_INPUT_LEFT) || StickX < -stickDeadZone)
	{
		mfCurrentSpeedH -= ADD_SPEED;
		if (mfCurrentSpeedH < -MAX_SPEED)	 // ここで-なんかになって
		{
			mfCurrentSpeedH = -MAX_SPEED;
		}
	}
	else if (CheckHitKey(KEY_INPUT_RIGHT) || StickX > stickDeadZone)
	{
		mfCurrentSpeedH += ADD_SPEED;
		if (mfCurrentSpeedH > MAX_SPEED)
		{
			mfCurrentSpeedH = MAX_SPEED;
		}
	}
	else
	{
		// 徐々に遅く
		if (mfCurrentSpeedH > 0)
		{
			mfCurrentSpeedH -= SUB_SPEED;
			if (mfCurrentSpeedH < 0)
			{
				mfCurrentSpeedH = 0;
			}
		}
		else if (mfCurrentSpeedH < 0)
		{
			mfCurrentSpeedH += SUB_SPEED;
			if (mfCurrentSpeedH > 0)
			{
				mfCurrentSpeedH = 0;
			}
		}
	}


	// 垂直方向
	if (CheckHitKey(KEY_INPUT_UP) || StickY < -stickDeadZone)
	{
		mfCurrentSpeedV += ADD_SPEED * 1.7f;
		if (mfCurrentSpeedV > MAX_SPEED) mfCurrentSpeedV = MAX_SPEED;
	}
	else if (CheckHitKey(KEY_INPUT_DOWN) || StickY > stickDeadZone)
	{
		mfCurrentSpeedV -= ADD_SPEED * 1.7f;
		if (mfCurrentSpeedV < -MAX_SPEED) mfCurrentSpeedV = -MAX_SPEED;
	}
	else
	{
		// 徐々に遅く
		if (mfCurrentSpeedV > 0)
		{
			mfCurrentSpeedV -= SUB_SPEED * 1.2f;
			if (mfCurrentSpeedV < 0)
			{
				mfCurrentSpeedV = 0;
			}
		}
		else if (mfCurrentSpeedV < 0)
		{
			mfCurrentSpeedV += SUB_SPEED * 1.2f;
			if (mfCurrentSpeedV > 0)
			{
				mfCurrentSpeedV = 0;
			}
		}
	}

	//  計算した速度を角度に反映
	mfHorizontalAngle += mfCurrentSpeedH; // それをここでやっても+ だけど-5とかだから左に行くはず
	mfVerticalAngle += mfCurrentSpeedV;

	// 角度の正規化clamp
	if (mfHorizontalAngle >= 180.0f) mfHorizontalAngle -= 360.0f;
	if (mfHorizontalAngle < -180.0f) mfHorizontalAngle += 360.0f;

	if (mfVerticalAngle >= 80.0f) mfVerticalAngle = 80.0f;
	if (mfVerticalAngle <= 11.0f)  mfVerticalAngle = 11.0f;
	
}


// ゲームを何週かしたときにカメラの位置が前のままになるのを防ぐため
// Scene3D.cppの初期化で呼んでいる
void Camera::Reset()
{
	// 座標と角度を初期化
	mvPosition = VGet(0.0f, 0.0f, -120.0f);
	mvLookAtPosition = VGet(0.0f, 0.0f, 0.0f);
	mfHorizontalAngle = 0.0f;
	mfVerticalAngle = 11.0f;
	mpTarget = nullptr;

	// 速度リセット
	mfCurrentSpeedH = 0.0f;
	mfCurrentSpeedV = 0.0f;

	mbEnableControl = true; // 操作可能にリセット

	// 画面揺れのリセットも呼ぶ
	mfShakeTime = 0.0f;
	mfShakeTimeCounter = 0.0f;
	mfShakeWidth = 0.0f;
	mfShakeAngle = 0.0f;
	mvShakePosition = VGet(0.0f, 0.0f, 0.0f);

	mbIsFixedMode = false; // 移動カメラオフ

	// カメラを反映
	SetCameraPositionAndTarget_UpVecY(mvPosition, mvLookAtPosition);
}


// カメラの右方向ベクトルを計算して返す
VECTOR Camera::GetRightVector()
{
	//  視線方向  注視点 - カメラ位置  を計算して正規化
	VECTOR forward = VNorm(VSub(mvLookAtPosition, mvPosition));

	// 世界の上(0, 1, 0)と視線の外積をとってカメラの右を出す
	VECTOR right = VCross(VGet(0.0f, 1.0f, 0.0f), forward);

	return VNorm(right);
}


// カメラの上方向ベクトルを計算して返す
VECTOR Camera::GetUpVector()
{
	// 線方向を計算
	VECTOR forward = VNorm(VSub(mvLookAtPosition, mvPosition));

	// 右と視線の外積をとってカメラにとっての真上を出す
	VECTOR right = GetRightVector();
	VECTOR up = VCross(forward, right);



	return VNorm(up);
}