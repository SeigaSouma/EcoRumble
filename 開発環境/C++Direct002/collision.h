//=============================================================================
// 
//  �R���W�����w�b�_�[ [collision.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _COLLISION_H_
#define _COLLISION_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "object.h"

//==========================================================================
// �N���X��`
//==========================================================================
// �R���W�����N���X��`
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