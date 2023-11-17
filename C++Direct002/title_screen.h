//=============================================================================
// 
//  �^�C�g����ʃw�b�_�[ [title_screen.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _TITLESCREEN_H_
#define _TITLESCREEN_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "object.h"

//==========================================================================
// �O���錾
//==========================================================================
class CObject2D;

//==========================================================================
// �}�N����`
//==========================================================================

//==========================================================================
// �N���X��`
//==========================================================================
// �w�i�N���X��`
class CTitleScreen : public CObject
{
public:

	CTitleScreen(int nPriority = 8);
	~CTitleScreen();

	static CTitleScreen *Create(void);

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetVtx(void);
	void SetVtx(int nCntSelect);

private:
	// �񋓌^��`
	enum VTX
	{
		VTX_LOGO = 0,		// �^�C�g�����S
		VTX_SELECTSTART,	// �X�^�[�g
		VTX_LOGO_MAX
	};

	// �����o�֐�
	void UpdateLogo(int nCntSelect);	// ���S�̍X�V����
	void UpdateSelect(int nCntSelect);	// �I�����̍X�V����

	// �����o�ϐ�
	int m_nCntAlpha;		// �s�����x�̃J�E���^�[
	int m_nCntLogoMove;		// ���S�ړ��̃J�E���g
	int m_nCntLogoRotation;	// ���S��]�̃J�E���g
	static const char *m_apTextureFile[VTX_LOGO_MAX];	// �e�N�X�`���̃t�@�C��
	int m_nTexIdx[VTX_LOGO_MAX];						// �e�N�X�`���̃C���f�b�N�X�ԍ�
	CObject2D *m_pObj2D[VTX_LOGO_MAX];					// �I�u�W�F�N�g2D�̃I�u�W�F�N�g
};



#endif