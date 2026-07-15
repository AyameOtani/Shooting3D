#include "PlayerAnim.h"
#include "Player3D.h"

// ƒ‚ƒfƒ‹‚²‚Æ‚É“Ç‚Ýž‚ÞƒAƒjƒ[ƒVƒ‡ƒ“‚ð•Ï‚¦‚Ä‚¢‚é
void PlayerAnim::Load(Player3D* player, const std::string& modelPath)
{
    //// ƒ]ƒ“ƒr
    //if (modelPath == "Resource/3D/Zombie/Zombie_Mesh.mv1")
    //{
    //    player->AddAnimation(ANIMATION_NEUTRAL, "Resource/3D/Zombie/Ch10_nonPBR@Zombie Idle.mv1");
    //    player->AddAnimation(ANIMATION_WALKING, "Resource/3D/Zombie/Ch10_nonPBR@Walking.mv1");
    //    player->AddAnimation(ANIMATION_ATTACK, "Resource/3D/Zombie/Ch10_nonPBR@Zombie Attack.mv1");
    //    player->AddAnimation(ANIMATION_RUN, "Resource/3D/Zombie/Ch10_nonPBR@Zombie Run.mv1");

    //    player->AddAnimation(ANIMATION_JUMP_IN, "Resource/3D/Zombie/Mutant JumpIn.mv1");
    //    player->AddAnimation(ANIMATION_JUMP_LOOP, "Resource/3D/Zombie/Mutant JumpLoop.mv1");
    //    player->AddAnimation(ANIMATION_JUMP_OUT, "Resource/3D/Zombie/Mutant JumpOut.mv1");
    //    return;
    //}

    //// ŒR‘à
    //if (modelPath == "Resource/3D/ŒR‘à/Guntai.mv1")
    //{
    //    player->AddAnimation(ANIMATION_NEUTRAL, "Resource/3D/ŒR‘à/Neutral.mv1");
    //    player->AddAnimation(ANIMATION_WALKING, "Resource/3D/ŒR‘à/Running.mv1");
    //    player->AddAnimation(ANIMATION_RUN, "Resource/3D/ŒR‘à/Running.mv1");
    //    player->AddAnimation(ANIMATION_ATTACK, "Resource/3D/ŒR‘à/Attack.mv1");

    //    player->AddAnimation(ANIMATION_JUMP_IN, "Resource/3D/ŒR‘à/Jump_In.mv1");
    //    player->AddAnimation(ANIMATION_JUMP_LOOP, "Resource/3D/ŒR‘à/Jump_Loop.mv1");
    //    player->AddAnimation(ANIMATION_JUMP_OUT, "Resource/3D/ŒR‘à/Jump_Out.mv1");
    //    return;
    //}

    //// ƒ{ƒu
    //if (modelPath == "Resource/3D/Bob/Bob.mv1")
    //{
    //    player->AddAnimation(ANIMATION_NEUTRAL, "Resource/3D/Bob/Idle.mv1");
    //    player->AddAnimation(ANIMATION_WALKING, "Resource/3D/Bob/Run.mv1");
    //    player->AddAnimation(ANIMATION_RUN, "Resource/3D/Bob/Run.mv1");
    //    player->AddAnimation(ANIMATION_ATTACK, "Resource/3D/Bob/Attack.mv1");

    //    player->AddAnimation(ANIMATION_JUMP_IN, "Resource/3D/Bob/JumpIn.mv1");
    //    player->AddAnimation(ANIMATION_JUMP_LOOP, "Resource/3D/Bob/JumpLoop.mv1");
    //    player->AddAnimation(ANIMATION_JUMP_OUT, "Resource/3D/Bob/JumpOut.mv1");
    //    return;
    //}

    //// “÷‰ò‚Ìˆ«–‚
    //if (modelPath == "Resource/3D/Devil/Devil.mv1")
    //{
    //    player->AddAnimation(ANIMATION_NEUTRAL, "Resource/3D/Devil/Idle.mv1");
    //    player->AddAnimation(ANIMATION_WALKING, "Resource/3D/Devil/Run.mv1");
    //    player->AddAnimation(ANIMATION_RUN, "Resource/3D/Devil/Run.mv1");
    //    player->AddAnimation(ANIMATION_ATTACK, "Resource/3D/Devil/Attack.mv1");

    //    player->AddAnimation(ANIMATION_JUMP_IN, "Resource/3D/Devil/JumpIn.mv1");
    //    player->AddAnimation(ANIMATION_JUMP_LOOP, "Resource/3D/Devil/JumpLoop.mv1");
    //    player->AddAnimation(ANIMATION_JUMP_OUT, "Resource/3D/Devil/JumpOut.mv1");
    //    return;
    //}
}

void PlayerAnim::LoadSelectAnim(Model* model, const std::string& modelPath)
{
    if (!model) return;

    //if (modelPath == "Resource/3D/Zombie/Zombie_Mesh.mv1")
    //{
    //    model->AddAnimation(ANIMATION_NEUTRAL,"Resource/3D/Zombie/Ch10_nonPBR@Zombie Idle.mv1");
    //}
    //else if (modelPath == "Resource/3D/ŒR‘à/Guntai.mv1")
    //{
    //    model->AddAnimation(ANIMATION_NEUTRAL,"Resource/3D/ŒR‘à/Neutral.mv1");
    //}
    //else if (modelPath == "Resource/3D/Bob/Bob.mv1")
    //{
    //    model->AddAnimation(ANIMATION_NEUTRAL,"Resource/3D/Bob/Idle.mv1");
    //}
    //else if (modelPath == "Resource/3D/Devil/Devil.mv1")
    //{
    //    model->AddAnimation(ANIMATION_NEUTRAL,"Resource/3D/Devil/Idle.mv1");
    //}
}