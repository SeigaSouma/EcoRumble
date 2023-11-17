//=============================================================================
// 
//  ���̃t�B�[���h���� [waterfield.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "waterfield.h"
#include "manager.h"
#include "game.h"
#include "renderer.h"
#include "calculation.h"
#include "player.h"
#include "impactwave.h"
#include "particle.h"
#include "sound.h"

//==========================================================================
// �}�N����`
//==========================================================================

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CWaterField::CWaterField(int nPriority) : CMeshField(nPriority)
{
	m_fTexU = 0.0f;		// U�X�N���[���p
	m_fTexV = 0.0f;		// V�X�N���[���p
	memset(&m_nTimeInWater[0], 0, sizeof(m_nTimeInWater));	// ���ɓ����Ă鎞��
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CWaterField::~CWaterField()
{

}

//==========================================================================
// ����������
//==========================================================================
HRESULT CWaterField::Init(void)
{
	// ���b�V���t�B�[���h�̏���������
	CMeshField::Init();

	// ��ސݒ�
	SetType(TYPE_WATERFIELD);

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CWaterField::Uninit(void)
{
	// ���b�V���t�B�[���h�̏���������
	CMeshField::Uninit();
}

//==========================================================================
// �X�V����
//==========================================================================
void CWaterField::Update(void)
{
	// �擪��ۑ�
	CObject *pObj = CObject::GetTop(mylib_const::ENEMY_PRIORITY);

	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();
	int nCntObj = 0;
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

		if (pObj == CObject::GetObject())
		{// �����Ɠ�����������

			// ���̃I�u�W�F�N�g����
			pObj = pObjNext;
			continue;
		}

		// ��ނ̎擾
		if (pObj->GetType() != CObject::TYPE_ENEMY)
		{// �G����Ȃ�������

			if (pObj->GetType() != CObject::TYPE_PLAYER)
			{// �v���C���[����Ȃ�������
				// ���̃I�u�W�F�N�g����
				pObj = pObjNext;
				continue;
			}
		}

		// �Ώۂ̏��擾
		D3DXVECTOR3 PlayerPos = pObj->GetPosition();
		D3DXVECTOR3 PlayerPosOld = pObj->GetOldPosition();
		D3DXVECTOR3 PlayerRot = pObj->GetRotation();

		if (PlayerPos.y <= pos.y)
		{// ����������������

			if (PlayerPosOld.y > pos.y)
			{// �O��͏ゾ������

				if (pObj->GetType() == CObject::TYPE_PLAYER)
				{// �v���C���[��������

					CPlayer *pPlayer = CManager::GetScene()->GetPlayer();
					if (pPlayer != NULL)
					{
						pPlayer->IsSwitchInWater(true);
					}

					// �T�E���h�Đ�
					CManager::GetSound()->PlaySound(CSound::LABEL_SE_WATERIN);
				}

				CImpactWave::Create
				(
					D3DXVECTOR3(PlayerPos.x, pos.y + 1.0f, PlayerPos.z),	// �ʒu
					D3DXVECTOR3(0.0f, 0.0f, 0.0f),				// ����
					D3DXCOLOR(0.6f, 0.6f, 1.0f, 0.2f),			// �F
					20.0f,										// ��
					2.0f,										// ����
					40,											// ����
					4.2f,										// ���̈ړ���
					CImpactWave::TYPE_BLACK2,					// �e�N�X�`���^�C�v
					true										// ���Z�������邩
				);

				CImpactWave::Create
				(
					D3DXVECTOR3(PlayerPos.x, pos.y + 1.0f, PlayerPos.z),	// �ʒu
					D3DXVECTOR3(0.0f, 0.0f, 0.0f),				// ����
					D3DXCOLOR(0.6f, 0.6f, 1.0f, 0.2f),			// �F
					20.0f,										// ��
					2.0f,										// ����
					45,											// ����
					5.0f,										// ���̈ړ���
					CImpactWave::TYPE_BLACK2,					// �e�N�X�`���^�C�v
					true										// ���Z�������邩
				);

				CImpactWave::Create
				(
					D3DXVECTOR3(PlayerPos.x, pos.y + 1.0f, PlayerPos.z),	// �ʒu
					D3DXVECTOR3(0.0f, 0.0f, 0.0f),				// ����
					D3DXCOLOR(0.6f, 0.6f, 1.0f, 0.2f),			// �F
					20.0f,										// ��
					2.0f,										// ����
					50,											// ����
					6.0f,										// ���̈ړ���
					CImpactWave::TYPE_BLACK2,					// �e�N�X�`���^�C�v
					true										// ���Z�������邩
				);

				CImpactWave::Create
				(
					D3DXVECTOR3(
						PlayerPos.x + sinf(D3DX_PI + PlayerRot.y) * 50.0f,
						pos.y + 1.0f + 60.0f,
						PlayerPos.z + cosf(D3DX_PI + PlayerRot.y) * 50.0f),	// �ʒu
					D3DXVECTOR3(D3DX_PI, 0.0f, 0.0f),				// ����
					D3DXCOLOR(0.8f, 0.8f, 1.0f, 1.0f),			// �F
					20.0f,										// ��
					60.0f,										// ����
					30,											// ����
					2.0f,										// ���̈ړ���
					CImpactWave::TYPE_SMOKE,					// �e�N�X�`���^�C�v
					true										// ���Z�������邩
				);
			}

			// ���ɓ����Ă鎞�ԉ��Z
			if (pObj->GetType() == CObject::TYPE_ENEMY)
			{// �G��������
				m_nTimeInWater[nCntObj] = (m_nTimeInWater[nCntObj] + 1) % 30;
			}
			else if (pObj->GetType() == CObject::TYPE_PLAYER)
			{// �v���C���[��������
				m_nTimeInWater[nCntObj] = (m_nTimeInWater[nCntObj] + 1) % 25;
			}

			// �Ռ��g����
			if (m_nTimeInWater[nCntObj] == 0)
			{
				
				CImpactWave::Create
				(
					D3DXVECTOR3(
						PlayerPos.x + sinf(D3DX_PI + PlayerRot.y) * 50.0f,
						pos.y + 1.0f,
						PlayerPos.z + cosf(D3DX_PI + PlayerRot.y) * 50.0f),	// �ʒu
					D3DXVECTOR3(0.0f, 0.0f, 0.0f),				// ����
					D3DXCOLOR(0.6f, 0.6f, 1.0f, 0.2f),			// �F
					20.0f,										// ��
					2.0f,										// ����
					40,											// ����
					4.2f,										// ���̈ړ���
					CImpactWave::TYPE_BLACK2,					// �e�N�X�`���^�C�v
					true										// ���Z�������邩
				);

				CImpactWave::Create
				(
					D3DXVECTOR3(
						PlayerPos.x + sinf(D3DX_PI + PlayerRot.y) * 50.0f,
						pos.y + 1.0f + 30.0f,
						PlayerPos.z + cosf(D3DX_PI + PlayerRot.y) * 50.0f),	// �ʒu
					D3DXVECTOR3(D3DX_PI, 0.0f, 0.0f),				// ����
					D3DXCOLOR(0.8f, 0.8f, 1.0f, 1.0f),			// �F
					10.0f,										// ��
					30.0f,										// ����
					30,											// ����
					2.0f,										// ���̈ړ���
					CImpactWave::TYPE_SMOKE,					// �e�N�X�`���^�C�v
					true										// ���Z�������邩
				);

				// ���p�[�e�B�N������
				my_particle::Create(D3DXVECTOR3(
					PlayerPos.x + sinf(D3DX_PI + PlayerRot.y) * 50.0f,
					pos.y + 1.0f,
					PlayerPos.z + cosf(D3DX_PI + PlayerRot.y) * 50.0f), my_particle::TYPE_WATERIN);
			}
		}
		else
		{
			m_nTimeInWater[nCntObj] = 0;

			if (pObj->GetType() == CObject::TYPE_PLAYER)
			{// �v���C���[��������
				// �T�E���h��~
				CManager::GetSound()->StopSound(CSound::LABEL_SE_WATERIN);

				CPlayer *pPlayer = CManager::GetScene()->GetPlayer();
				if (pPlayer != NULL)
				{
					pPlayer->IsSwitchInWater(false);
				}
			}
		}

		// ���̃I�u�W�F�N�g����
		pObj = pObjNext;
		nCntObj++;
	}



	// �X�N���[��
	Scroll();

	// ���_���ݒ�
	SetVtx();
}

//==========================================================================
// �X�N���[������
//==========================================================================
void CWaterField::Scroll(void)
{
	m_fTexU += 0.001f;		// U�X�N���[���p
	m_fTexV += 0.001f;		// V�X�N���[���p

	if (m_fTexU >= 1.0f)
	{
		m_fTexU = 0.0f;
	}
	else if (m_fTexU <= 0.0f)
	{
		m_fTexU = 1.0f;
	}

	if (m_fTexV >= 1.0f)
	{
		m_fTexV = 0.0f;
	}
	else if (m_fTexV <= 0.0f)
	{
		m_fTexV = 1.0f;
	}
}

//==========================================================================
// �`�揈��
//==========================================================================
void CWaterField::Draw(void)
{
	//  �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	if (CManager::IsWireframe() == true)
	{
		pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);	// ���C���[�t���[�����[�h
	}

	// �`�揈��
	CMeshField::Draw();

	pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);	// ���߂郂�[�h
}

//==========================================================================
// ���_���ݒ菈��
//==========================================================================
void CWaterField::SetVtx(void)
{
	// ���_���X�V
	CMeshField::SetVtx();

	VERTEX_3D *pVtx;	// ���_���ւ̃|�C���^
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();
	int nHBlock = GetHeightBlock();
	int nWBlock = GetWidthBlock();

	// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// ���_���̐ݒ�
	for (int nCntHeight = 0; nCntHeight < nHBlock + 1; nCntHeight++)
	{//�c�̒��_�����J��Ԃ�

		for (int nCntWidth = 0; nCntWidth < nWBlock + 1; nCntWidth++)
		{// ���̒��_�����J��Ԃ�

			// ���_�J���[�̐ݒ�
			pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.6f);

			// �e�N�X�`�����W�̐ݒ�
			pVtx[0].tex = D3DXVECTOR2
			(
				((float)nCntWidth / (1.0f / (float)(nWBlock + 1))) * (1.0f / (float)(nWBlock + 1)) + m_fTexU,
				((float)nCntHeight / (1.0f / (float)(nHBlock + 1))) * (1.0f / (float)(nHBlock + 1)) + m_fTexV
			);
			pVtx += 1;
		}
	}

	// ���_�o�b�t�@���A�����b�N����
	pVtxBuff->Unlock();
}

//==========================================================================
// �I�u�W�F�N�g�̎擾
//==========================================================================
CWaterField *CWaterField::GetObject3DMesh(void)
{
	return this;
}
