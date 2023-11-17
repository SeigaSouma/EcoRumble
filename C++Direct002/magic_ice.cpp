//=============================================================================
// 
//  �X���@���� [magic_ice.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "magic_ice.h"
#include "manager.h"
#include "renderer.h"
#include "game.h"
#include "calculation.h"
#include "objectX.h"
#include "objectChara.h"
#include "3D_Effect.h"
#include "particle.h"
#include "ballast.h"
#include "camera.h"
#include "elevation.h"

//==========================================================================
// �}�N����`
//==========================================================================
#define LIFE	(64)	// ����
#define RANGE	(400)	// �����_�������͈̔�
#define ATK_RANGE	(25.0f)

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================
const char *CMagicIce::m_pModelFile = "data\\MODEL\\magic_ice.x";	// ���f���t�@�C��
int CMagicIce::m_nIdxXFile = 0;			// X�t�@�C���̃C���f�b�N�X�ԍ�

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CMagicIce::CMagicIce(int nPriority) : CObject(nPriority)
{
	m_posOrigin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// ���_
	m_nNumAll = 0;		// ����
	m_nLife = 0;		// ����
	memset(&m_pObjX[0], NULL, sizeof(m_pObjX));				// �I�u�W�F�N�gX�̃|�C���^
	memset(&m_nWait[0], NULL, sizeof(m_nWait));				// �ҋ@����
	memset(&m_nDirection[0], NULL, sizeof(m_nDirection));	// ����
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CMagicIce::~CMagicIce()
{
	
}

//==========================================================================
// ���[�h����
//==========================================================================
HRESULT CMagicIce::Load(void)
{
	// X�t�@�C���̃f�[�^�擾
	CXLoad *pXLoad = CScene::GetXLoad();

	// X�t�@�C���̃��[�h
	m_nIdxXFile = pXLoad->XLoad(m_pModelFile);

	if (m_nIdxXFile <= 0)
	{// ���s���Ă�����
		return E_FAIL;
	}

	return S_OK;
}

//==========================================================================
// ��������
//==========================================================================
CMagicIce *CMagicIce::Create(D3DXVECTOR3 pos)
{
	// �����p�̃I�u�W�F�N�g
	CMagicIce *pBallast = NULL;

	if (pBallast == NULL)
	{// NULL��������

		// �������̊m��
		pBallast = DEBUG_NEW CMagicIce;

		//if (pBallast->GetID() < 0)
		//{// �������m�ۂɎ��s���Ă�����

		//	delete pBallast;
		//	return NULL;
		//}

		if (pBallast != NULL)
		{// �������̊m�ۂ��o���Ă�����

			// �u��
			pos.y += 300.0f;

			// �ʒu�ݒ�
			pBallast->m_posOrigin = pos;

			// ����������
			pBallast->Init();
		}
		else
		{
			return NULL;
		}

		return pBallast;
	}

	return NULL;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CMagicIce::Init(void)
{
	// ��ނ̐ݒ�
	SetType(TYPE_MAGIC_ICE);

	m_nLife = LIFE;	// ����
	m_nIdxBig = LIFE / 2 + (LIFE / 2);	// ����Ȃ�̃C���f�b�N�X�ԍ�

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CMagicIce::Uninit(void)
{
	for (int nCntIce = 0; nCntIce < MAX_MAGICICE; nCntIce++)
	{
		if (m_pObjX[nCntIce] != NULL)
		{// NULL����Ȃ�������

			// �I�u�W�F�N�gX�̏I������
			m_pObjX[nCntIce]->Uninit();
			m_pObjX[nCntIce] = NULL;
		}
	}

	// ���폜
	Release();
}

//==========================================================================
// �X�V����
//==========================================================================
void CMagicIce::Update(void)
{

	// �������Z
	m_nLife--;

	if (m_nLife >= 0)
	{// �܂�����������ꍇ

		// �ݒ菈��
		if (Set() == false)
		{// ���s���Ă�����
			return;
		}
		if (Set() == false)
		{// ���s���Ă�����
			return;
		}
	}

	if (m_nNumAll <= 0)
	{// �S�ď����Ă�����
		Uninit();
		return;
	}

	for (int nCntIce = 0; nCntIce < MAX_MAGICICE; nCntIce++)
	{// ���ꂼ��̍X�V

		if (m_pObjX[nCntIce] == NULL)
		{// NULL��������
			continue;
		}

		// �ʒu�擾
		D3DXVECTOR3 pos = m_pObjX[nCntIce]->GetPosition();

		// �ړ��ʎ擾
		D3DXVECTOR3 move = m_pObjX[nCntIce]->GetMove();

		// �����擾
		D3DXVECTOR3 rot = m_pObjX[nCntIce]->GetRotation();

		// �U���̔���
		float fATKRange = ATK_RANGE;

		if (nCntIce == m_nIdxBig)
		{// ����Ȃ�̎�����
			fATKRange *= 10.0f;
		}

		// ��]
		rot.y += D3DX_PI * 0.03f * m_nDirection[nCntIce];

		// �����ݒ�
		m_pObjX[nCntIce]->SetRotation(rot);

		// �҂����Ԍ��Z
		m_nWait[nCntIce]--;

		if (m_nWait[nCntIce] <= 0)
		{// 0�����������
			m_nWait[nCntIce] = 0;
		}
		else
		{
			continue;
		}

		move.y -= 0.1f;

		// �ʒu�X�V
		pos += move;

		// �ʒu�ݒ�
		m_pObjX[nCntIce]->SetPosition(pos);

		// �ړ��ʐݒ�
		m_pObjX[nCntIce]->SetMove(move);

		// �G�t�F�N�g����
		CEffect3D::Create(pos, D3DXVECTOR3(move.x, -move.y * 2.0f, move.z), D3DXCOLOR(0.2f, 0.2f, 1.0f, 1.0f), fATKRange, 6, CEffect3D::MOVEEFFECT_SUB, CEffect3D::TYPE_NORMAL);

		if (CGame::GetObject3DMesh()->IsHit(pos) == true)
		//if (pos.y <= -50.0f)
		{
			if (m_pObjX[nCntIce] != NULL)
			{// NULL����Ȃ�������

				// ���I����
				CBallast::Create(D3DXVECTOR3(pos.x, pos.y + 5.0f, pos.z), DEFAULTMOVE_BALLAST, 8, 0.6f, CBallast::TYPE_ICE);

				if (nCntIce == m_nIdxBig)
				{// ����ȕX

					// �U��
					CManager::GetCamera()->SetShake(20, 0.0f, 10.0f);
					//CManager::GetObject3DMesh()->Damage(pos, (ATK_RANGE * 2.0f) * 10.0f, -100.0f);
				}
				else
				{
					//CManager::GetObject3DMesh()->Damage(pos, ATK_RANGE * 2.0f, -50.0f);
				}

				// �I�u�W�F�N�gX�̏I������
				m_pObjX[nCntIce]->Uninit();
				m_pObjX[nCntIce] = NULL;
				m_nNumAll--;
			}
			else
			{
				int n = 0;
			}
		}
	}

	// �����蔻��
	Collision();

}

//==========================================================================
// �ݒ菈��
//==========================================================================
bool CMagicIce::Set(void)
{
	// ����̔ԍ��ۑ�
	int nIdx = m_nNumAll;

	if (nIdx >= MAX_MAGICICE)
	{// �����𒴂��Ă�����
		return false;
	}

	// ��������
	m_pObjX[nIdx] = CObjectX::Create(
		m_nIdxXFile,
		D3DXVECTOR3(m_posOrigin.x + sinf(Random(-31, 31) * 0.1f) * Random(-RANGE, RANGE), m_posOrigin.y + Random(-200, 200) * 0.1f, m_posOrigin.z + cosf(Random(-31, 31) * 0.1f) * Random(-RANGE, RANGE)),
		D3DXVECTOR3(0.0f, 0.0f, 0.0f), false);

	if (m_pObjX[nIdx] == NULL)
	{// ���s�����Ƃ�

		delete m_pObjX[nIdx];
		m_pObjX[nIdx] = NULL;
		return false;
	}

	// �҂����Ԑݒ�
	m_nWait[nIdx] = 40;

	// �ړ��ʐݒ�
	m_pObjX[nIdx]->SetMove(D3DXVECTOR3(0.0f, -7.0f + Random(-20, 20) * 0.1f, 0.0f));

	if (rand() % 2 == 0)
	{
		m_nDirection[nIdx] = 1;
	}
	else
	{
		m_nDirection[nIdx] = -1;
	}

	if (m_nIdxBig == nIdx)
	{// �����̎�

		// �ʒu�ݒ�
		m_pObjX[nIdx]->SetPosition(D3DXVECTOR3(m_posOrigin.x, m_posOrigin.y + 100.0f, m_posOrigin.z));

		// �X�P�[���ݒ�
		m_pObjX[nIdx]->SetScale(D3DXVECTOR3(10.0f, 10.0f, 10.0f));

		// �ړ��ʐݒ�
		m_pObjX[nIdx]->SetMove(D3DXVECTOR3(0.0f, -6.0f, 0.0f));
	}

	// �p�[�e�B�N������
	my_particle::Create(m_pObjX[nIdx]->GetPosition(), my_particle::TYPE_ICE_SPAWN);

	// �������Z
	m_nNumAll++;
	return true;
}

//==========================================================================
// �����蔻��
//==========================================================================
void CMagicIce::Collision(void)
{
#if TOPCUR

	// �擪��ۑ�
	CObject *pObj = CObject::GetTop(mylib_const::ENEMY_PRIORITY);

	while (pObj != NULL)
	{// NULL������܂Ŗ������[�v

		// ���̃I�u�W�F�N�g���ꎞ�ۑ�
		CObject *pObjNext = pObj->GetNext();

		// ���S�̔���
		if (pObj->IsDeath() == true)
		{// ���S�t���O�������Ă�����

			// ���̃I�u�W�F�N�g����
			pObj = pObjNext;
			continue;
		}

		// ��ނ̎擾
		if (pObj->GetType() != CObject::TYPE_ENEMY)
		{// �G����Ȃ�������

			// ���̃I�u�W�F�N�g����
			pObj = pObjNext;
			continue;
		}

		for (int nCntIce = 0; nCntIce < MAX_MAGICICE; nCntIce++)
		{// ���ꂼ��̍X�V

			if (m_pObjX[nCntIce] == NULL)
			{// ���s�����Ƃ�
				continue;
			}

			// �ʒu�擾
			D3DXVECTOR3 pos = m_pObjX[nCntIce]->GetPosition();

			// �G�̈ʒu�擾
			D3DXVECTOR3 TargetPos = pObj->GetPosition();

			// �U���̔���
			float fATKRange = ATK_RANGE;

			if (nCntIce == m_nIdxBig)
			{// ����Ȃ�̎�����
				fATKRange *= 10.0f;
			}

			// ����T�C�Y�擾
			float fRadius = pObj->GetObjectChara()->GetRadius();

			if (SphereRange(pos, TargetPos, fATKRange, fRadius))
			{
				if (pObj->Hit(1) == true)
				{// ����ł���

					my_particle::Create(TargetPos, my_particle::TYPE_OFFSETTING);
					break;
				}
			}
		}

		// ���̃I�u�W�F�N�g����
		pObj = pObjNext;
	}

#endif
}

//==========================================================================
// �`�揈��
//==========================================================================
void CMagicIce::Draw(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	for (int nCntIce = 0; nCntIce < MAX_MAGICICE; nCntIce++)
	{
		if (m_pObjX[nCntIce] != NULL)
		{// NULL����Ȃ�������

			// �`�揈��
			m_pObjX[nCntIce]->Draw();
		}
	}

}
