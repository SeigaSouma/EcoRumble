//=============================================================================
// 
//  ������ [grassblades.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "grassblades.h"
#include "manager.h"
#include "renderer.h"
#include "game.h"
#include "calculation.h"
#include "meshwall.h"
#include "elevation.h"
#include "player.h"

//==========================================================================
// �}�N����`
//==========================================================================
#define RANGELEN		(300.0f)		// �͈͓��̒���
//#define RANGELEN		(40.0f)		// �͈͓��̒���
#define RANGE			(30.0f)
#define VECWIND			(0.25f)		// ���̋���
#define INT_WINDCHANGE	(240)		// ���̐؂�ւ��Ԋu
#define GRASSTEXTURE	"data\\TEXTURE\\grassblades_01.png"

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================
int CGrassBlades::m_nTexIdx = 0;		// �e�N�X�`���̃C���f�b�N�X�ԍ�

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CGrassBlades::CGrassBlades(int nPriority) : CObject(nPriority)
{
	// �l�̃N���A
	memset(&m_sGrassInfo[0], NULL, sizeof(m_sGrassInfo));	// �����
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CGrassBlades::~CGrassBlades()
{
	
}

//==========================================================================
// ��������
//==========================================================================
CGrassBlades *CGrassBlades::Create(D3DXVECTOR3 pos)
{
	// �����p�̃I�u�W�F�N�g
	CGrassBlades *pGrass = NULL;

	if (pGrass == NULL)
	{// NULL��������

		// �������̊m��
		pGrass = DEBUG_NEW CGrassBlades;

		if (pGrass != NULL)
		{// �������̊m�ۂ��o���Ă�����

			// �ʒu�ݒ�
			pGrass->SetPosition(pos);

			// ����������
			pGrass->Init();
		}
		else
		{
			return NULL;
		}

		return pGrass;
	}

	return NULL;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CGrassBlades::Init(void)
{
	// ��ނ̐ݒ�
	SetType(TYPE_GRASSBLADES);

	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();

	for (int nCntObj = 0; nCntObj < MAX_GRASSBLADES; nCntObj++)
	{
		// ���b�V���E�H�[���̐���
		float fHeightLen = (float)Random(-90, 90) * 0.1f + 18.0f;
		fHeightLen *= 3.0f;
		m_sGrassInfo[nCntObj].pMeshWall = CMeshWall::Create(
			D3DXVECTOR3(
				pos.x + sinf(Random(-31, 31) * 0.1f) * Random(-RANGELEN, RANGELEN),
				pos.y,
				pos.z + cosf(Random(-31, 31) * 0.1f) * Random(-RANGELEN, RANGELEN)),
			D3DXVECTOR3(0.0f, Random(-31, 31) * 0.1f, 0.0f),
			//D3DXVECTOR3(0.0f, 0.0f, 0.0f),
			40.0f, fHeightLen,
			1, 1, 
			(CMeshWall::TYPE_GRASS), GRASSTEXTURE);

		if (m_sGrassInfo[nCntObj].pMeshWall == NULL)
		{// ���s���Ă�����
			Uninit();
			return E_FAIL;
		}

		// ���̏��擾
		D3DXVECTOR3 posGrass = m_sGrassInfo[nCntObj].pMeshWall->GetPosition();

		// �����擾
		bool bLand = false;
		posGrass.y = CGame::GetObject3DMesh()->GetHeight(posGrass, D3DXVECTOR3(0.0f, 0.0f, 0.0f), bLand, TYPE_GRASSBLADES);

		// �ʒu�ݒ�
		m_sGrassInfo[nCntObj].pMeshWall->SetPosition(posGrass);

		// ���̌��������߂�
		m_sGrassInfo[nCntObj].windDirection.x = Random(-40, 40) * 0.01f;
		m_sGrassInfo[nCntObj].windDirection.z = Random(-40, 40) * 0.01f;

		// ���_���擾
		D3DXVECTOR3 WallRot = m_sGrassInfo[nCntObj].pMeshWall->GetRotation();
		D3DXVECTOR2 *pVtxTex = m_sGrassInfo[nCntObj].pMeshWall->GetVtxTex();
		D3DXVECTOR3 *pVtxPos = m_sGrassInfo[nCntObj].pMeshWall->GetVtxPos();
		int nHeight = m_sGrassInfo[nCntObj].pMeshWall->GetHeightBlock();
		int nWidth = m_sGrassInfo[nCntObj].pMeshWall->GetWidthBlock();

		for (int nCntHeight = 0; nCntHeight < nHeight + 1; nCntHeight++)
		{//�c�̒��_�����J��Ԃ�

			for (int nCntWidth = 0; nCntWidth < nWidth + 1; nCntWidth++)
			{// ���̒��_�����J��Ԃ�

				// ����̒��_
				int nNowVtx = nCntWidth + (nCntHeight * (nWidth + 1));

				// �e�N�X�`�����W�ݒ�
				pVtxTex[nNowVtx] = D3DXVECTOR2
				(
					((float)nCntWidth / (1.0f / (float)(nWidth + 1))) * (1.0f / (float)(nWidth + 1)),
					(1.0f / (float)(nHeight) * nCntHeight)
				);
			}
		}

	}

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CGrassBlades::Uninit(void)
{
	for (int nCntObj = 0; nCntObj < MAX_GRASSBLADES; nCntObj++)
	{
		if (m_sGrassInfo[nCntObj].pMeshWall != NULL)
		{// NULL����Ȃ�������

			// �I�u�W�F�N�gX�̏I������
			//m_sGrassInfo[nCntObj].pMeshWall->Uninit();
			m_sGrassInfo[nCntObj].pMeshWall = NULL;
		}
	}

	// ���폜
	Release();
}

//==========================================================================
// �X�V����
//==========================================================================
void CGrassBlades::Update(void)
{
	bool bStageEnd = false;
	if (CManager::GetScene()->GetWaveManager()->IsStageFrag() == false)
	{// �X�e�[�W�ύX��
		bStageEnd = true;
	}

	// �v���C���[���
	CPlayer *pPlayer = CManager::GetScene()->GetPlayer();

	if (pPlayer == NULL)
	{// NULL��������
		return;
	}

	// �v���C���[�̈ʒu�擾
	D3DXVECTOR3 posPlayer = pPlayer->GetPosition();

	// ���_�擾
	D3DXVECTOR3 pos = GetPosition();

	// �͈͂̔���
	bool bRange = false;

	// �~�̔���
	bRange = CircleRange(posPlayer, pos, pPlayer->GetRadius(), RANGELEN);

	for (int nCntObj = 0; nCntObj < MAX_GRASSBLADES; nCntObj++)
	{
		// ���̏��擾
		D3DXVECTOR3 posGrass = m_sGrassInfo[nCntObj].pMeshWall->GetPosition();
		D3DXVECTOR3 rotGrass = m_sGrassInfo[nCntObj].pMeshWall->GetRotation();

		// �����擾
		if (bStageEnd == false)
		{
			bool bLand = false;
			posGrass.y = CGame::GetObject3DMesh()->GetHeight(posGrass, D3DXVECTOR3(0.0f, 0.0f, 0.0f), bLand, TYPE_GRASSBLADES);
		}

		// ���̈ʒu�ݒ�
		m_sGrassInfo[nCntObj].pMeshWall->SetPosition(posGrass);

		if (bRange == true)
		{// �v���C���[���͈͓��̏ꍇ

			// �|��鏈��
			Falling(nCntObj);
		}
		else
		{
			// �ڕW�̌����ݒ�
			m_sGrassInfo[nCntObj].rotDest.x = m_sGrassInfo[nCntObj].windRot.x;
			m_sGrassInfo[nCntObj].rotDest.z = m_sGrassInfo[nCntObj].windRot.z;
			m_sGrassInfo[nCntObj].bHit = false;
		}

		// ���œ|��鏈��
		WindFalling(nCntObj);

		// ���������߂�
		float fDiffX = m_sGrassInfo[nCntObj].rotDest.x - rotGrass.x;
		float fDiffZ = m_sGrassInfo[nCntObj].rotDest.z - rotGrass.z;

		// �����̐��K��
		RotNormalize(fDiffX);
		RotNormalize(fDiffZ);

		// ������␳
		rotGrass.x += fDiffX * 0.05f;
		rotGrass.z += fDiffZ * 0.05f;

		// �����̐��K��
		RotNormalize(rotGrass);

		// �����ݒ�
		m_sGrassInfo[nCntObj].pMeshWall->SetRotation(rotGrass);
	}
}

//==========================================================================
// �|��鏈��
//==========================================================================
void  CGrassBlades::Falling(int nCntObj)
{
	// �v���C���[���
	CPlayer *pPlayer = CManager::GetScene()->GetPlayer();

	if (pPlayer == NULL)
	{// NULL��������
		return;
	}

	// �v���C���[�̏��擾
	D3DXVECTOR3 posPlayer = pPlayer->GetPosition();
	D3DXVECTOR3 rotPlayer = pPlayer->GetRotation();
	float fPlayerRadius = pPlayer->GetRadius();

	// ���̏��擾
	D3DXVECTOR3 pos = m_sGrassInfo[nCntObj].pMeshWall->GetPosition();
	D3DXVECTOR3 rot = m_sGrassInfo[nCntObj].pMeshWall->GetRotation();

	if (CircleRange(posPlayer, pos, fPlayerRadius, RANGE) == false)
	{// ����ɓ����ĂȂ�������
		// �ڕW�̌����ݒ�
		m_sGrassInfo[nCntObj].rotDest.x = m_sGrassInfo[nCntObj].windRot.x;
		m_sGrassInfo[nCntObj].rotDest.z = m_sGrassInfo[nCntObj].windRot.z;
		m_sGrassInfo[nCntObj].bHit = false;
		return;
	}

	// �x�N�g�������߂�
	D3DXVECTOR3 impactDirection = posPlayer - pos;
	//D3DXVec3Normalize(&impactDirection, &impactDirection);

	// �v���C���[�Ƒ��̋���
	float fNowLength =
		sqrtf((pos.x - posPlayer.x) * (pos.x - posPlayer.x)
			+ (pos.z - posPlayer.z) * (pos.z - posPlayer.z));

	if (fNowLength < 0)
	{
		int n = 0;
	}

	if (fNowLength >= RANGE + fPlayerRadius)
	{// �͈͊O
		// �ڕW�̌����ݒ�
		m_sGrassInfo[nCntObj].rotDest.x = m_sGrassInfo[nCntObj].windRot.x;
		m_sGrassInfo[nCntObj].rotDest.z = m_sGrassInfo[nCntObj].windRot.z;
		m_sGrassInfo[nCntObj].bHit = false;
		return;
	}

	// �ڕW�̊p�x�����߂�
	//float fMaxAngle = atan2f(impactDirection.x, impactDirection.z);
	float fMaxAngle = atan2f((posPlayer.x - pos.x), (posPlayer.z - pos.z));

	// �����̊��������]�p�x��␳
	float fAngle = ((RANGE + fPlayerRadius) - fNowLength) / (RANGE + fPlayerRadius);
	fAngle = fMaxAngle * fAngle;

	// �ڕW�̌����ݒ�
	m_sGrassInfo[nCntObj].rotDest.x = sinf(D3DX_PI/* * -0.5f*/ + fAngle);
	m_sGrassInfo[nCntObj].rotDest.z = cosf(D3DX_PI * -0.5f + fAngle);
	/*m_sGrassInfo[nCntObj].rotDest.x = cosf(fAngle) * sinf(rotPlayer.y);
	m_sGrassInfo[nCntObj].rotDest.z = cosf(fAngle) * cosf(rotPlayer.y);*/
	/*m_sGrassInfo[nCntObj].rotDest.x = fAngle;
	m_sGrassInfo[nCntObj].rotDest.z = fAngle;*/
	m_sGrassInfo[nCntObj].bHit = true;
	RotNormalize(m_sGrassInfo[nCntObj].windRot);

}

//==========================================================================
// ���ŗh��鏈��
//==========================================================================
void CGrassBlades::WindFalling(int nCntObj)
{
	// ���̃J�E���^�[���Z
	switch (m_sGrassInfo[nCntObj].nDirection)
	{
	case 0:
		m_sGrassInfo[nCntObj].nCntWind++;

		if (m_sGrassInfo[nCntObj].nCntWind == INT_WINDCHANGE)
		{// �K��J�E���g�ɒB������
			m_sGrassInfo[nCntObj].nDirection = m_sGrassInfo[nCntObj].nDirection ? 0 : 1;
		}
		break;

	case 1:
		m_sGrassInfo[nCntObj].nCntWind--;

		if (m_sGrassInfo[nCntObj].nCntWind == -INT_WINDCHANGE)
		{// �K��J�E���g�ɒB������
			m_sGrassInfo[nCntObj].nDirection = m_sGrassInfo[nCntObj].nDirection ? 0 : 1;
		}
		break;

	default:
		break;
	}

	// ���ŗh�炷
	m_sGrassInfo[nCntObj].windRot.x = (m_sGrassInfo[nCntObj].windDirection.x / (float)INT_WINDCHANGE) * m_sGrassInfo[nCntObj].nCntWind;
	m_sGrassInfo[nCntObj].windRot.z = (m_sGrassInfo[nCntObj].windDirection.z / (float)INT_WINDCHANGE) * m_sGrassInfo[nCntObj].nCntWind;

	// �p�x�̐��K��
	RotNormalize(m_sGrassInfo[nCntObj].windRot);
}

//==========================================================================
// �`�揈��
//==========================================================================
void CGrassBlades::Draw(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// �w�ʂ̃J�����O�Ȃ�
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// �A���t�@�e�X�g��L���ɂ���
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	for (int nCntObj = 0; nCntObj < MAX_GRASSBLADES; nCntObj++)
	{
		if (m_sGrassInfo[nCntObj].pMeshWall != NULL)
		{// NULL����Ȃ�������

			// �`�揈��
			m_sGrassInfo[nCntObj].pMeshWall->Draw();
		}
	}

	// �A���t�@�e�X�g�𖳌��ɂ���
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	// �J�����O�̃f�t�H���g
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}
