#include "BonusExtraLife.h"
#include "BoundingSphere.h"
#include "Animation.h"           
#include "AnimationManager.h"
#include "Sprite.h"

BonusExtraLife::BonusExtraLife()
    : GameObject("BonusExtraLife")
{
    Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("bonus");
    if (anim_ptr)
    {
        shared_ptr<Sprite> sprite = make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
        sprite->SetLoopAnimation(true);
        SetSprite(sprite);
        SetScale(0.1f);
    }

    SetBoundingShape(make_shared<BoundingSphere>(GetThisPtr(), 5.0f));
}
