//=============================================================================
// 
//  �N������ [elevation_outside.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "elevation_outside.h"
#include "manager.h"
#include "renderer.h"
#include "game.h"
#include "texture.h"
#include "input.h"
#include "calculation.h"
#include "debugproc.h"
#include "particle.h"
#include "3D_effect.h"
#include "player.h"
#include "model.h"
#include "ballast.h"
#include "power_gauge.h"
#include "boss.h"
#include "enemy.h"
#include "targetpoint.h"
#include "impactwave.h"
#include "camera.h"
#include "enemymanager.h"
#include "enemybase.h"
#include "ranking.h"
#include "sound.h"

//==========================================================================
// �}�N����`
//==========================================================================
#define TEXTURE		"data\\TEXTURE\\glass.jpg"	// �e�N�X�`��
#define MAX_MOVE	(8.0f)		// �ړ���
#define MAX_RANGE	(200.0f)	// �͈�
#define MOVE_LEN	(1.0f)		// ���ړ���
#define MIN_HEIGHT	(-400.0f)	// �Œ�̍���
#define MOVE		(5.0f)
#define IMPACTSHAKE_DIRECT	(30.0f)
#define IMPACTSHAKE_IMPACT	(20.0f)
#define SHAKETIME_DIRECT	(15 + 5)
#define SHAKETIME_IMPACT	(10 + 5)
#define SPAWN_WAITTIME		(20)
#if 1
#define CHANGE	(0)
#endif

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CElevationOutside::CElevationOutside(int nPriority) : CElevation(nPriority)
{
	// �l�̃N���A
	m_nTexIdx = 0;	// �e�N�X�`���̃C���f�b�N�X�ԍ�
	m_postype = POSTYPE_UP;	// �ʒu�̎��
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CElevationOutside::~CElevationOutside()
{

}

//==========================================================================
// ����������
//==========================================================================
HRESULT CElevationOutside::Init(void)
{
	return E_FAIL;
}


//==========================================================================
// ����������
//==========================================================================
HRESULT CElevationOutside::Init(const char *pText)
{
	HRESULT hr;

	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// �e�N�X�`���̊��蓖��
	m_nTexIdx = CManager::GetTexture()->Regist(TEXTURE);

	// �e�N�X�`���̊��蓖��
	BindTexture(m_nTexIdx);

	// ��ސݒ�
	SetType(TYPE_ELEVATION_OUTSIDE);

	// ���[�h����
	hr = Load(pText);

	if (FAILED(hr))
	{// ���s���Ă�����
		return E_FAIL;
	}

	// �e��ϐ�������
	SetPosition(m_aInfo.pos);				// �ʒu
	SetWidthBlock(m_aInfo.nWidthBlock);		// ������
	SetHeightBlock(m_aInfo.nHeightBlock);	// �c����
	SetWidthLen(m_aInfo.fWidthLength);		// �c����
	SetHeightLen(m_aInfo.fHeightLength);	// ������
	m_fBrushStrength = MAX_MOVE;			// �u���V����
	m_fBrushRange = MAX_RANGE;				// �͈�

	// �ʒu�̎��
	if (strcmp(pText, "data\\TEXT\\elevation_outside_UP.txt") == 0)
	{
		m_postype = POSTYPE_UP;
	}
	else if (strcmp(pText, "data\\TEXT\\elevation_outside_Right.txt") == 0)
	{
		m_postype = POSTYPE_RIGHT;
	}
	else if (strcmp(pText, "data\\TEXT\\elevation_outside_Left.txt") == 0)
	{
		m_postype = POSTYPE_LEFT;
	}
	else if (strcmp(pText, "data\\TEXT\\elevation_outside_Down.txt") == 0)
	{
		m_postype = POSTYPE_DOWN;
	}

	// �I�u�W�F�N�g3D���b�V���̏���������
	hr = CObject3DMesh::Init(CObject3DMesh::TYPE_FIELD);

	if (FAILED(hr))
	{// ���s���Ă�����
		return E_FAIL;
	}

	int nStage = 0;
	switch (CManager::GetMode())
	{
	case CScene::MODE_RESULT:
		nStage = 3;
		break;

	case CScene::MODE_RANKING:
		nStage = CRanking::GetRandStage();
		break;

	default:
		nStage = 0;
		break;
	}

	for (int nCntHeight = 0; nCntHeight < m_aInfo.nHeightBlock + 1; nCntHeight++)
	{// �c�̕������J��Ԃ�

		for (int nCntWidth = 0; nCntWidth < m_aInfo.nWidthBlock + 1; nCntWidth++)
		{// ���̕������J��Ԃ�
			m_VtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y = m_aInfo.pStageVtxPos[nStage][nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y;
		}
	}
	

	// ���_�����������m��
	int nVtxNum = GetNumVertex();
	m_sFadeInfo.pRot = DEBUG_NEW float[nVtxNum];
	memset(&m_sFadeInfo.pRot[0], 0, sizeof(float) * nVtxNum);

	// ���_���ݒ�
	//SetVtx();

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CElevationOutside::Uninit(void)
{

	// �I������
	CElevation::Uninit();
}

//==========================================================================
// �������
//==========================================================================
void CElevationOutside::Release(void)
{
	// �I������
	CElevation::Release();
}

//==========================================================================
// �X�V����
//==========================================================================
void CElevationOutside::Update(void)
{
	// �L�[�{�[�h���擾
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();

	if (m_postype == POSTYPE_LEFT)
	{
		if (pInputKeyboard->GetTrigger(DIK_F3) == true)
		{// �؂�ւ�
			m_bEdit = m_bEdit ? false : true;

			// �ڕW�̒n�_����
			if (m_pTargetP != NULL)
			{
				m_pTargetP->Uninit();
				m_pTargetP = NULL;
			}
			// �ڕW�̒n�_����
			else if (m_pTargetP == NULL)
			{
				m_pTargetP = CTargetPoint::Create(D3DXVECTOR3(0.0f, 0.0f, 0.0f), 400.0f, 800.0f);
			}
		}

		// ���_����
		if (m_bEdit == true)
		{// �G�f�B�b�g���Ȃ�
			UPVtxField(m_pTargetP->GetPosition());
		}
	}

	if (CManager::GetScene()->GetWaveManager()->IsStageFrag() == false)
	{// �X�e�[�W�؂�ւ�������
		// ���_���ݒ�
		SetVtx();
	}
}

//==========================================================================
// �`�揈��
//==========================================================================
void CElevationOutside::Draw(void)
{
	//  �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	if (CManager::IsWireframe() == true)
	{
		pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);	// ���C���[�t���[�����[�h
	}

	// �`�揈��
	CObject3DMesh::Draw();

	pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);	// ���߂郂�[�h
}

//==========================================================================
// �����擾
//==========================================================================
float CElevationOutside::GetHeight(D3DXVECTOR3& pos, D3DXVECTOR3& move, bool &bLand, CObject::TYPE type)
{
	// �x�N�g���Ɩ@��
	D3DXVECTOR3 vec1, vec2, nor;
	float fHeight = 0.0f;
	D3DXVECTOR3 *pVtxPos = GetVtxPos();

#if 0
	//for (int nCntHeight = 0; nCntHeight < m_aInfo.nHeightBlock; nCntHeight++)
	//{// �c�̕������J��Ԃ�

	//	for (int nCntWidth = 0; nCntWidth < m_aInfo.nWidthBlock; nCntWidth++)
	//	{// ���̕������J��Ԃ�

	//		if (bLand == true)
	//		{
	//			break;
	//		}

	//		// ����̒��_
	//		int nNowPoint = (nCntWidth + 1) + (nCntHeight * (m_aInfo.nWidthBlock + 1));
	//		int nVerTexW = (m_aInfo.nWidthBlock + 1) + 1;

	//		int nLeft	= nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1));
	//		int nRight	= nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1)) + nVerTexW;

	//		if (CollisionTriangle(pVtxPos[nNowPoint], pVtxPos[nLeft], pVtxPos[nRight], pos, pos) == true)
	//		{// �O�p�ɓ����Ă�����

	//			// �x�N�g�����v�Z
	//			vec1 = pVtxPos[nRight] - pVtxPos[nNowPoint];
	//			vec2 = pVtxPos[nLeft] - pVtxPos[nNowPoint];

	//			// �O�ς����߂�
	//			D3DXVec3Cross(&nor, &vec1, &vec2);

	//			// �O�ς̐��K�������Ė@���ɂ���
	//			D3DXVec3Normalize(&nor, &nor);

	//			if (nor.y != 0.0f)
	//			{// �@����0.0f����Ȃ�������

	//				// ���������߂�
	//				fHeight = ((pos.x - pVtxPos[nNowPoint].x) * nor.x + (pos.z - pVtxPos[nNowPoint].z) * nor.z + (pVtxPos[nNowPoint].y * -nor.y)) / -nor.y;
	//				fHeight += GetPosition().y;
	//				bLand = true;
	//				break;
	//			}
	//		}

	//		if (CollisionTriangle(pVtxPos[nNowPoint + m_aInfo.nWidthBlock], pVtxPos[nRight], pVtxPos[nLeft], pos, pos) == true)
	//		{// �O�p�ɓ����Ă�����

	//			// �x�N�g�����v�Z
	//			vec1 = pVtxPos[nLeft] - pVtxPos[nNowPoint + m_aInfo.nWidthBlock];
	//			vec2 = pVtxPos[nRight] - pVtxPos[nNowPoint + m_aInfo.nWidthBlock];

	//			// �O�ς����߂�
	//			D3DXVec3Cross(&nor, &vec1, &vec2);

	//			// �O�ς̐��K�������Ė@���ɂ���
	//			D3DXVec3Normalize(&nor, &nor);

	//			if (nor.y != 0.0f)
	//			{// �@����0.0f����Ȃ�������

	//				// ���������߂�
	//				fHeight = ((pos.x - pVtxPos[nNowPoint + m_aInfo.nWidthBlock].x) * nor.x + (pos.z - pVtxPos[nNowPoint + m_aInfo.nWidthBlock].z) * nor.z + (pVtxPos[nNowPoint + m_aInfo.nWidthBlock].y * -nor.y)) / -nor.y;
	//				fHeight += GetPosition().y;
	//				bLand = true;
	//				break;
	//			}
	//		}
	//	}
	//}
#endif
	// �t�B�[���h�̈ʒu
	D3DXVECTOR3 posfield = GetPosition();

	// �����擾
	float fWidthLen = GetWidthLen();
	float fHeightLen = GetHeightLen();

	// �ő�̒���
	float fMaxWidthLen = fWidthLen * m_aInfo.nWidthBlock + posfield.x;
	float fMaxHeightLen = -fWidthLen * m_aInfo.nHeightBlock + posfield.z;

	// ���_���ߑł�
	int nCntWidth = 0;
	int nCntHeight = 0;
	int nWidthPoint = (int)((pos.x - fMaxWidthLen * 0.5f) / fWidthLen) + m_aInfo.nWidthBlock;
	int nHeightPoint = m_aInfo.nHeightBlock - (int)((pos.z - fMaxHeightLen * 0.5f) / fHeightLen);

	for (int nCntH = 0; nCntH < 2; nCntH++)
	{
		for (int nCntW = 0; nCntW < 2; nCntW++)
		{
			// �����_
			nCntWidth = nWidthPoint + (nCntW - 1);
			nCntHeight = nHeightPoint + (nCntH - 1);

			if (nCntWidth < 0 || m_aInfo.nWidthBlock < nCntWidth)
			{// �͈͊O�Ő擪
				continue;
			}

			if (nCntHeight < 0 || m_aInfo.nHeightBlock < nCntHeight)
			{// �͈͊O�Ő擪
				continue;
			}

			// ����̒��_
			int nNowPoint = (nCntWidth + 1) + (nCntHeight * (m_aInfo.nWidthBlock + 1));
			int nVerTexW = (m_aInfo.nWidthBlock + 1) + 1;

			int nLeft = nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1));
			int nRight = nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1)) + nVerTexW;

			if (nLeft >= GetNumVertex() || nRight >= GetNumVertex())
			{
				continue;
			}

			if (CollisionTriangle(pVtxPos[nNowPoint], pVtxPos[nLeft], pVtxPos[nRight], pos, pos) == true)
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
					fHeight = ((pos.x - pVtxPos[nNowPoint].x) * nor.x + (pos.z - pVtxPos[nNowPoint].z) * nor.z + (pVtxPos[nNowPoint].y * -nor.y)) / -nor.y;
					fHeight += GetPosition().y;
					bLand = true;


					if (nor.y < 0.0f)
					{
						int n = 0;
					}
					/*if (nor.y <= 0.5f && nor.y >= -0.5f && fHeight > pos.y && (type == CObject::TYPE_PLAYER || type == CObject::TYPE_ENEMY))
					{
						pos.x -= move.x;
						pos.z -= move.z;
						move.x = 0.0f;
						move.z = 0.0f;
						int n = 0;
					}*/
				}
				break;
			}

			if (CollisionTriangle(pVtxPos[nNowPoint + m_aInfo.nWidthBlock], pVtxPos[nRight], pVtxPos[nLeft], pos, pos) == true)
			{// �O�p�ɓ����Ă�����

				// �x�N�g�����v�Z
				vec1 = pVtxPos[nLeft] - pVtxPos[nNowPoint + m_aInfo.nWidthBlock];
				vec2 = pVtxPos[nRight] - pVtxPos[nNowPoint + m_aInfo.nWidthBlock];

				// �O�ς����߂�
				D3DXVec3Cross(&nor, &vec1, &vec2);

				// �O�ς̐��K�������Ė@���ɂ���
				D3DXVec3Normalize(&nor, &nor);

				if (nor.y != 0.0f)
				{// �@����0.0f����Ȃ�������

					// ���������߂�
					fHeight = ((pos.x - pVtxPos[nNowPoint + m_aInfo.nWidthBlock].x) * nor.x + (pos.z - pVtxPos[nNowPoint + m_aInfo.nWidthBlock].z) * nor.z + (pVtxPos[nNowPoint + m_aInfo.nWidthBlock].y * -nor.y)) / -nor.y;
					fHeight += GetPosition().y;
					bLand = true;

					if (nor.y < 0.0f)
					{
						int n = 0;
					}


					/*if (nor.y <= 0.5f && nor.y >= -0.5f && fHeight >= pos.y && (type == CObject::TYPE_PLAYER || type == CObject::TYPE_ENEMY))
					{
						pos.x -= move.x;
						pos.z -= move.z;
						move.x = 0.0f;
						move.z = 0.0f;
						int n = 0;
					}*/
					break;
				}
			}

		}
	}

	if (bLand == true)
	{// ���n���Ă�����

		return fHeight;
	}

	// �������擾
	return pos.y;
}

//==========================================================================
// ���_�グ����
//==========================================================================
void CElevationOutside::UPVtxField(D3DXVECTOR3 pos)
{

	float m_fWidthLen = GetWidthLen();
	float m_fHeightLen = GetHeightLen();
	D3DXVECTOR3 *pVtxPos = GetVtxPos();

	// �f�o�b�O�\��
	CManager::GetDebugProc()->Print(
		"------------------[ �N���G�f�B�b�g��� ]------------------\n"
		"�u���V�̃T�C�Y�F[1, 2] �y%f�z\n"
		"�u���V�̋����F  [3, 4] �y%f�z\n"
		"���_�グ�����F  [5, 6]\n"
		"���g�k�F        [7, 8] �y%f, %f�z\n"
		"�܂����F        [ �X ]\n"
		"���_���Z�b�g�F  [delete]\n"
		"�N�����ۑ��F  [F9]\n\n", m_fBrushRange, m_fBrushStrength, m_fWidthLen, m_fHeightLen);

	// �L�[�{�[�h���擾
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();

	if (pInputKeyboard->GetTrigger(DIK_F9) == true)
	{// �Z�[�u
		Save();
	}

	// �u���V�̃T�C�Y
	if (pInputKeyboard->GetPress(DIK_1))
	{// 1�ŉ�����
		m_fBrushRange -= 1.0f;
	}

	if (pInputKeyboard->GetPress(DIK_2))
	{// 2�ŏグ��
		m_fBrushRange += 1.0f;
	}

	// �u���V�̋���
	if (pInputKeyboard->GetPress(DIK_3))
	{// 3�ŉ�����
		m_fBrushStrength -= 0.01f;
	}

	if (pInputKeyboard->GetPress(DIK_4))
	{// 4�ŏグ��
		m_fBrushStrength += 0.01f;
	}

	// �t�B�[���h�̕�
	if (pInputKeyboard->GetPress(DIK_7))
	{// 7�ŏグ��
		m_fWidthLen -= MOVE_LEN;
		m_fHeightLen -= MOVE_LEN;
	}

	if (pInputKeyboard->GetPress(DIK_8))
	{// 8�ŉ�����
		m_fWidthLen += MOVE_LEN;
		m_fHeightLen += MOVE_LEN;
	}

	// �l�̐��K��
	ValueNormalize(m_fWidthLen, 99999999.0f, 10.0f);
	ValueNormalize(m_fHeightLen, 99999999.0f, 10.0f);

	// �l�̐��K��
	ValueNormalize(m_fBrushStrength, 99999999.0f, 0.1f);
	ValueNormalize(m_fBrushRange, 99999999.0f, 1.0f);

	// �t�B�[���h�̈ʒu
	D3DXVECTOR3 posfield = GetPosition();

	// ���_���̐ݒ�
	for (int nCntHeight = 0; nCntHeight < m_aInfo.nHeightBlock + 1; nCntHeight++)
	{//�c�̒��_�����J��Ԃ�

		for (int nCntWidth = 0; nCntWidth < m_aInfo.nWidthBlock + 1; nCntWidth++)
		{// ���̒��_�����J��Ԃ�

			if (pInputKeyboard->GetTrigger(DIK_DELETE))
			{// delete�Ń��Z�b�g
				m_aInfo.pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y = 0.0f;
				m_VtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y = 0.0f;
			}

			float fNowLength =
				sqrtf((pos.x - (pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].x + posfield.x)) * (pos.x - (pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].x + posfield.x))
					+ (pos.z - (pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].z + posfield.z)) * (pos.z - (pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].z + posfield.z)));

			float Wariai = fNowLength / m_fBrushRange;

			if (Wariai > 1.0f)
			{// ������1.0f�ȉ�����
				continue;
			}

			D3DXVECTOR3 move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

			if (pInputKeyboard->GetPress(DIK_5))
			{// 5�ŉ�����
				move.y = -m_fBrushStrength - (-m_fBrushStrength * Wariai);
			}

			if (pInputKeyboard->GetPress(DIK_6))
			{// 6�ŏグ��
				move.y = m_fBrushStrength - (m_fBrushStrength * Wariai);
			}

			if (pInputKeyboard->GetTrigger(DIK_9))
			{// 9�ł܂���
				m_aInfo.pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y = 0.0f;
			}

			if ((m_aInfo.nWidthBlock + 1) * (m_aInfo.nHeightBlock + 1) - 1 == nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1)))
			{
				int n = 0;
			}

			m_aInfo.pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y += move.y;
			// �e���_���W�m�F
			//pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y += move.y;
		}
	}

	// ���_�ݒ�
	SetVtxPos(pVtxPos);
	SetWidthLen(m_fWidthLen);
	SetHeightLen(m_fHeightLen);

	if (m_pTargetP != NULL)
	{
		// �����ύX
		m_pTargetP->SetWidthLen(m_fBrushRange);

		// �J�����̏��擾
		CCamera *pCamera = CManager::GetCamera();

		// �J�����̌����擾
		D3DXVECTOR3 Camerarot = pCamera->GetRotation();

		if (pInputKeyboard->GetPress(DIK_LEFT) == true)
		{// ���L�[�������ꂽ,���ړ�

			if (pInputKeyboard->GetPress(DIK_UP) == true)
			{// A+W,����ړ�

				pos.x += sinf(-D3DX_PI * MOVE_LRDW + Camerarot.y) * MOVE;
				pos.z += cosf(-D3DX_PI * MOVE_LRDW + Camerarot.y) * MOVE;
			}
			else if (pInputKeyboard->GetPress(DIK_DOWN) == true)
			{// A+S,�����ړ�

				pos.x += sinf(-D3DX_PI * MOVE_LRUP + Camerarot.y) * MOVE;
				pos.z += cosf(-D3DX_PI * MOVE_LRUP + Camerarot.y) * MOVE;
			}
			else
			{// A,���ړ�

				pos.x += sinf(-D3DX_PI * MOVE_LR + Camerarot.y) * MOVE;
				pos.z += cosf(-D3DX_PI * MOVE_LR + Camerarot.y) * MOVE;
			}
		}
		else if (pInputKeyboard->GetPress(DIK_RIGHT) == true)
		{// D�L�[�������ꂽ,�E�ړ�

			if (pInputKeyboard->GetPress(DIK_UP) == true)
			{// D+W,�E��ړ�

				pos.x += sinf(D3DX_PI * MOVE_LRDW + Camerarot.y) * MOVE;
				pos.z += cosf(D3DX_PI * MOVE_LRDW + Camerarot.y) * MOVE;
			}
			else if (pInputKeyboard->GetPress(DIK_DOWN) == true)
			{// D+S,�E���ړ�

				pos.x += sinf(D3DX_PI * MOVE_LRUP + Camerarot.y) * MOVE;
				pos.z += cosf(D3DX_PI * MOVE_LRUP + Camerarot.y) * MOVE;
			}
			else
			{// D,�E�ړ�

				pos.x += sinf(D3DX_PI * MOVE_LR + Camerarot.y) * MOVE;
				pos.z += cosf(D3DX_PI * MOVE_LR + Camerarot.y) * MOVE;
			}
		}
		else if (pInputKeyboard->GetPress(DIK_UP) == true)
		{// W�������ꂽ�A���ړ�

			pos.x += sinf(Camerarot.y) * MOVE;
			pos.z += cosf(Camerarot.y) * MOVE;
		}
		else if (pInputKeyboard->GetPress(DIK_DOWN) == true)
		{// S�������ꂽ�A��O�ړ�

			pos.x += sinf(D3DX_PI + Camerarot.y) * MOVE;
			pos.z += cosf(D3DX_PI + Camerarot.y) * MOVE;
		}

		// �ʒu�ݒ�
		m_pTargetP->SetPosition(pos);
	}

}


//==========================================================================
// ���_���ݒ菈��
//==========================================================================
void CElevationOutside::SetVtx(void)
{
	// ���_���X�V
	CElevation::SetVtx();
}

//==========================================================================
// ���[�h����
//==========================================================================
HRESULT CElevationOutside::Load(const char *pText)
{
	char aComment[mylib_const::MAX_STRING] = {};	//�R�����g�p

	FILE *pFile = NULL;	// �t�@�C���|�C���^��錾

	// �t�@�C�����J��
	pFile = fopen(pText, "r");

	if (pFile == NULL)
	{// �t�@�C�����J���Ȃ������ꍇ
		fclose(pFile);
		return E_FAIL;
	}

	int nCntStage = 0;	// �X�e�[�W�̃J�E���g

	while (1)
	{// END_SCRIPT������܂ŌJ��Ԃ�

		// ������̓ǂݍ���
		fscanf(pFile, "%s", &aComment[0]);

		if (strcmp(&aComment[0], "TEXTURE_FILENAME") == 0)
		{// TEXTYPE���������ޓǂݍ���

			fscanf(pFile, "%s", &aComment[0]);	// =�̕�
			fscanf(pFile, "%s", &aComment[0]);	// �e�N�X�`���t�@�C����

			// �t�@�C�����ۑ�
			m_aInfo.TextureFileName = aComment;
		}

		// ���b�V���t�B�[���h�̐ݒ�
		if (strcmp(&aComment[0], "FIELDSET") == 0)
		{// ���f���̓ǂݍ��݂��J�n

			while (strcmp(&aComment[0], "END_FIELDSET"))
			{// END_FIELDSET������܂ŌJ��Ԃ�

				fscanf(pFile, "%s", &aComment[0]);	// �m�F����

				if (strcmp(&aComment[0], "POS") == 0)
				{// POS��������ʒu�ǂݍ���

					fscanf(pFile, "%s", &aComment[0]);	// =�̕�
					fscanf(pFile, "%f", &m_aInfo.pos.x);		// X���W
					fscanf(pFile, "%f", &m_aInfo.pos.y);		// Y���W
					fscanf(pFile, "%f", &m_aInfo.pos.z);		// Z���W
				}

				if (strcmp(&aComment[0], "BLOCK") == 0)
				{// BLOCK������������ǂݍ���

					fscanf(pFile, "%s", &aComment[0]);			// =�̕�
					fscanf(pFile, "%d", &m_aInfo.nWidthBlock);	// ���̕�����
					fscanf(pFile, "%d", &m_aInfo.nHeightBlock);	// �c�̕�����
				}

				if (strcmp(&aComment[0], "SIZE") == 0)
				{// SIZE������������ǂݍ���

					fscanf(pFile, "%s", &aComment[0]);		// =�̕�
					fscanf(pFile, "%f", &m_aInfo.fWidthLength);	// ���̒���
					fscanf(pFile, "%f", &m_aInfo.fHeightLength);	// �c�̒���
				}

				if (strcmp(&aComment[0], "VTXPOS") == 0)
				{// VTXPOS�������璸�_�ǂݍ���

					// ���_��
					int nVtxNum = (m_aInfo.nHeightBlock + 1) * (m_aInfo.nWidthBlock + 1);
					int nCntVtx = 0;

					// ���_���Ń������m��
					if (nCntStage == 0)
					{
						m_aInfo.pVtxPos = DEBUG_NEW D3DXVECTOR3[nVtxNum];
					}
					m_aInfo.pStageVtxPos[nCntStage] = DEBUG_NEW D3DXVECTOR3[nVtxNum];

					// ���_�ǂݍ��ݗp
					std::string VtxPos;

					while (1)
					{// END_VTXPOS������܂ŌJ��Ԃ�

						fscanf(pFile, "%s", (char*)VtxPos.c_str());

						if (strcmp(VtxPos.c_str(), "END_VTXPOS") == 0)
						{// �r���I������Ă���

							for (int nCnt = nCntVtx; nCnt < nVtxNum; nCnt++)
							{
								// 0�N���A
								m_aInfo.pVtxPos[nCnt].y = 0.0f;	// ����
								m_aInfo.pStageVtxPos[nCntStage][nCnt].y = 0.0f;	// ���
							}
							break;
						}

						// float�ϊ�
						if (nCntStage == 0)
						{
							m_aInfo.pVtxPos[nCntVtx].y = 0.0f;	// ����
						}
						m_aInfo.pStageVtxPos[nCntStage][nCntVtx].y = std::strtof(VtxPos.c_str(), nullptr);	// ���
						nCntVtx++;
					}
					// �X�e�[�W�����Z
					nCntStage++;
				}

			}// END_FIELDSET�̂�����
		}

		if (strcmp(&aComment[0], "END_SCRIPT") == 0)
		{// �I�������Ń��[�v�𔲂���

			break;
		}
	}

	// �t�@�C�������
	fclose(pFile);
	return S_OK;

}

//==========================================================================
// �Z�[�u����
//==========================================================================
void CElevationOutside::Save(void)
{
	FILE *pFile = NULL;	// �t�@�C���|�C���^��錾

	// �t�@�C�����J��
	switch (m_postype)
	{
	case CElevation::POSTYPE_UP:
		pFile = fopen("data\\TEXT\\elevation_outside_save_UP.txt", "w");
		break;
	case CElevation::POSTYPE_DOWN:
		pFile = fopen("data\\TEXT\\elevation_outside_save_DOWN.txt", "w");
		break;
	case CElevation::POSTYPE_RIGHT:
		pFile = fopen("data\\TEXT\\elevation_outside_save_RIGHT.txt", "w");
		break;
	case CElevation::POSTYPE_LEFT:
		pFile = fopen("data\\TEXT\\elevation_outside_save_LEFT.txt", "w");
		break;
	case CElevation::POSTYPE_MAX:
		break;
	default:
		break;
	}

	if (pFile == NULL)
	{// �t�@�C�����J���Ȃ������ꍇ
		return;
	}

	// �t�@�C���ɏ����o��
	fprintf(pFile,
		"#------------------------------------------------------------------------------\n"
		"# �e�N�X�`���t�@�C����\n"
		"#------------------------------------------------------------------------------\n");

	// �e�N�X�`���t�@�C����
	fprintf(pFile, "TEXTURE_FILENAME = %s\t\t\n", m_aInfo.TextureFileName.c_str());
	
	// �t�B�[���h�̐ݒu
	fprintf(pFile,
		"\n"
		"#==============================================================================\n"
		"# �t�B�[���h�̐ݒu\n"
		"#==============================================================================\n");

	float m_fWidthLen = GetWidthLen();
	float m_fHeightLen = GetHeightLen();

	fprintf(pFile,
		"FIELDSET\n"
		"\tPOS = %.2f %.2f %.2f\n"
		"\tBLOCK = %d %d\n"
		"\tSIZE = %.2f %.2f\n"
		"\tVTXPOS\n\t", m_aInfo.pos.x, m_aInfo.pos.y, m_aInfo.pos.z,
		m_aInfo.nWidthBlock, m_aInfo.nHeightBlock,
		m_fWidthLen, m_fHeightLen);

	// ���_�擾
	D3DXVECTOR3 *pVtxPos = GetVtxPos();

	// ���_���̐ݒ�
	for (int nCntHeight = 0; nCntHeight < m_aInfo.nHeightBlock + 1; nCntHeight++)
	{//�c�̒��_�����J��Ԃ�

		fprintf(pFile, "\t");
		for (int nCntWidth = 0; nCntWidth < m_aInfo.nWidthBlock + 1; nCntWidth++)
		{// ���̒��_�����J��Ԃ�

			fprintf(pFile, "%.2f ", pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y);
		}
		fprintf(pFile, "\n\t");
	}
	fprintf(pFile, "\n\tEND_VTXPOS\n");

	fprintf(pFile, "\nEND_FIELDSET\n");
	fprintf(pFile, "\nEND_SCRIPT		# ���̍s�͐�Ώ����Ȃ����ƁI");

	//�t�@�C�������
	fclose(pFile);
}

//==========================================================================
// �N���̏��擾
//==========================================================================
CElevationOutside *CElevationOutside::GetElevation(void)
{
	return this;
}
