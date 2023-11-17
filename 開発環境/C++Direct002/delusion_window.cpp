//=============================================================================
// 
//  �ϑz�̃E�B���h�E���� [delusion_window.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "delusion_window.h"
#include "input.h"
#include "manager.h"
#include "texture.h"
#include "renderer.h"
#include "calculation.h"
#include "debugproc.h"

//==========================================================================
// �}�N����`
//==========================================================================
#define WIDTH	(400.0f)		// ����
#define HEIGHT	(200.0f)		// �c��
#define TEXTURE	"data\\TEXTURE\\bullet_01.png"

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================
int CDelusionWindow::m_nNumAll = 0;		// �ϑz�̃E�B���h�E�̑���

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CDelusionWindow::CDelusionWindow(int nPriority) : CObject2D(nPriority)
{
	// �������Z
	m_nNumAll++;
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CDelusionWindow::~CDelusionWindow()
{
	
}

//==========================================================================
// ��������
//==========================================================================
CDelusionWindow *CDelusionWindow::Create(void)
{
	// �����p�̃I�u�W�F�N�g
	CDelusionWindow *pItem = NULL;

	if (pItem == NULL)
	{// NULL��������

		// �������̊m��
		pItem = DEBUG_NEW CDelusionWindow;

		if (pItem != NULL)
		{// �������̊m�ۂ��o���Ă�����

			// ����������
			pItem->Init();
		}

		return pItem;
	}

	return NULL;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CDelusionWindow::Init(void)
{
	HRESULT hr;

	// �e��ϐ��̏�����
 	SetSize(D3DXVECTOR2(0.0f, 0.0f));				// �T�C�Y
	SetPosition(D3DXVECTOR3(640.0f, 250.0f, 0.0f));	// �ʒu
	SetColor(D3DXCOLOR(0.6f, 0.6f, 1.0f, 0.8f));

	// ��ނ̐ݒ�
	SetType(CObject::TYPE_DELUSION_WINDOW);

	// ����������
	hr = CObject2D::Init();

	if (FAILED(hr))
	{// ���s�����Ƃ�

		return E_FAIL;
	}

	// �e�N�X�`���̓ǂݍ���
	m_nTexIdx = CManager::GetTexture()->Regist(NULL);

	// �e�N�X�`���̊��蓖��
	BindTexture(m_nTexIdx);

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CDelusionWindow::Uninit(void)
{

	// �I������
	CObject2D::Uninit();

	// �������Z
	m_nNumAll--;
}

//==========================================================================
// �X�V����
//==========================================================================
void CDelusionWindow::Update(void)
{

	if (IsDeath() == true)
	{// ���S�t���O�������Ă�����
		return;
	}

	// �T�C�Y�擾
	D3DXVECTOR2 size = GetSize();

	// �����␳
	InertiaCorrection(size.x, WIDTH, 0.25f);
	InertiaCorrection(size.y, HEIGHT, 0.25f);

	// �T�C�Y�ݒ�
	SetSize(size);

	// ���_���ݒ�
	SetVtx();

	// �f�o�b�O�\��
	CManager::GetDebugProc()->Print(
		"\n"
		"------------------[����̑���]------------------\n"
		" �v�f�؂�ւ��F�y��, ���z\n"
		" �㏸ or �����F�y��, ���z\n"
		" ��F[�p���[�n], �E�F[�Q�̌n], ���F[�X�s�[�h�n]\n"
		"\n");
}

//==========================================================================
// �`�揈��
//==========================================================================
void CDelusionWindow::Draw(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// �A���t�@�e�X�g��L���ɂ���
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	CObject2D::Draw();

	// �A���t�@�e�X�g��L���ɂ���
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
}

//==========================================================================
// ���_���ݒ菈��
//==========================================================================
void CDelusionWindow::SetVtx(void)
{
	// ���_�ݒ�
	CObject2D::SetVtx();
}

//==========================================================================
// �����擾
//==========================================================================
int CDelusionWindow::GetNumAll(void)
{
	return m_nNumAll;
}
