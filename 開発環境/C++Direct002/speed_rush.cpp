//=============================================================================
// 
//  �������b�V������ [speed_rush.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "speed_rush.h"
#include "manager.h"
#include "renderer.h"
#include "calculation.h"
#include "objectX.h"
#include "objectChara.h"
#include "3D_Effect.h"
#include "particle.h"
#include "ballast.h"
#include "model.h"

//==========================================================================
// �}�N����`
//==========================================================================
#define OBJ_LIFE	(4)		// �I�u�W�F�̎���
#define RANGE		(100)	// �����_�������͈̔�
#define MOVE		(20.0f)	// �ړ���
#define ATK_RANGE	(10.0f)	// �U������
#define OFFSET		(-40.0f)	// �����蔻��̃I�t�Z�b�g

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================
const char *CSpeedRush::m_pModelFile = "data\\MODEL\\rushhand.x";	// ���f���t�@�C��
int CSpeedRush::m_nIdxXFile = 0;			// X�t�@�C���̃C���f�b�N�X�ԍ�

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CSpeedRush::CSpeedRush(int nPriority) : CObject(nPriority)
{
	m_posOrigin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// ���_
	m_rotOrigin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// ������
	m_nNumAll = 0;		// ����
	m_nLife = 0;		// ����

	for (int nCntRush = 0; nCntRush < MAX_SPEEDRUSH; nCntRush++)
	{
		m_nObjLife[nCntRush] = 0;	// ����
		m_pObjX[nCntRush] = NULL;	// �I�u�W�F�N�gX�̃|�C���^
	}
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CSpeedRush::~CSpeedRush()
{
	
}

//==========================================================================
// ���[�h����
//==========================================================================
HRESULT CSpeedRush::Load(void)
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
CSpeedRush *CSpeedRush::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, int nNum)
{
	// �����p�̃I�u�W�F�N�g
	CSpeedRush *pBallast = NULL;

	if (pBallast == NULL)
	{// NULL��������

		// �������̊m��
		pBallast = DEBUG_NEW CSpeedRush;

		//if (pBallast->GetID() < 0)
		//{// �������m�ۂɎ��s���Ă�����

		//	delete pBallast;
		//	return NULL;
		//}

		if (pBallast != NULL)
		{// �������̊m�ۂ��o���Ă�����\

			// �u��
			pos.y += 150.0f;

			// �ʒu�ݒ�
			pBallast->m_posOrigin = pos;

			// �����ݒ�
			rot.x = -0.5f;
			pBallast->m_rotOrigin = rot;

			// �����ݒ�
			pBallast->m_nLife = nNum;

			// ����������
			pBallast->Init();
		}

		return pBallast;
	}

	return NULL;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CSpeedRush::Init(void)
{
	// ��ނ̐ݒ�
	SetType(TYPE_BALLAST);

	for (int nCntRush = 0; nCntRush < MAX_SPEEDRUSH; nCntRush++)
	{
		m_nObjLife[nCntRush] = OBJ_LIFE;
	}

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CSpeedRush::Uninit(void)
{
	for (int nCntRush = 0; nCntRush < MAX_SPEEDRUSH; nCntRush++)
	{
		if (m_pObjX[nCntRush] != NULL)
		{// NULL����Ȃ�������

			// �I�u�W�F�N�gX�̏I������
			m_pObjX[nCntRush]->Uninit();
			m_pObjX[nCntRush] = NULL;
		}
	}

	// ���폜
	Release();
}

//==========================================================================
// �X�V����
//==========================================================================
void CSpeedRush::Update(void)
{
	// ���S�̔���
	if (IsDeath() == true)
	{// ���S�t���O�������Ă�����
		return;
	}

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

	for (int nCntRush = 0; nCntRush < MAX_SPEEDRUSH; nCntRush++)
	{// ���ꂼ��̍X�V

		if (m_pObjX[nCntRush] == NULL)
		{// NULL��������
			continue;
		}

		// �ʒu�擾
		D3DXVECTOR3 pos = m_pObjX[nCntRush]->GetPosition();

		// �ړ��ʎ擾
		D3DXVECTOR3 move = m_pObjX[nCntRush]->GetMove();

		// �����擾
		D3DXVECTOR3 rot = m_pObjX[nCntRush]->GetRotation();

		// �ʒu�X�V
		pos += move;

		// �ʒu�ݒ�
		m_pObjX[nCntRush]->SetPosition(pos);

		// �ړ��ʐݒ�
		m_pObjX[nCntRush]->SetMove(move);

		// �I�u�W�F�̎��������炷
		m_nObjLife[nCntRush]--;

		// �������s���������
		if (m_nObjLife[nCntRush] < 0)
		{
			if (m_pObjX[nCntRush] != NULL)
			{// NULL����Ȃ�������

				// �I�u�W�F�N�gX�̏I������
				m_pObjX[nCntRush]->Uninit();
				m_pObjX[nCntRush] = NULL;
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
bool CSpeedRush::Set(void)
{
	// ����̔ԍ��ۑ�
	int nIdx = m_nNumAll;

	if (nIdx >= MAX_SPEEDRUSH)
	{// �����𒴂��Ă�����
		return false;
	}

	// ��������
	m_pObjX[nIdx] = CObjectX::Create(
		m_nIdxXFile,
		D3DXVECTOR3(
			m_posOrigin.x + sinf(Random(-31, 31) * 0.1f) * Random(-RANGE, RANGE),
			m_posOrigin.y + cosf(Random(-31, 31) * 0.1f) * Random(-RANGE, RANGE),
			m_posOrigin.z + cosf(Random(-31, 31) * 0.1f) * Random(-RANGE, RANGE)),
		D3DXVECTOR3(0.0f, 0.0f, 0.0f), false);

	if (m_pObjX[nIdx] == NULL)
	{// ���s�����Ƃ�

		delete m_pObjX[nIdx];
		m_pObjX[nIdx] = NULL;
		Uninit();
		return false;
	}

	// �ړ��ʐݒ�
	m_pObjX[nIdx]->SetMove(D3DXVECTOR3(sinf(D3DX_PI + m_rotOrigin.y) * MOVE, cosf(D3DX_PI + m_rotOrigin.x) * MOVE, cosf(D3DX_PI + m_rotOrigin.y) * MOVE));

	// �����ݒ�
	m_pObjX[nIdx]->SetRotation(D3DXVECTOR3(m_rotOrigin.x, m_rotOrigin.y, Random(-314, 314) * 0.01f));

	// �������Z
	m_nNumAll++;
	return true;
}

//==========================================================================
// �����蔻��
//==========================================================================
void CSpeedRush::Collision(void)
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

			for (int nCntRush = 0; nCntRush < MAX_SPEEDRUSH; nCntRush++)
			{// ���ꂼ��̍X�V

				if (m_pObjX[nCntRush] == NULL)
				{// ���s�����Ƃ�
					continue;
				}

				// �ʒu�擾
				D3DXVECTOR3 pos = m_pObjX[nCntRush]->GetPosition();

				// �ʒu�擾
				D3DXVECTOR3 rot = m_pObjX[nCntRush]->GetRotation();

				// ���肷��p�[�c�̃}�g���b�N�X�擾
				D3DXMATRIX mtxWepon, mtxTrans, mtxOrigin, mtxRot;	// �v�Z�p�}�g���b�N�X�錾
				D3DXMatrixIdentity(&mtxWepon);

				// �����𔽉f����
				D3DXMatrixRotationYawPitchRoll(&mtxRot, rot.y, rot.x, rot.z);
				D3DXMatrixMultiply(&mtxWepon, &mtxWepon, &mtxRot);

				// ���̈ʒu�𔽉f����
				D3DXMatrixTranslation(&mtxOrigin, pos.x, pos.y, pos.z);
				D3DXMatrixMultiply(&mtxWepon, &mtxWepon, &mtxOrigin);

				// �I�t�Z�b�g�̈ʒu�𔽉f����
				D3DXMatrixTranslation(&mtxTrans, 0.0f, 0.0f, OFFSET);
				D3DXMatrixMultiply(&mtxWepon, &mtxTrans, &mtxWepon);

				// ����̈ʒu
				D3DXVECTOR3 weponpos = D3DXVECTOR3(mtxWepon._41, mtxWepon._42, mtxWepon._43);

				// ����T�C�Y�擾
				float fRadius = pObj->GetObjectChara()->GetRadius();

				//CEffect3D::Create(weponpos, D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f), ATK_RANGE, 2, CEffect3D::MOVEEFFECT_NONE, CEffect3D::TYPE_NORMAL);

				// �G�̈ʒu�擾
				D3DXMATRIX mtxRange = pObj->GetObjectChara()->GetModel()[0]->GetWorldMtx();
				D3DXVECTOR3 TargetPos = D3DXVECTOR3(mtxRange._41, mtxRange._42, mtxRange._43);

				if (SphereRange(weponpos, TargetPos, ATK_RANGE, fRadius))
				{
					// �I�u�W�F�N�gX�̏I������
					m_pObjX[nCntRush]->Uninit();
					m_pObjX[nCntRush] = NULL;
					m_nNumAll--;

					if(pObj->Hit(1) == true)
					{// ����ł���

						my_particle::Create(TargetPos, my_particle::TYPE_OFFSETTING);
					}
					break;
				}
			}

			// ���̃I�u�W�F�N�g����
			pObj = pObjNext;
		}
	}
#endif
}

//==========================================================================
// �`�揈��
//==========================================================================
void CSpeedRush::Draw(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	for (int nCntRush = 0; nCntRush < MAX_SPEEDRUSH; nCntRush++)
	{
		if (m_pObjX[nCntRush] != NULL)
		{// NULL����Ȃ�������

			// �`�揈��
			m_pObjX[nCntRush]->Draw();
		}
	}

}
