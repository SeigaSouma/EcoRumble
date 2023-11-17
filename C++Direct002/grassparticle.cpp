//=============================================================================
// 
//  ���̃p�[�e�B�N������ [grassparticle.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "game.h"
#include "grassparticle.h"
#include "manager.h"
#include "renderer.h"
#include "calculation.h"
#include "elevation.h"

//==========================================================================
// �}�N����`
//==========================================================================
#define LIFE	(40)	// ����
#define WIDTHLEN	(2.5f * 1.2f)	// ����
#define HEIGHTLEN	(10.0f * 1.2f)	// �c��
#define TEXTURE	"grassblades_01"	// ����

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================
int CGrassParticle::m_nTexIdx = 0;		// �e�N�X�`���̃C���f�b�N�X�ԍ�

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CGrassParticle::CGrassParticle(int nPriority) : CObject(nPriority)
{
	m_posOrigin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// ���_
	m_nNumAll = 0;		// ����
	memset(&m_nLife[0], 0, sizeof(m_nLife));	// ����
	memset(&m_nMaxLife[0], 0, sizeof(m_nMaxLife));	// �ő����
	memset(&m_pObjMesh[0], NULL, sizeof(m_pObjMesh));	// 3D���b�V���̃|�C���^
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CGrassParticle::~CGrassParticle()
{
	
}

//==========================================================================
// ��������
//==========================================================================
CGrassParticle *CGrassParticle::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR2 move, int nNum)
{
	// �����p�̃I�u�W�F�N�g
	CGrassParticle *pBallast = NULL;

	if (pBallast == NULL)
	{// NULL��������

		// �������̊m��
		pBallast = DEBUG_NEW CGrassParticle;

		if (pBallast != NULL)
		{// �������̊m�ۂ��o���Ă�����

			if (nNum >= MAX_GRASSPARTICLE)
			{
				nNum = MAX_GRASSPARTICLE;
			}

			// �����ݒ�
			pBallast->m_nNumAll = nNum;

			// �ʒu�ݒ�
			pBallast->m_posOrigin = pos;

			// ���̌���
			pBallast->m_rotOrigin = rot;;

			// ���̈ړ��ʐݒ�
			pBallast->m_moveOrigin = move;

			// ����������
			HRESULT hr = pBallast->Init();
			if (FAILED(hr))
			{
				return NULL;
			}
		}

		return pBallast;
	}

	return NULL;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CGrassParticle::Init(void)
{
	// ��ނ̐ݒ�
	SetType(TYPE_BALLAST);

	for (int nCntGrass = 0; nCntGrass < m_nNumAll; nCntGrass++)
	{
		// �ړ��ʐݒ�
		m_move[nCntGrass] = D3DXVECTOR3(
			sinf(Random(-30, 30) * 0.01f + m_rotOrigin.y) * m_moveOrigin.x,
			m_moveOrigin.y * (Random(800, 1000) * 0.001f),
			cosf(Random(-30, 30) * 0.01f + m_rotOrigin.y) * m_moveOrigin.x);

		// ��]�ړ���
		m_rotMove[nCntGrass] = D3DXVECTOR3(
			(Random(-400, 400) * 0.0001f) * D3DX_PI,
			(Random(-400, 400) * 0.0001f) * D3DX_PI,
			(Random(-400, 400) * 0.0001f) * D3DX_PI);

		// ��������
		m_pObjMesh[nCntGrass] = CObject3DMesh::Create
		(
			D3DXVECTOR3(
				m_posOrigin.x + sinf(Random(-314, 314) * 0.01f) * m_moveOrigin.x,
				m_posOrigin.y + sinf(Random(-314, 314) * 0.01f) * m_moveOrigin.y,
				m_posOrigin.z + cosf(Random(-314, 314) * 0.01f) * m_moveOrigin.x),
			D3DXVECTOR3(Random(-31, 31) * 0.1f, Random(-31, 31) * 0.1f, Random(-31, 31) * 0.1f),
			WIDTHLEN, HEIGHTLEN,
			1, 3,
			CObject3DMesh::TYPE_FIELD,
			TEXTURE
		);

		if (m_pObjMesh[nCntGrass] == NULL)
		{// ���s�����Ƃ�

			// 3D���b�V���̏I������
			delete m_pObjMesh[nCntGrass];
			m_pObjMesh[nCntGrass] = NULL;
			return E_FAIL;
		}

		// �����ݒ�
		m_nLife[nCntGrass] = LIFE + Random(0, 10);
		m_nMaxLife[nCntGrass] = m_nLife[nCntGrass];

		// �F�ݒ�
		D3DXCOLOR *pVtxCol = m_pObjMesh[nCntGrass]->GetVtxCol();
		D3DXVECTOR3 *pVtxPos = m_pObjMesh[nCntGrass]->GetVtxPos();
		int nWidth = m_pObjMesh[nCntGrass]->GetWidthBlock();
		int nHeight = m_pObjMesh[nCntGrass]->GetHeightBlock();

		for (int nCntHeight = 0; nCntHeight < nHeight + 1; nCntHeight++)
		{//�c�̒��_�����J��Ԃ�

			for (int nCntWidth = 0; nCntWidth < nWidth + 1; nCntWidth++)
			{// ���̒��_�����J��Ԃ�

				// ����̒��_
				int nNowVtx = nCntWidth + (nCntHeight * (nWidth + 1));
				float fLen = ((1.0f / (float)nHeight) * (WIDTHLEN * 0.5f)) * nCntHeight;
				int nAngle = -1;

				if (nCntWidth % 2 == 0)
				{
					nAngle *= -1;
				}

				// ���_���W�ݒ�
				pVtxPos[nNowVtx] = D3DXVECTOR3
				(
					pVtxPos[nNowVtx].x + fLen * nAngle,
					pVtxPos[nNowVtx].y,
					pVtxPos[nNowVtx].z
				);
				int n = 0;
			}
		}

		for (int nCntVtx = 0; nCntVtx < m_pObjMesh[nCntGrass]->GetNumVertex(); nCntVtx++)
		{
			// ���_�J���[�ݒ�
			pVtxCol[nCntVtx] = D3DXCOLOR(0.2f, 0.8f, 0.2f, 1.0f);
		}
	}

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CGrassParticle::Uninit(void)
{
	for (int nCntGrass = 0; nCntGrass < MAX_GRASSPARTICLE; nCntGrass++)
	{
		if (m_pObjMesh[nCntGrass] != NULL)
		{// NULL����Ȃ�������

			// 3D���b�V���̏I������
			m_pObjMesh[nCntGrass]->Uninit();
			m_pObjMesh[nCntGrass] = NULL;
		}
	}

	// ���폜
	CObject::Release();
}

//==========================================================================
// �I������
//==========================================================================
void CGrassParticle::Release(int nIdx)
{
	if (m_pObjMesh[nIdx] != NULL)
	{// NULL����Ȃ�������

		// 3D���b�V���̏I������
		m_pObjMesh[nIdx]->Uninit();
		m_pObjMesh[nIdx] = NULL;
	}

}

//==========================================================================
// �X�V����
//==========================================================================
void CGrassParticle::Update(void)
{
	for (int nCntGrass = 0; nCntGrass < m_nNumAll; nCntGrass++)
	{
		if (m_pObjMesh[nCntGrass] == NULL)
		{// NULL��������
			continue;
		}

		// �ʒu�擾
		D3DXVECTOR3 pos = m_pObjMesh[nCntGrass]->GetPosition();

		// �����擾
		D3DXVECTOR3 rot = m_pObjMesh[nCntGrass]->GetRotation();

		// ���擾
		float fWidthLen = m_pObjMesh[nCntGrass]->GetWidthLen();
		float fHeightLen = m_pObjMesh[nCntGrass]->GetHeightLen();

		// �ʒu�X�V
		pos += m_move[nCntGrass];

		// �����X�V
		rot += m_rotMove[nCntGrass];

		// �p�x�̐��K��
		RotNormalize(rot);

		// �s�����x�X�V
		fWidthLen = WIDTHLEN * ((float)m_nLife[nCntGrass] / (float)m_nMaxLife[nCntGrass]);
		fHeightLen = HEIGHTLEN * ((float)m_nLife[nCntGrass] / (float)m_nMaxLife[nCntGrass]);

		// ���ݒ�
		m_pObjMesh[nCntGrass]->SetWidthLen(fWidthLen);
		m_pObjMesh[nCntGrass]->SetHeightLen(fHeightLen);

		// �ʒu�ݒ�
		m_pObjMesh[nCntGrass]->SetPosition(pos);

		// �����ݒ�
		m_pObjMesh[nCntGrass]->SetRotation(rot);

		m_nLife[nCntGrass]--;
		if (m_nLife[nCntGrass] <= 0)
		{// �������s������

			// �I������
			Release(nCntGrass);
			continue;
		}

		// ���̍X�V����
		m_pObjMesh[nCntGrass]->Update();

		D3DXVECTOR3 *pVtxPos = m_pObjMesh[nCntGrass]->GetVtxPos();
		int nWidth = m_pObjMesh[nCntGrass]->GetWidthBlock();
		int nHeight = m_pObjMesh[nCntGrass]->GetHeightBlock();

		// ���_���̐ݒ�
		for (int nCntHeight = 0; nCntHeight < nHeight + 1; nCntHeight++)
		{//�c�̒��_�����J��Ԃ�

			for (int nCntWidth = 0; nCntWidth < nWidth + 1; nCntWidth++)
			{// ���̒��_�����J��Ԃ�
				
				// ����̒��_
				int nNowVtx = nCntWidth + (nCntHeight * (nWidth + 1));

				float fLen = ((1.0f / (float)nHeight) * (WIDTHLEN * 0.5f)) * nCntHeight;
				int nAngle = -1;

				if (nCntWidth % 2 == 0)
				{
					nAngle *= -1;
				}

				// ���_���W�̐ݒ�
				pVtxPos[nNowVtx] = D3DXVECTOR3
				(
					(fWidthLen * nCntWidth) - ((fWidthLen * nWidth) * 0.5f) + (fLen * nAngle),
					(fHeightLen * nHeight) - ((fHeightLen * nCntHeight)),
					0.0f
				);
			}
		}

	}
}

//==========================================================================
// �`�揈��
//==========================================================================
void CGrassParticle::Draw(void)
{
	for (int nCntGrass = 0; nCntGrass < m_nNumAll; nCntGrass++)
	{
		if (m_pObjMesh[nCntGrass] == NULL)
		{// NULL�̎�
			continue;
		}

		// �`�揈��
		m_pObjMesh[nCntGrass]->Draw();
	}

}
