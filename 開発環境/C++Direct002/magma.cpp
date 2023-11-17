//=============================================================================
// 
//  �N������ [magma.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "magma.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "input.h"
#include "calculation.h"
#include "debugproc.h"
#include "player.h"

//==========================================================================
// �}�N����`
//==========================================================================
#define TEXTURE		"data\\TEXTURE\\image.png"	// �e�N�X�`��
#define MAX_MOVE	(8.0f)		// �ړ���
#define MAX_RANGE	(200.0f)	// �͈�
#define MOVE_LEN	(1.0f)		// ���ړ���
#define MIN_HEIGHT	(-400.0f)	// �Œ�̍���

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CMagma::CMagma(int nPriority) : CObject3DMesh(nPriority)
{
	m_nTexIdx = 0;			// �e�N�X�`���̃C���f�b�N�X�ԍ�
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CMagma::~CMagma()
{

}

//==========================================================================
// ��������
//==========================================================================
CMagma *CMagma::Create(D3DXVECTOR3 pos, float fWidthLen, int nBlock)
{
	// �����p�̃I�u�W�F�N�g
	CMagma *pObjMeshField = NULL;

	if (pObjMeshField == NULL)
	{// NULL��������

		// �������̊m��
		pObjMeshField = DEBUG_NEW CMagma();

		if (pObjMeshField != NULL)
		{// �������̊m�ۂ��o���Ă�����

			pObjMeshField->SetPosition(pos);				// �ʒu
			pObjMeshField->SetWidthBlock(nBlock);		// ������
			pObjMeshField->SetHeightBlock(nBlock);	// �c����
			pObjMeshField->SetWidthLen(fWidthLen);		// �c����
			pObjMeshField->SetHeightLen(fWidthLen);	// ������

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
HRESULT CMagma::Init(void)
{
	HRESULT hr;

	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// ��ސݒ�
	SetType(TYPE_MAGMA);

	// �I�u�W�F�N�g3D���b�V���̏���������
	hr = CObject3DMesh::Init(CObject3DMesh::TYPE_FIELD);

	if (FAILED(hr))
	{// ���s���Ă�����
		return E_FAIL;
	}

	// �e�N�X�`���̊��蓖��
	m_nTexIdx = CManager::GetTexture()->Regist(TEXTURE);

	// �e�N�X�`���̊��蓖��
	BindTexture(m_nTexIdx);

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CMagma::Uninit(void)
{

	// �I������
	CObject3DMesh::Uninit();
}

//==========================================================================
// �X�V����
//==========================================================================
void CMagma::Update(void)
{
	// �L�[�{�[�h���擾
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();


	D3DXVECTOR3 pos = GetPosition();
	D3DXVECTOR3 *pVtxPos = GetVtxPos();
	int nHeightBlock = GetHeightBlock();
	int nWidthBlock = GetWidthBlock();
	float fWidthLen = GetWidthLen();

	static float fWave[1280];

	for (int nCntHeight = 0; nCntHeight < nHeightBlock; nCntHeight++)
	{// �c�̕������J��Ԃ�

		for (int nCntWidth = 0; nCntWidth < nWidthBlock; nCntWidth++)
		{// ���̕������J��Ԃ�

			// ���S�_�ƒ��_�̒���
			float fNowLength =
				sqrtf((pos.x - pVtxPos[nCntWidth + (nCntHeight * (nWidthBlock + 1))].x) * (pos.x - pVtxPos[nCntWidth + (nCntHeight * (nWidthBlock + 1))].x)
					+ (pos.z - pVtxPos[nCntWidth + (nCntHeight * (nWidthBlock + 1))].z) * (pos.z - pVtxPos[nCntWidth + (nCntHeight * (nWidthBlock + 1))].z));

			// ���S�ƒ��_���W�̌���
			float fRotDest = atan2f((pos.x - pVtxPos[nCntWidth + (nCntHeight * (nWidthBlock + 1))].x), (pos.z - pVtxPos[nCntWidth + (nCntHeight * (nWidthBlock + 1))].z));


			// ����
			float Wariai = fNowLength / (fWidthLen * 4.0f);

			// �g�̌���
			float fRot = D3DX_PI * Wariai;

			// �E�F�[�u������
			fWave[nCntWidth + (nCntHeight * (nWidthBlock + 1))] -= D3DX_PI * 0.001f;

			// �p�x�̐��K��
			RotNormalize(fRot);
			RotNormalize(fWave[nCntWidth + (nCntHeight * (nWidthBlock + 1))]);

			// �g
			pVtxPos[nCntWidth + (nCntHeight * (nWidthBlock + 1))].y = 
				sinf(fRot + fWave[nCntWidth + (nCntHeight * (nWidthBlock + 1))]) * 20.0f;
		}
	}




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
			if (pObj->GetType() == CObject::TYPE_PLAYER ||
				pObj->GetType() == CObject::TYPE_ENEMY)
			{// ����̃^�C�v��������

				D3DXVECTOR3 pos = pObj->GetPosition();

				// �����擾
				bool bHit = IsHit(pos);

				if (bHit == true)
				{// �������Ă�����

					// �q�b�g
					pObj->Hit(1);
				}
				
			}

			// ���̃I�u�W�F�N�g����
			pObj = pObjNext;
		}
	}



	// ���_���ݒ�
	SetVtx();
}

//==========================================================================
// �`�揈��
//==========================================================================
void CMagma::Draw(void)
{
	// �`�揈��
	CObject3DMesh::Draw();
}

//==========================================================================
// ���_���ݒ菈��
//==========================================================================
void CMagma::SetVtx(void)
{

	// ���_���X�V
	CObject3DMesh::SetVtx();
}

//==========================================================================
// �n�ʂɓ���������
//==========================================================================
bool CMagma::IsHit(D3DXVECTOR3& pos)
{
	bool bHit = false;

	// �x�N�g���Ɩ@��
	D3DXVECTOR3 vec1, vec2, nor;
	float fHeight = -99999.0f;
	D3DXVECTOR3 *pVtxPos = GetVtxPos();
	D3DXVECTOR3 posfield = GetPosition();

	// �����擾
	float fWidthLen = GetWidthLen();
	float fHeightLen = GetHeightLen();
	//+posfield.x
	// �ő�̒���
	float fMaxWidthLen = fWidthLen * GetWidthBlock();
	float fMaxHeightLen = -fWidthLen * GetHeightBlock();

	// ���肷�钸�_�̌v�Z
	int nCntWidth = 0;
	int nCntHeight = 0;
	int nWidthPoint = ((pos.x - posfield.x) - fMaxWidthLen * 0.5f) / fWidthLen + GetWidthBlock();
	int nHeightPoint = GetHeightBlock() - ((pos.z - posfield.z) - fMaxHeightLen * 0.5f) / fHeightLen;

	for (int nCntH = 0; nCntH < 2; nCntH++)
	{
		for (int nCntW = 0; nCntW < 2; nCntW++)
		{
			// �����_
			nCntWidth = nWidthPoint + (nCntW - 1);
			nCntHeight = nHeightPoint + (nCntH - 1);

			if (nCntWidth < 0 || GetWidthBlock() < nCntWidth)
			{// �͈͊O�Ő擪
				continue;
			}

			if (nCntHeight < 0 || GetHeightBlock() < nCntHeight)
			{// �͈͊O�Ő擪
				continue;
			}

			// ����̒��_
			int nNowPoint = (nCntWidth + 1) + (nCntHeight * (GetWidthBlock() + 1));
			int nVerTexW = (GetWidthBlock() + 1) + 1;

			int nLeft = nCntWidth + (nCntHeight * (GetWidthBlock() + 1));
			int nRight = nCntWidth + (nCntHeight * (GetWidthBlock() + 1)) + nVerTexW;

			if (CollisionTriangle(pVtxPos[nNowPoint] + posfield, pVtxPos[nLeft] + posfield, pVtxPos[nRight] + posfield, pos, pos) == true)
			{// �O�p�ɓ����Ă�����

				// �x�N�g�����v�Z
				vec1 = pVtxPos[nRight] - pVtxPos[nNowPoint];
				vec2 = pVtxPos[nLeft] - pVtxPos[nNowPoint];

				// �O�ς����߂�
				D3DXVec3Cross(&nor, &vec1, &vec2);

				// �O�ς̐��K�������Ė@���ɂ���
				D3DXVec3Normalize(&nor, &nor);

				if (nor.y != 0.0f)
				{// �@����0.0f����Ȃ�������

					// ���������߂�
					fHeight = ((pos.x - pVtxPos[nNowPoint].x - posfield.x) * nor.x + (pos.z - pVtxPos[nNowPoint].z - posfield.z) * nor.z + (pVtxPos[nNowPoint].y * -nor.y)) / -nor.y;
					fHeight += GetPosition().y;
					break;
				}
			}

			if (CollisionTriangle(pVtxPos[nNowPoint + GetWidthBlock()] + posfield, pVtxPos[nRight] + posfield, pVtxPos[nLeft] + posfield, pos, pos) == true)
			{// �O�p�ɓ����Ă�����

				// �x�N�g�����v�Z
				vec1 = pVtxPos[nLeft] - pVtxPos[nNowPoint + GetWidthBlock()];
				vec2 = pVtxPos[nRight] - pVtxPos[nNowPoint + GetWidthBlock()];

				// �O�ς����߂�
				D3DXVec3Cross(&nor, &vec1, &vec2);

				// �O�ς̐��K�������Ė@���ɂ���
				D3DXVec3Normalize(&nor, &nor);

				if (nor.y != 0.0f)
				{// �@����0.0f����Ȃ�������

					// ���������߂�
					fHeight = ((pos.x - pVtxPos[nNowPoint + GetWidthBlock()].x - posfield.x) * nor.x + (pos.z - pVtxPos[nNowPoint + GetWidthBlock()].z - posfield.z) * nor.z + (pVtxPos[nNowPoint + GetWidthBlock()].y * -nor.y)) / -nor.y;
					fHeight += GetPosition().y;
					break;
				}
			}
		}
	}

	if (fHeight > pos.y)
	{// �O��̈ʒu��������������(�������Ă�����)
		bHit = true;
	}

	// �����Ԃ�
	return bHit;
}

//==========================================================================
// �N���̏��擾
//==========================================================================
CMagma *CMagma::GetMagma(void)
{
	return this;
}
