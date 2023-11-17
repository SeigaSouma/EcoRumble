//=============================================================================
// 
//  �R���W�������� [collision.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "collision.h"
#include "object2D.h"
#include "explosion.h"
#include "particle.h"
#include "manager.h"

#include "score.h"
#include "sound.h"
#include "particle.h"
#include "enemy.h"

//==========================================================================
// �}�N����`
//==========================================================================

//==========================================================================
// �����蔻��
//==========================================================================
bool collision::Collision(D3DXVECTOR3 SelfPos, D3DXVECTOR2 SelfSize, CObject::TYPE TargetType)
{
#if TOPCUR
	for (int nCntPriority = 0; nCntPriority < mylib_const::PRIORITY_NUM; nCntPriority++)
	{
		// �擪��ۑ�
		CObject *pObj = CObject::GetTop(nCntPriority);

		while (pObj != NULL)
		{// NULL������܂Ŗ������[�v

			// ���̃I�u�W�F�N�g���ꎞ�ۑ�
			CObject *pObjNext = pObj->GetNext();

			// ��ނ̎擾
			CObject::TYPE type = pObj->GetType();

			if (type == TargetType)
			{// �ړI�̎�ނ�������

				// 2D���擾
				CObject2D *pObj2D = pObj->GetObject2D();

				// �ʒu�擾
				D3DXVECTOR3 TargetPos = pObj2D->GetPosition();

				// �T�C�Y�擾
				D3DXVECTOR2 TargetSize = pObj2D->GetSize();

				// �����蔻��
				if (
					SelfPos.x - SelfSize.x < TargetPos.x + TargetSize.x &&
					SelfPos.x + SelfSize.x > TargetPos.x - TargetSize.x &&
					SelfPos.y - SelfSize.y < TargetPos.y + TargetSize.y &&
					SelfPos.y + SelfSize.y > TargetPos.y - TargetSize.y)
				{// �ΏۂƓG���d�Ȃ�����

					// ����������̏���
					HitAction(TargetType, pObj2D);

					// ��������
					return true;
				}
			}

			// ���̃I�u�W�F�N�g����
			pObj = pObjNext;
		}
	}
#endif
	// �������ĂȂ�
	return false;
}

//==========================================================================
// �����蔻��(�I�[�o�[���[�h)
//==========================================================================
bool collision::Collision(D3DXVECTOR3 SelfPos, D3DXVECTOR2 SelfSize, CObject2D *pSelfObj2D, CObject::TYPE TargetType)
{
#if TOPCUR
	for (int nCntPriority = 0; nCntPriority < mylib_const::PRIORITY_NUM; nCntPriority++)
	{
		// �擪��ۑ�
		CObject *pObj = CObject::GetTop(nCntPriority);

		while (pObj != NULL)
		{// NULL������܂Ŗ������[�v

			// ���̃I�u�W�F�N�g���ꎞ�ۑ�
			CObject *pObjNext = pObj->GetNext();

			// ��ނ̎擾
			CObject::TYPE type = pObj->GetType();

			if (type == TargetType)
			{// �ړI�̎�ނ�������

				// 2D���擾
				CObject2D *pObj2D = pObj->GetObject2D();

				// �ʒu�擾
				D3DXVECTOR3 TargetPos = pObj2D->GetPosition();

				// �T�C�Y�擾
				D3DXVECTOR2 TargetSize = pObj2D->GetSize();

				// �����蔻��
				if (
					SelfPos.x - SelfSize.x < TargetPos.x + TargetSize.x &&
					SelfPos.x + SelfSize.x > TargetPos.x - TargetSize.x &&
					SelfPos.y - SelfSize.y < TargetPos.y + TargetSize.y &&
					SelfPos.y + SelfSize.y > TargetPos.y - TargetSize.y)
				{// �ΏۂƓG���d�Ȃ�����

					// ����������̏���
					HitAction(TargetType, pSelfObj2D, pObj2D);

					// ��������
					return true;
				}
			}

			// ���̃I�u�W�F�N�g����
			pObj = pObjNext;
		}
	}
#endif
	// �������ĂȂ�
	return false;
}

//==========================================================================
// �����蔻��(�I�[�o�[���[�h)
//==========================================================================
bool collision::Collision(float fPosLeft, float fPosRight, float fPosUp, float fPosDown, CObject::TYPE TargetType)
{
#if TOPCUR
	for (int nCntPriority = 0; nCntPriority < mylib_const::PRIORITY_NUM; nCntPriority++)
	{
		// �擪��ۑ�
		CObject *pObj = CObject::GetTop(nCntPriority);

		while (pObj != NULL)
		{// NULL������܂Ŗ������[�v

			// ���̃I�u�W�F�N�g���ꎞ�ۑ�
			CObject *pObjNext = pObj->GetNext();

			// ��ނ̎擾
			CObject::TYPE type = pObj->GetType();

			if (type == TargetType)
			{// �ړI�̎�ނ�������

				// 2D���擾
				CObject2D *pObj2D = pObj->GetObject2D();

				// �ʒu�擾
				D3DXVECTOR3 TargetPos = pObj2D->GetPosition();

				// �T�C�Y�擾
				D3DXVECTOR2 TargetSize = pObj2D->GetSize();

				// �����蔻��
				if (
					fPosLeft < TargetPos.x + TargetSize.x &&
					fPosRight > TargetPos.x - TargetSize.x &&
					fPosUp < TargetPos.y + TargetSize.y &&
					fPosDown > TargetPos.y - TargetSize.y)
				{// �ΏۂƓG���d�Ȃ�����

					// ����������̏���
					HitAction(TargetType, pObj2D);

					// ��������
					return true;
				}
			}

			// ���̃I�u�W�F�N�g����
			pObj = pObjNext;
		}
	}
#endif

	// �������ĂȂ�
	return false;
}

//==========================================================================
// �����������Ƃ̏���
//==========================================================================
void collision::HitAction(CObject::TYPE TargetType, CObject2D *pObj2D)
{
	// �ʒu�擾
	D3DXVECTOR3 TargetPos = pObj2D->GetPosition();

	switch (TargetType)
	{
	case CObject::TYPE_ITEM:
		// �p�[�e�B�N������
		my_particle::Create(TargetPos, my_particle::TYPE_RESULTMESSAGE);

		// ���ˉ�
		CManager::GetSound()->PlaySound(CSound::LABEL_SE_KYUPIIN);

		// �I������
		pObj2D->Uninit();
		break;

	case CObject::TYPE_BULLET:
		// �����̐���
		CExplosion::Create(TargetPos);

		// �p�[�e�B�N������
		my_particle::Create(TargetPos, my_particle::TYPE_MANYTXT_MINI);

		// �Ώۂ̏I������
		pObj2D->Uninit();
		break;

	default:
		break;
	}
}

//==========================================================================
// �����������Ƃ̏���(�I�|�o�[���[�h)
//==========================================================================
void collision::HitAction(CObject::TYPE TargetType, CObject2D *pSelfObj2D, CObject2D *pTargetObj2D)
{
	// �ʒu�擾
	D3DXVECTOR3 TargetPos = pTargetObj2D->GetPosition();

	// �����̎�ގ擾
	CObject::TYPE SelfType = pSelfObj2D->GetType();
	//CEnemy *pEnemy;
	// �����̎��
	switch (SelfType)
	{
	case CObject::TYPE_BULLET:

		// �Ώۂ̎�ޕʏ���
		switch (TargetType)
		{
		case CObject::TYPE_PLAYER:

			// �Ώۂ̏I������
			pTargetObj2D->Uninit();

			// �����̐���
			CExplosion::Create(TargetPos);

			// �p�[�e�B�N������
			my_particle::Create(TargetPos, my_particle::TYPE_MANYTXT_MINI);
			break;

		case CObject::TYPE_ENEMY:

			//pSelfObj2D->GetTemplate<CEnemy>()->Hit(1);
			//pEnemy = pSelfObj2D->GetTemplate<CEnemy>();

			//pTargetObj2D->GetEnemy()->Hit(1);

			// �Ώۂ̏I������
			pTargetObj2D->Uninit();

			// �����̐���
			CExplosion::Create(TargetPos);

			// �p�[�e�B�N������
			my_particle::Create(TargetPos, my_particle::TYPE_MANYTXT_MINI);
			break;

		default:
			break;
		}
		break;
	}

	switch (TargetType)
	{
	case CObject::TYPE_ITEM:
		// �p�[�e�B�N������
		my_particle::Create(TargetPos, my_particle::TYPE_RESULTMESSAGE);

		// ���ˉ�
		CManager::GetSound()->PlaySound(CSound::LABEL_SE_KYUPIIN);

		// �I������
		pTargetObj2D->Uninit();
		break;

	case CObject::TYPE_BULLET:
		// �����̐���
		CExplosion::Create(TargetPos);

		// �p�[�e�B�N������
		my_particle::Create(TargetPos, my_particle::TYPE_MANYTXT_MINI);

		// �Ώۂ̏I������
		pTargetObj2D->Uninit();
		break;

	default:
		break;
	}
}

//==========================================================================
// �u���b�N�̓����蔻��
//==========================================================================
bool collision::CollisionBlock(CObject2D *(&pSelfObj2D))
{
	// ���n�������ǂ���
	bool bLand = false;

	// �ʒu�擾
	D3DXVECTOR3 pos = pSelfObj2D->GetPosition();

	// �ړ��ʎ擾
	D3DXVECTOR3 move = pSelfObj2D->GetMove();

	// �T�C�Y�擾
	D3DXVECTOR2 size = pSelfObj2D->GetSize();

	// �O��̈ʒu�擾
	D3DXVECTOR3 posOld = pSelfObj2D->GetOldPosition();

	// �ʒu�X�V
	pos.x += move.x;

	// �ʒu�ݒ�
	pSelfObj2D->SetPosition(pos);

	// X�̃u���b�N����
	CollisionBlockX(pSelfObj2D);
	pos = pSelfObj2D->GetPosition();
	move = pSelfObj2D->GetMove();

	// �ʒu�ݒ�
	pSelfObj2D->SetPosition(pos);

	// �ʒu�X�V
	pos.y += move.y;

	// �ʒu�ݒ�
	pSelfObj2D->SetPosition(pos);

	// Y�̃u���b�N����
	bLand = collision::CollisionBlockY(pSelfObj2D);
	pos = pSelfObj2D->GetPosition();
	move = pSelfObj2D->GetMove();

	if (pos.y >= SCREEN_HEIGHT)
	{// �n�ʂɕt����

		bLand = true;	// ���n���
		move.y = 0.0f;
	}

	// �ʒu�ݒ�
	pSelfObj2D->SetPosition(pos);

	// �ړ��ʐݒ�
	pSelfObj2D->SetMove(move);

	return bLand;
}

//==========================================================================
// �u���b�N�̓����蔻��(Y���W)
//==========================================================================
bool collision::CollisionBlockY(CObject2D *(&pSelfObj2D))
{
	// �ʒu�擾
	D3DXVECTOR3 pos = pSelfObj2D->GetPosition();

	// �ړ��ʎ擾
	D3DXVECTOR3 move = pSelfObj2D->GetMove();

	// �T�C�Y�擾
	D3DXVECTOR2 size = pSelfObj2D->GetSize();

	// �O��̈ʒu�擾
	D3DXVECTOR3 posOld = pSelfObj2D->GetOldPosition();

	// ���n�������ǂ���
	bool bLand = false;

#if TOPCUR
	for (int nCntPriority = 0; nCntPriority < mylib_const::PRIORITY_NUM; nCntPriority++)
	{
		// �擪��ۑ�
		CObject *pObj = CObject::GetTop(nCntPriority);

		while (pObj != NULL)
		{// NULL������܂Ŗ������[�v

			// ���̃I�u�W�F�N�g���ꎞ�ۑ�
			CObject *pObjNext = pObj->GetNext();

			// ��ނ̎擾
			CObject::TYPE TargetType = pObj->GetType();

			if (TargetType == CObject::TYPE_BLOCK)
			{// �u���b�N��������

				// 2D���擾
				CObject2D *pObj2D = pObj->GetObject2D();

				// �ʒu�擾
				D3DXVECTOR3 posBlock = pObj2D->GetPosition();

				// �O��̈ʒu�擾
				D3DXVECTOR3 posOldBlock = pObj2D->GetOldPosition();

				// �T�C�Y�擾
				D3DXVECTOR2 sizeBlock = pObj2D->GetSize();

				if (
					posBlock.x < pos.x + size.x &&		// ���݂̈ʒu���u���b�N�̕��ɂ���
					posBlock.x + sizeBlock.x > pos.x - size.x
					)
				{// �u���b�N�̕��ɂ���

					if (
						posOld.y <= posOldBlock.y &&	// �O��̈ʒu���A�u���b�N�̈ʒu������
						pos.y >= posBlock.y				// ���݂̈ʒu���u���b�N�̈ʒu������
						)
					{// �ォ��߂荞�񂾎�

						bLand = true;					// ���n���
						pos.y = posOldBlock.y;			// �u���b�N�̈ʒu�ɖ߂�
						move.y = 0.0f;					// �~�߂�
					}
					else if
						(
							posOld.y - size.y > posBlock.y + sizeBlock.y &&	// �O��̓��̈ʒu���A�u���b�N�̒������
							pos.y - size.y < posBlock.y + sizeBlock.y		// ���݂̓��̈ʒu���u���b�N������ɂ���
							)
					{// ������߂荞�񂾎�

						pos.y = (posBlock.y + sizeBlock.y) + size.y;	// �u���b�N��̈ʒu�ɖ߂�
						move.y = 0.0f;					// �~�߂�
					}
				}
			}

			// ���̃I�u�W�F�N�g����
			pObj = pObjNext;
		}
	}
#endif
	// �ʒu�ݒ�
	pSelfObj2D->SetPosition(pos);

	// �ړ��ʐݒ�
	pSelfObj2D->SetMove(move);

	return bLand;
}

//==========================================================================
// �u���b�N�̓����蔻��(X���W)
//==========================================================================
bool collision::CollisionBlockX(CObject2D *(&pSelfObj2D))
{
	// �ʒu�擾
	D3DXVECTOR3 pos = pSelfObj2D->GetPosition();

	// �ړ��ʎ擾
	D3DXVECTOR3 move = pSelfObj2D->GetMove();

	// �T�C�Y�擾
	D3DXVECTOR2 size = pSelfObj2D->GetSize();

	// �O��̈ʒu�擾
	D3DXVECTOR3 posOld = pSelfObj2D->GetOldPosition();

	// ���n�������ǂ���
	bool bLand = false;
#if TOPCUR

	for (int nCntPriority = 0; nCntPriority < mylib_const::PRIORITY_NUM; nCntPriority++)
	{
		// �擪��ۑ�
		CObject *pObj = CObject::GetTop(nCntPriority);

		while (pObj != NULL)
		{// NULL������܂Ŗ������[�v

			// ���̃I�u�W�F�N�g���ꎞ�ۑ�
			CObject *pObjNext = pObj->GetNext();

			// ��ނ̎擾
			CObject::TYPE TargetType = pObj->GetType();

			if (TargetType == CObject::TYPE_BLOCK)
			{// �u���b�N��������

				// 2D���擾
				CObject2D *pObj2D = pObj->GetObject2D();

				// �ʒu�擾
				D3DXVECTOR3 posBlock = pObj2D->GetPosition();

				// �O��̈ʒu�擾
				D3DXVECTOR3 posOldBlock = pObj2D->GetOldPosition();

				// �T�C�Y�擾
				D3DXVECTOR2 sizeBlock = pObj2D->GetSize();

				// ���̔���
				if (posBlock.y < pos.y &&
					posBlock.y + sizeBlock.y > pos.y - size.y
					)
				{// ���݂̈ʒu���u���b�N�̍����ɂ���

					if (
						posOld.x + size.x <= posOldBlock.x &&	// �O��̈ʒu���u���b�N�̈ʒu������
						pos.x + size.x >= posBlock.x			// ���݂̈ʒu���u���b�N�̈ʒu�����E
						)
					{//������߂荞�񂾏ꍇ

						pos.x = posOldBlock.x - size.x;	//�u���b�N�̈ʒu�ɖ߂�
						move.x = 0.0f;	// �~�߂�
					}
					else if
						(
							posOld.x - size.x >= posOldBlock.x + sizeBlock.x &&	// �O��̈ʒu���u���b�N�̈ʒu���E
							pos.x - size.x <= posBlock.x + sizeBlock.x			// ���݂̈ʒu���u���b�N�̕���荶
							)
					{// �E����߂荞�񂾏ꍇ

						pos.x = posOldBlock.x + sizeBlock.x + size.x;	// �u���b�N�̈ʒu�ɖ߂�
						move.x = 0.0f;	// �~�߂�
					}
				}
			}

			// ���̃I�u�W�F�N�g����
			pObj = pObjNext;
		}
	}
#endif
	// �ʒu�ݒ�
	pSelfObj2D->SetPosition(pos);

	// �ړ��ʐݒ�
	pSelfObj2D->SetMove(move);

	return bLand;
}