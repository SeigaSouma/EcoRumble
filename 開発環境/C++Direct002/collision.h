//=============================================================================
// 
//  コリジョンヘッダー [collision.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _COLLISION_H_
#define _COLLISION_H_	// 二重インクルード防止

#include "main.h"
#include "object.h"

//==========================================================================
// クラス定義
//==========================================================================
// コリジョンクラス定義
namespace collision
{
	bool Collision(D3DXVECTOR3 SelfPos, D3DXVECTOR2 SelfSize, CObject::TYPE TargetType);
	bool Collision(D3DXVECTOR3 SelfPos, D3DXVECTOR2 SelfSize, CObject2D *pSelfObj2D, CObject::TYPE TargetType);
	bool Collision(float fPosLeft, float fPosRight, float fPosUp, float fPosDown, CObject::TYPE TargetType);
	void HitAction(CObject::TYPE TargetType, CObject2D *pObj2D);
	void HitAction(CObject::TYPE TargetType, CObject2D *pSelfObj2D, CObject2D *pTargetObj2D);
	bool CollisionBlock(CObject2D  *(&pSelfObj2D));
	bool CollisionBlockX(CObject2D  *(&pSelfObj2D));
	bool CollisionBlockY(CObject2D  *(&pSelfObj2D));
}

#endif