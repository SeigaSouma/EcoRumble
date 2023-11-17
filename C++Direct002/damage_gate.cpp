//=============================================================================
// 
//  �_���[�W�Q�[�g���� [damage_gate.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "damage_gate.h"
#include "manager.h"
#include "renderer.h"
#include "game.h"
#include "calculation.h"
#include "objectX.h"
#include "objectchara.h"
#include "3D_Effect.h"
#include "particle.h"
#include "calculation.h"

#include "input.h"
#include "power_gauge.h"
//==========================================================================
// �}�N����`
//==========================================================================
#define MODELFILE	"data\\MODEL\\damage_gate.x"	// ���f���t�@�C��
#define LENGTH		(2500.0f)						// ���_����̋���
#define ROT_MOVE	(D3DX_PI * 0.001f)				// ��]�ړ���

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================
int CDamageGate::m_nIdxXFile = 0;			// X�t�@�C���̃C���f�b�N�X�ԍ�

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CDamageGate::CDamageGate(int nPriority) : CObject(nPriority)
{
	m_posOrigin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// ���_
	m_rotOrigin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// ����
	m_fLength = 0.0f;								// ���_����̋���
	memset(&m_pObjX[0], NULL, sizeof(m_pObjX));				// �I�u�W�F�N�gX�̃|�C���^
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CDamageGate::~CDamageGate()
{
	
}

//==========================================================================
// ���[�h����
//==========================================================================
HRESULT CDamageGate::Load(void)
{
	// X�t�@�C���̃f�[�^�擾
	CXLoad *pXLoad = CScene::GetXLoad();

	// X�t�@�C���̃��[�h
	m_nIdxXFile = pXLoad->XLoad(MODELFILE);

	if (m_nIdxXFile <= 0)
	{// ���s���Ă�����
		return E_FAIL;
	}

	return S_OK;
}

//==========================================================================
// ��������
//==========================================================================
CDamageGate *CDamageGate::Create(void)
{
	// �����p�̃I�u�W�F�N�g
	CDamageGate *pBallast = NULL;

	if (pBallast == NULL)
	{// NULL��������

		// �������̊m��
		pBallast = DEBUG_NEW CDamageGate;

		if (pBallast != NULL)
		{// �������̊m�ۂ��o���Ă�����

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
HRESULT CDamageGate::Init(void)
{
	// ��ނ̐ݒ�
	SetType(TYPE_MAGIC_ICE);

	m_posOrigin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// ���_
	m_fLength = LENGTH;								// ���_����̋���

	float fRot = (D3DX_PI * 2.0f) / (float)MAX_DAMAGEGATE;

	for (int nCntGate = 0; nCntGate < MAX_DAMAGEGATE; nCntGate++)
	{// ���ꂼ��̍X�V

		// ��������
		m_pObjX[nCntGate] = CObjectX::Create(
			m_nIdxXFile,
			D3DXVECTOR3(m_posOrigin.x + sinf(fRot * nCntGate) * m_fLength, m_posOrigin.y, m_posOrigin.z + cosf(fRot * nCntGate) * m_fLength),
			D3DXVECTOR3(0.0f, 0.0f, 0.0f), false);

		if (m_pObjX[nCntGate] == NULL)
		{// ���s�����Ƃ�

			delete m_pObjX[nCntGate];
			m_pObjX[nCntGate] = NULL;
			return false;
		}
	}

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CDamageGate::Uninit(void)
{
	for (int nCntGate = 0; nCntGate < MAX_DAMAGEGATE; nCntGate++)
	{
		if (m_pObjX[nCntGate] != NULL)
		{// NULL����Ȃ�������

			// �I�u�W�F�N�gX�̏I������
			m_pObjX[nCntGate]->Uninit();
			m_pObjX[nCntGate] = NULL;
		}
	}

	// ���폜
	Release();
}

//==========================================================================
// �X�V����
//==========================================================================
void CDamageGate::Update(void)
{
	// 1������̊p�x
	float fRot = (D3DX_PI * 2.0f) / (float)MAX_DAMAGEGATE;

	// ��]������
	m_rotOrigin.y += ROT_MOVE;

	// �L�[�{�[�h���擾
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();

	if (pInputKeyboard->GetPress(DIK_UP) == true)
	{// �U��
		m_fLength += 5.0f;
	}
	if (pInputKeyboard->GetPress(DIK_DOWN) == true)
	{// �U��
		m_fLength -= 5.0f;
	}


	// �l�̐��K������
	ValueNormalize(m_fLength, LENGTH, 0.0f);


	for (int nCntGate = 0; nCntGate < MAX_DAMAGEGATE; nCntGate++)
	{// ���ꂼ��̍X�V

		if (m_pObjX[nCntGate] == NULL)
		{// NULL��������
			continue;
		}

		// �ʒu�擾
		D3DXVECTOR3 pos = m_pObjX[nCntGate]->GetPosition();

		// �ړ��ʎ擾
		D3DXVECTOR3 move = m_pObjX[nCntGate]->GetMove();

		// �����擾
		D3DXVECTOR3 rot = m_pObjX[nCntGate]->GetRotation();

		// �ʒu�X�V
		pos = D3DXVECTOR3(m_posOrigin.x + sinf(m_rotOrigin.y + fRot * nCntGate) * m_fLength, m_posOrigin.y, m_posOrigin.z + cosf(m_rotOrigin.y + fRot * nCntGate) * m_fLength);

		// �����X�V
		rot.y = fRot * nCntGate + m_rotOrigin.y;

		// �����ݒ�
		m_pObjX[nCntGate]->SetRotation(rot);

		// �ʒu�ݒ�
		m_pObjX[nCntGate]->SetPosition(pos);

		// �����蔻��
		Collision(nCntGate);
	}

}

//==========================================================================
// �����蔻��
//==========================================================================
void CDamageGate::Collision(int nCntGate)
{
	// �ʒu�擾
	D3DXVECTOR3 pos = m_pObjX[nCntGate]->GetPosition();

	// �����擾
	D3DXVECTOR3 rot = m_pObjX[nCntGate]->GetRotation();

	// �ő�l�擾
	D3DXVECTOR3 vtxMax = m_pObjX[nCntGate]->GetVtxMax();

	float fLength = sqrtf(vtxMax.x * vtxMax.x + vtxMax.z * vtxMax.z);	// �Ίp���̒���
	float fAngle = atan2f(vtxMax.x, vtxMax.z);							// �Ίp���̌���

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

			if (pObj->GetType() != CObject::TYPE_BOSS)
			{// �G����Ȃ�������

				// ���̃I�u�W�F�N�g����
				pObj = pObjNext;
				continue;
			}
		}

		// ���肷��l�p
		float fLen = fLength + pObj->GetObjectChara()->GetRadius();
		D3DXVECTOR3 LeftUp = D3DXVECTOR3(pos.x + sinf(rot.y - fAngle) * fLen, pos.y, pos.z + cosf(rot.y - fAngle) * fLen);
		D3DXVECTOR3 RightUp = D3DXVECTOR3(pos.x + sinf(rot.y + fAngle) * fLen, pos.y, pos.z + cosf(rot.y + fAngle) * fLen);
		D3DXVECTOR3 LeftDown = D3DXVECTOR3(pos.x + sinf(rot.y - D3DX_PI + fAngle) * fLen, pos.y, pos.z + cosf(rot.y - D3DX_PI + fAngle) * fLen);
		D3DXVECTOR3 RightDown = D3DXVECTOR3(pos.x + sinf(rot.y + D3DX_PI - fAngle) * fLen, pos.y, pos.z + cosf(rot.y + D3DX_PI - fAngle) * fLen);

		// �G�̈ʒu�擾
		D3DXVECTOR3 TargetPos = pObj->GetPosition();

		// ����T�C�Y�擾
		float fRadius = pObj->GetObjectChara()->GetRadius();

		// �l�p�̔���
		bool bHit = CollisionSquare(LeftUp, RightUp, LeftDown, RightDown, TargetPos);

		if (bHit == false)
		{// �������Ă�����

			// ���̃I�u�W�F�N�g����
			pObj = pObjNext;
			continue;
		}

		if (pObj->Hit(10) == true)
		{// ����ł���

			// �p�[�e�B�N������
			my_particle::Create(TargetPos, my_particle::TYPE_MAGIC_EXPLOSION);
			my_particle::Create(D3DXVECTOR3(TargetPos.x, TargetPos.y + 50.0f, TargetPos.z), my_particle::TYPE_EXPLOSION_START);
			CGame::GetPowerGauge()->SetChargeCount(5 * 6);
		}

		// ���̃I�u�W�F�N�g����
		pObj = pObjNext;
	}
}

//==========================================================================
// �`�揈��
//==========================================================================
void CDamageGate::Draw(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	for (int nCntGate = 0; nCntGate < MAX_DAMAGEGATE; nCntGate++)
	{
		if (m_pObjX[nCntGate] != NULL)
		{// NULL����Ȃ�������

			// �`�揈��
			m_pObjX[nCntGate]->Draw();
		}
	}

}
