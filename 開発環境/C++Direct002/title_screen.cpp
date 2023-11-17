//=============================================================================
// 
//  �^�C�g����ʏ��� [title_screen.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "title_screen.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "object2D.h"
#include "calculation.h"

//==========================================================================
// �}�N����`
//==========================================================================
#define WIDTH		(640.0f)	// ����
#define HEIGHT		(360.0f)	// �c��
#define ALPHATIME	(60)		// �s�����x�X�V�̎���
#define LOGOMOVE_TIME	(180)	// ���S�ړ��̎���
#define LOGO_POSITION	(D3DXVECTOR3(640.0f, 160.0f, 0.0f))

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================
const char *CTitleScreen::m_apTextureFile[VTX_LOGO_MAX] =				// �e�N�X�`���̃t�@�C��
{
	"data\\TEXTURE\\titlelogo_01.png",
	"data\\TEXTURE\\titlelogo_02.png",
};

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CTitleScreen::CTitleScreen(int nPriority) : CObject(nPriority)
{
	// �l�̃N���A
	for (int nCntTex = 0; nCntTex < VTX_LOGO_MAX; nCntTex++)
	{
		m_nTexIdx[nCntTex] = 0;		// �e�N�X�`���̃C���f�b�N�X�ԍ�
		m_pObj2D[nCntTex] = NULL;
	}
	m_nCntAlpha = 0;		// �s�����x�̃J�E���^�[
	m_nCntLogoMove = 0;		// ���S�ړ��̃J�E���g
	m_nCntLogoRotation = 0;	// ���S��]�̃J�E���g
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CTitleScreen::~CTitleScreen()
{

}

//==========================================================================
// ��������
//==========================================================================
CTitleScreen *CTitleScreen::Create(void)
{
	// �����p�̃I�u�W�F�N�g
	CTitleScreen *pTitleScreen = NULL;

	if (pTitleScreen == NULL)
	{// NULL��������

		// �������̊m��
		pTitleScreen = DEBUG_NEW CTitleScreen;

		if (pTitleScreen != NULL)
		{// �������̊m�ۂ��o���Ă�����

			// ����������
			pTitleScreen->Init();
		}

		return pTitleScreen;
	}

	return NULL;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CTitleScreen::Init(void)
{
	// ��ނ̐ݒ�
	SetType(TYPE_TITLESCREEN);

	for (int nCntSelect = 0; nCntSelect < VTX_LOGO_MAX; nCntSelect++)
	{
		// ��������
		m_pObj2D[nCntSelect] = CObject2D::Create(6);

		// ��ނ̐ݒ�
		m_pObj2D[nCntSelect]->SetType(TYPE_TITLESELECT);

		// �e�N�X�`���̊��蓖��
		m_nTexIdx[nCntSelect] = CManager::GetTexture()->Regist(m_apTextureFile[nCntSelect]);

		// �e�N�X�`���̊��蓖��
		m_pObj2D[nCntSelect]->BindTexture(m_nTexIdx[nCntSelect]);

		D3DXVECTOR2 size;
		// �e��ϐ��̏�����
		switch (nCntSelect)
		{
		case VTX_LOGO:

			// �T�C�Y�擾
			size = CManager::GetTexture()->GetImageSize(m_nTexIdx[nCntSelect]);
			m_pObj2D[nCntSelect]->SetSize(size * 0.5f);	// �T�C�Y
			m_pObj2D[nCntSelect]->SetPosition(LOGO_POSITION);	// �ʒu
			break;

		case VTX_SELECTSTART:

			// �T�C�Y�擾
			size = CManager::GetTexture()->GetImageSize(m_nTexIdx[nCntSelect]);
			m_pObj2D[nCntSelect]->SetSize(D3DXVECTOR2(size.x * 0.4f, size.y * 0.4f));	// �T�C�Y
			m_pObj2D[nCntSelect]->SetPosition(D3DXVECTOR3(640.0f, 550.0f, 0.0f));	// �ʒu
			break;
		}
	}

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CTitleScreen::Uninit(void)
{
	for (int nCntSelect = 0; nCntSelect < VTX_LOGO_MAX; nCntSelect++)
	{
		if (m_pObj2D[nCntSelect] != NULL)
		{// NULL����Ȃ�������

			// �I������
			//m_pObj2D[nCntSelect]->Uninit();
			m_pObj2D[nCntSelect] = NULL;
		}
	}

	// ���폜
	Release();
}

//==========================================================================
// �X�V����
//==========================================================================
void CTitleScreen::Update(void)
{
	// �s�����x�̃J�E���^�[���Z
	//m_nCntAlpha = (m_nCntAlpha + 1) % (ALPHATIME * 2);

	for (int nCntSelect = 0; nCntSelect < VTX_LOGO_MAX; nCntSelect++)
	{
		if (m_pObj2D[nCntSelect] == NULL)
		{// NULL��������
			continue;
		}

		switch (nCntSelect)
		{
		case VTX_LOGO:
			// ���S�̍X�V����
			UpdateLogo(nCntSelect);
			break;

		case VTX_SELECTSTART:
			// �I�����̍X�V����
			UpdateSelect(nCntSelect);
			break;
		}

		// ���_���ݒ�
		SetVtx(nCntSelect);
	}
}

//==========================================================================
// ���S�̍X�V����
//==========================================================================
void CTitleScreen::UpdateLogo(int nCntSelect)
{
	// �ʒu�擾
	D3DXVECTOR3 pos = m_pObj2D[nCntSelect]->GetPosition();
	D3DXVECTOR3 rot = m_pObj2D[nCntSelect]->GetRotation();

	// ���S�ړ��̃J�E���g���Z
	m_nCntLogoMove = (m_nCntLogoMove + 1) % (LOGOMOVE_TIME * 2);
	m_nCntLogoRotation = (m_nCntLogoRotation + 1) % (360 * 2);	// ���S��]�̃J�E���g

	// ������ς���
	float fHeight = sinf(D3DX_PI * ((float)m_nCntLogoMove / (float)LOGOMOVE_TIME)) * 10.0f;
	pos.y = LOGO_POSITION.y + fHeight;

	// ������ς���
	float fRot = sinf(D3DX_PI * ((float)m_nCntLogoRotation / (float)360)) * 0.075f;
	rot.z = fRot;

	// �ʒu�ݒ�
	m_pObj2D[nCntSelect]->SetPosition(pos);
	m_pObj2D[nCntSelect]->SetRotation(rot);
}

//==========================================================================
// �I�����̍X�V����
//==========================================================================
void CTitleScreen::UpdateSelect(int nCntSelect)
{
	// �F�擾
	D3DXCOLOR col = m_pObj2D[nCntSelect]->GetColor();

	// �s�����x�X�V
	//col.a = sinf(D3DX_PI * ((float)m_nCntAlpha / (float)ALPHATIME));
	CuadricCurveComp(col.a, ALPHATIME, 0.1f, 1.0f, m_nCntAlpha);

	// �F�ݒ�
	m_pObj2D[nCntSelect]->SetColor(col);
}

//==========================================================================
// �`�揈��
//==========================================================================
void CTitleScreen::Draw(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// �A���t�@�e�X�g��L���ɂ���
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	for (int nCntSelect = 0; nCntSelect < VTX_LOGO_MAX; nCntSelect++)
	{
		// �`�揈��
		m_pObj2D[nCntSelect]->Draw();
	}

	// �A���t�@�e�X�g�𖳌��ɂ���
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
}

//==========================================================================
// ���_���ݒ菈��
//==========================================================================
void CTitleScreen::SetVtx(void)
{

}

//==========================================================================
// ���_���ݒ菈��
//==========================================================================
void CTitleScreen::SetVtx(int nCntSelect)
{
	// ���_�ݒ�
	m_pObj2D[nCntSelect]->SetVtx();
}
