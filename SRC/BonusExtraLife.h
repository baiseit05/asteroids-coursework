#ifndef __BONUS_EXTRA_LIFE_H__
#define __BONUS_EXTRA_LIFE_H__


#include "GameObject.h"



class BonusExtraLife : public GameObject
{
public:
    BonusExtraLife(void);
    ~BonusExtraLife(void);

    bool CollisionTest(shared_ptr<GameObject> o);
    void OnCollision(const GameObjectList& objects);
    

};

#endif



