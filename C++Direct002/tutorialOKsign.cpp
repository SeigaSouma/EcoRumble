//=============================================================================
// 
//  �`���[�g���A���E�B���h�E���� [tutorialOKsign.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "tutorialOKsign.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "calculation.h"
#include "input.h"
#include "fade.h"
#include "tutorial.h"
#include "tutorialwindow.h"

//==========================================================================
// �}�N����`
//==========================================================================
#define MOVETIME	(100)	// �ړ�����
#define TEXTURE		"data\\TEXTURE\\tutorialwindow\\tutorial_OK.png"

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================
int CTutorialOKSign::m_nTexIdx = 0;	// �e�N�X�`���̃C���f�b�N�X�ԍ�

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CTutorialOKSign::CTutorialOKSign(int nPriority) : CObject2D(nPriority)
{
	// �l�̃N���A
	m_nCntMove = 0;		// �ړ�����

}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CTutorialOKSign::~CTutorialOKSign()
{

}

//==========================================================================
// ��������
//==========================================================================
CTutorialOKSign *CTutorialOKSign::Create(void)
{
	// �����p�̃I�u�W�F�N�g
	CTutorialOKSign *pWindow = NULL;

	if (pWindow == NULL)
	{// NULL��������

		// �������̊m��
		pWindow = DEBUG_NEW CTutorialOKSign;

		if (pWindow != NULL)
		{// �������̊m�ۂ��o���Ă�����

			// ����������
			pWindow->Init();
		}

		return pWindow;
	}

	return NULL;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CTutorialOKSign::Init(void)
{

	// �I�u�W�F�N�g2D�̏���������
	CObject2D::Init();

	// ��ނ̐ݒ�
	SetType(TYPE_OKSIGN);

	// �e��ϐ��̏�����
	m_nCntMove = MOVETIME;		// �ړ�����

	// �e�N�X�`���̊��蓖��
	if (m_nTexIdx == 0)
	{// �܂��ǂݍ���łȂ�������
		m_nTexIdx = CManager::GetTexture()->Regist(TEXTURE);
	}

	// �e�N�X�`���̊��蓖��
	BindTexture(m_nTexIdx);

	// �ݒ�
	SetSize(CManager::GetTexture()->GetImageSize(m_nTexIdx) * 0.5f);		// �T�C�Y
	SetPosition(D3DXVECTOR3(640.0f, SCREEN_HEIGHT + GetSize().y, 0.0f));	// �ʒu

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CTutorialOKSign::Uninit(void)
{
	// �I�u�W�F�N�g2D�̏I������
	CObject2D::Uninit();
}

//==========================================================================
// �X�V����
//==========================================================================
void CTutorialOKSign::Update(void)
{
	// ���_���X�V
	SetVtx();

	// �ړ����Ԍ��Z
	m_nCntMove--;

	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();

	// �ړ�
	pos.y += (360.0f - pos.y) * 0.15f;

	// �ʒu�ݒ�
	SetPosition(pos);

	if (m_nCntMove > 0)
	{// �ړ����Ԃ��I����ĂȂ�
		return;
	}

	// ���Z�b�g
	m_nCntMove = 0;

	// �E�B���h�E����
	CTutorialWindow::Create(CTutorial::GetStep()->GetNowStep());

	// �I������
	Uninit();
}
