//=============================================================================
// 
//  ���������� [explosion_sphere.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "explosion_sphere.h"
#include "game.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "input.h"
#include "objectchara.h"
#include "calculation.h"
#include "particle.h"
#include "debugproc.h"
#include "impactwave.h"
#include "elevation.h"

//==========================================================================
// �}�N����`
//==========================================================================
#define TEXTURE		"data\\TEXTURE\\image.png"	// �e�N�X�`��
#define POS_MESHDOME	(0.0f)
#define LIFE			(90)
#define ALPHALIFE		(60)
#define IMPACT_DISTANCE	(100.0f)

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CExplosionSphere::CExplosionSphere(int nPriority) : CObject3DMesh(nPriority)
{
	m_nTexIdx = 0;		// �e�N�X�`���̃C���f�b�N�X�ԍ�
	m_nLife = 0;		// ����
	m_fSizeDest = 0.0f;	// �ڕW�̃T�C�Y
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CExplosionSphere::~CExplosionSphere()
{

}

//==========================================================================
// ��������
//==========================================================================
CExplosionSphere *CExplosionSphere::Create(D3DXVECTOR3 pos, float fSizeDest)
{
	// �����p�̃I�u�W�F�N�g
	CExplosionSphere *pObjMeshField = NULL;

	if (pObjMeshField == NULL)
	{// NULL��������

		// �������̊m��
		pObjMeshField = DEBUG_NEW CExplosionSphere();

		//if (pObjMeshField->GetID() < 0)
		//{// �������m�ۂɎ��s���Ă�����

		//	delete pObjMeshField;
		//	return NULL;
		//}

		if (pObjMeshField != NULL)
		{// �������̊m�ۂ��o���Ă�����

			// �e�N�X�`���̊��蓖��
			pObjMeshField->m_nTexIdx = CManager::GetTexture()->Regist(TEXTURE);

			// �e�N�X�`���̊��蓖��
			pObjMeshField->BindTexture(pObjMeshField->m_nTexIdx);

			// �ʒu�E����
			pObjMeshField->SetPosition(pos);
			pObjMeshField->SetWidthBlock(16);
			pObjMeshField->SetHeightBlock(16);
			pObjMeshField->SetWidthLen(POS_MESHDOME);
			pObjMeshField->SetHeightLen(POS_MESHDOME);
			pObjMeshField->m_fSizeDest = fSizeDest;

			// ����������
			pObjMeshField->Init();
		}

		return pObjMeshField;
	}

	return NULL;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CExplosionSphere::Init(void)
{
	HRESULT hr;

	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// ��ސݒ�
	SetType(TYPE_EXPLOSIONSPHERE);

	// ����
	m_nLife = LIFE;

	// �I�u�W�F�N�g3D���b�V���̏���������
	hr = CObject3DMesh::Init(CObject3DMesh::TYPE_SPHERE);

	if (FAILED(hr))
	{// ���s���Ă�����
		return E_FAIL;
	}

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CExplosionSphere::Uninit(void)
{
	////delete m_pVtxPos;
	//m_pVtxPos = NULL;

	// �I������
	CObject3DMesh::Uninit();
}

//==========================================================================
// �X�V����
//==========================================================================
void CExplosionSphere::Update(void)
{
	// �L�[�{�[�h���擾
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();

	// �����擾
	D3DXVECTOR3 rot = GetRotation();

	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();

	// �F�擾
	D3DXCOLOR col = GetColor();

	// �������擾
	float fWidthLen = GetWidthLen();

	// �c�����擾
	float fHeightLen = GetHeightLen();

	// ��]
	rot.y += D3DX_PI * 0.0025f;

	// �p�x�̐��K��
	RotNormalize(rot.y);

	// ���������炷
	m_nLife--;

	if (m_nLife <= LIFE)
	{// �K��l�����������

		// �s�����x�X�V
		col.a = (float)m_nLife / (float)ALPHALIFE;
	}

	// �����␳
	if (m_nLife <= LIFE - 10)
	{
		InertiaCorrection(fWidthLen, m_fSizeDest, 0.2f);
		InertiaCorrection(fHeightLen, m_fSizeDest, 0.2f);
	}

	// �Ռ��g����
	if (m_nLife == LIFE - 6 ||
		m_nLife == LIFE - 12)
	{
		CImpactWave::Create
		(
			D3DXVECTOR3(pos.x, pos.y - (m_fSizeDest * 0.5f - 20.0f), pos.z),	// �ʒu
			D3DXVECTOR3(0.0f, 0.0f, 0.0f),				// ����
			D3DXCOLOR(1.0f, 0.4f, 0.1f, 0.4f),			// �F
			60.0f,										// ��
			10.0f,										// ����
			30,											// ����
			20.0f,										// ���̈ړ���
			CImpactWave::TYPE_BLACK2,					// �e�N�X�`���^�C�v
			true
		);
	}

	// �p�[�e�B�N������
	if (m_nLife >= 40)
	{
		int nRand = (int)m_fSizeDest;
		// �p�[�e�B�N��
		my_particle::Create(
			D3DXVECTOR3(pos.x + Random(-nRand, nRand),
				pos.y + Random(-nRand, nRand),
				pos.z + Random(-nRand, nRand)), my_particle::TYPE_EXPLOSION_REMAIN);

		pos.y -= fHeightLen;
		my_particle::Create(pos, my_particle::TYPE_EXPLOSION_REMAIN);
	}

	// �����ݒ�
	SetWidthLen(fWidthLen);
	SetHeightLen(fHeightLen);

	// �����ݒ�
	SetRotation(rot);

	// �F�ݒ�
	SetColor(col);

	if (m_nLife == 80)
	{
		// �����蔻��
		//CManager::GetObject3DMesh()->Damage(pos, m_fSizeDest * 2.5f, -150.0f, CElevation::DAMAGE_RATIO);

		// �������
		CGame::GetObject3DMesh()->SetWave
		(
			pos,												// �ʒu
			D3DXVECTOR3(0.0f, 0.0f, 0.0f),						// ����
			D3DXVECTOR3(0.0f, 0.0f, 0.0f),						// �ړ���
			D3DXVECTOR2(m_fSizeDest * 6.0f, m_fSizeDest * 6.0f),									// ����͈�
			CGame::GetObject3DMesh()->GetWidthLen() * 3.0f,	// �g�����̕p�x
			80.0f,												// �g�̍���
			D3DX_PI * 0.05f,									// �g�łp�x
			60 * 1,												// ��������
			CElevation::WAVETYPE_RIPPLE,						// ���
			CElevation::TYPE_PLAYER,							// �v���C���[�̎��
			1
		);


		//CManager::GetObject3DMesh()->Damage(pos, m_fSizeDest * 2.5f, -300.0f, CElevation::DAMAGE_RATIO);


		//CManager::GetObject3DMesh()->SetWave(pos, m_fSizeDest * 6.0f, CManager::GetObject3DMesh()->GetWidthLen() * 8.0f, 80.0f, D3DX_PI * 0.025f, 40);


		//CManager::GetObject3DMesh()->SetWave(pos, m_fSizeDest * 6.0f, CManager::GetObject3DMesh()->GetWidthLen() * 3.0f, 80.0f, D3DX_PI * 0.025f, 100);



		//CManager::GetObject3DMesh()->SetWave(pos, m_fSizeDest * 8.0f, CManager::GetObject3DMesh()->GetWidthLen() * 4.0f, -100.0f, D3DX_PI * 0.025f, 60);
		//CManager::GetObject3DMesh()->SetWave(pos, m_fSizeDest * 8.0f, CManager::GetObject3DMesh()->GetWidthLen() * 8.0f, -100.0f, D3DX_PI * 0.025f, 60);
		Collision();
	}


	// ���_���ݒ�
	SetVtx();

	if (m_nLife <= 0)
	{// �������s������
		Uninit();
	}
}


//==========================================================================
// �����蔻��
//==========================================================================
void CExplosionSphere::Collision(void)
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

		// �ʒu�擾
		D3DXVECTOR3 pos = GetPosition();

		// �G�̈ʒu�擾
		D3DXVECTOR3 TargetPos = pObj->GetPosition();

		// ����T�C�Y�擾
		float fRadius = pObj->GetObjectChara()->GetRadius();

		if (SphereRange(pos, TargetPos, m_fSizeDest * 2.0f, fRadius))
		{
			if (pObj->Hit(3) == true)
			{// ����ł���

				// �p�[�e�B�N������
				my_particle::Create(TargetPos, my_particle::TYPE_OFFSETTING);
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
void CExplosionSphere::Draw(void)
{
	//  �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// ���C�e�B���O�𖳌��ɂ���
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// �A���t�@�e�X�g��L���ɂ���
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	// �`�揈��
	CObject3DMesh::Draw();

	// ���u�����f�B���O�����ɖ߂�
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// ���C�e�B���O��L���ɂ���
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	// �A���t�@�e�X�g�𖳌��ɂ���
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
}

//==========================================================================
// ���_���ݒ菈��
//==========================================================================
void CExplosionSphere::SetVtx(void)
{
	// ���_���X�V
	CObject3DMesh::SetVtx();
}
