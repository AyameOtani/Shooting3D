#include "LockOnMark.h"
#include "Master.h"
#include "Player3D.h"

LockOnMark::LockOnMark(Player3D* player)
{
    prevTime = GetNowCount();

    mpPlayer = player;
}


LockOnMark::~LockOnMark()
{

}

// 時間を取り直す
void LockOnMark::ResetMark()
{
    prevTime = GetNowCount();
}


void LockOnMark::UpdateMark()
{
  
    // プレイヤーから今の時間もらう
    float delay = mpPlayer->GetLockDelay();  // 間隔
    float maxDelay = mpPlayer->GetMaxDelay(); // 最大間隔


    if (maxDelay <= 0) return;

    //　割合計算
    float rate = 1.0f - (delay / maxDelay);

    // ゲージの枚数
    float currentTotal = rate * SEGMENTS;

    filledSeg = (int)currentTotal; // 整数部分
    partial = currentTotal - (float)filledSeg; // 少数部分

}

void LockOnMark::InitializeMark()
{
    filledSeg = SEGMENTS;
    partial = 0.0f; // ここを0.0にしておかないと時間+1からカウントが始まる
    prevTime = GetNowCount();
    mnRemainTime = (int)TotalTime;
}


void LockOnMark::DrawMark()
{
    const auto& targets = mpPlayer->GetLockedTargets(); // 敵リストもらう
    if (targets.empty()) { return; } // ターゲットいないならやらん


    // リストの最後の敵がチャージ中の場合
    Object3D* lastTarget = targets.back();

    // 敵の数だけループして描画
    for (auto pTarget : targets)
    {
        if (pTarget == nullptr) continue;

        float posY = 70.0f; // デフォ
        // 岩の時は上に
        if (pTarget->GetTag() == Object3D::T_Rock3D)
        {
            posY = 120.0;
        }

        // 位置
        VECTOR pos = VAdd(pTarget->GetPosition(), VGet(0.0f, posY, 0.0f));

        SetUseZBuffer3D(TRUE);   // Zバッファ参照
        SetWriteZBuffer3D(FALSE); // 書き込みOFF
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);

        if (pTarget == lastTarget && mpPlayer->GetLockDelay() > 0.0f)
        {
            // 3D空間での半径
            float radius = 35.0f;
            int color = GetColor(30, 30, 30);

            // 背景の暗い円
            DrawRadialGauge3D(pos, radius - 1.0f, SEGMENTS, 0.0f, SEGMENTS, color, 90.0f);
            // 赤いチャージゲージ
            DrawRadialGauge3D(pos, radius, filledSeg, partial, SEGMENTS, GetColor(235, 0, 0), 90.0f);

            // 中央の暗い円
            DrawRadialGauge3D(pos, 27.0f - 1.0f, SEGMENTS, 0.0f, SEGMENTS, color, 90.0f);
        }

        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        SetUseZBuffer3D(TRUE);
        SetWriteZBuffer3D(TRUE);
    }
}


// 円を書く関数
void LockOnMark::DrawRadialGauge(float cx, float cy, float r,
    int filledSeg, float partial, int segments,
    unsigned int colFill, unsigned int colBack, float startDeg = -90.0f)
{
    if (segments < 1) segments = 1;                 // 最大数が１より少なかったら１にする
    if (filledSeg < 0) filledSeg = 0;               // 埋まっている三角形の数が０以下なら０にする
    if (filledSeg > segments) filledSeg = segments; // 枚数が最大数を変えたら最大数にする
    if (partial < 0.0f) partial = 0.0f;             // 埋まり具合が０以下なら０にする
    if (partial > 1.0f) partial = 1.0f;             // 埋まり具合が１以上なら１にする

    stepDeg = 360.0f / segments; // 初期化 最大数を円の最大角度(360.0f)で割る


    // 充填（完全に埋まる分）
    for (int i = 0; i < filledSeg; ++i)
    {
        float a0 = DegToRad(startDeg + stepDeg * i);
        float a1 = DegToRad(startDeg + stepDeg * (i + 1));
        float x0 = cx + r * cosf(a0), y0 = cy + r * sinf(a0);
        float x1 = cx + r * cosf(a1), y1 = cy + r * sinf(a1);
        DrawTriangleAA(cx, cy, x0, y0, x1, y1, colFill, TRUE); // 埋まった部分のゲージ描画
    }

    // 端数分（部分的に埋まる1枚）
    if (partial > 0.0f && filledSeg < segments)
    {
        float a0 = DegToRad(startDeg + stepDeg * filledSeg);
        float aP = DegToRad(startDeg + stepDeg * (filledSeg + partial));
        float x0 = cx + r * cosf(a0), y0 = cy + r * sinf(a0);
        float xP = cx + r * cosf(aP), yP = cy + r * sinf(aP);
        DrawTriangleAA(cx, cy, x0, y0, xP, yP, colFill, TRUE); // 一枚のpartialの埋まり具合の描画
    }
}

// 新しく作成する3D用ゲージ関数
void LockOnMark::DrawRadialGauge3D(VECTOR centerPos, float r,
    int filledSeg, float partial, int segments,
    unsigned int colFill, float startDeg)
{
    if (segments < 1) segments = 1;
    float stepDeg = 360.0f / segments;

    // カメラの「右」と「上」の向きを取得して、常にカメラを向く板（ビルボード）を作る
    // Master::mpCamera からベクトルを取れる前提です
    VECTOR camRight = Master::mpCamera->GetRightVector();
    VECTOR camUp = Master::mpCamera->GetUpVector();

    // 充填部分（ループで三角形を並べて扇形を作る）
    for (int i = 0; i < filledSeg; ++i) {
        float a0 = DegToRad(startDeg + stepDeg * i);
        float a1 = DegToRad(startDeg + stepDeg * (i + 1));

        // 中心からカメラの右・上方向にずらして頂点を決める
        VECTOR v0 = VAdd(centerPos, VAdd(VScale(camRight, cosf(a0) * r), VScale(camUp, sinf(a0) * r)));
        VECTOR v1 = VAdd(centerPos, VAdd(VScale(camRight, cosf(a1) * r), VScale(camUp, sinf(a1) * r)));

        DrawTriangle3D(centerPos, v0, v1, colFill, TRUE);
    }

    // 端数部分（最後の1枚）
    if (partial > 0.0f && filledSeg < segments) {
        float a0 = DegToRad(startDeg + stepDeg * filledSeg);
        float aP = DegToRad(startDeg + stepDeg * (filledSeg + partial));

        VECTOR v0 = VAdd(centerPos, VAdd(VScale(camRight, cosf(a0) * r), VScale(camUp, sinf(a0) * r)));
        VECTOR vP = VAdd(centerPos, VAdd(VScale(camRight, cosf(aP) * r), VScale(camUp, sinf(aP) * r)));

        DrawTriangle3D(centerPos, v0, vP, colFill, TRUE);
    }
}