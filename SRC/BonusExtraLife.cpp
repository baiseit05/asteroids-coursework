#include "GameUtil.h"
#include "BonusExtraLife.h"
#include "Animation.h"
#include "AnimationManager.h"
#include "BoundingShape.h"
#include "BoundingSphere.h"
#include "Spaceship.h"
#include <stdlib.h>
#include "Asteroids.h"

BonusExtraLife::BonusExtraLife(void) : GameObject("BonusExtraLife")
{
    // Random direction and speed
    mAngle = rand() % 360;
    mRotation = 0;
    mPosition.x = rand() / 2;
    mPosition.y = rand() / 2;
    mPosition.z = 0.0;
    mVelocity.x = 5.0f * cos(DEG2RAD * mAngle);
    mVelocity.y = 5.0f * sin(DEG2RAD * mAngle);
    mVelocity.z = 0.0;

    

    // Set sprite
    Animation* anim = AnimationManager::GetInstance().GetAnimationByName("bonus_life");
    if (anim)
    {
        shared_ptr<Sprite> sprite = make_shared<Sprite>(anim->GetWidth(), anim->GetHeight(), anim);
        sprite->SetLoopAnimation(true);
        SetSprite(sprite);
        SetScale(0.1f);
    }
}

BonusExtraLife::~BonusExtraLife(void)
{
}

bool BonusExtraLife::CollisionTest(shared_ptr<GameObject> o)
{
    if (GetType() == o->GetType()) return false;
    if (!mBoundingShape || !o->GetBoundingShape()) return false;
    return mBoundingShape->CollisionTest(o->GetBoundingShape());
}

void BonusExtraLife::OnCollision(const GameObjectList& objects)
{
    for (auto obj : objects)
    {
        if (obj->GetType() == GameObjectType("Spaceship"))
        {
            // Bonus will be removed; Asteroids.cpp handles awarding the life
            mWorld->FlagForRemoval(GetThisPtr());
            
            break;
        }
    }
}

