//=============================================================================
// 
//  �}�l�[�W���w�b�_�[ [manager.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _MANAGER_H_
#define _MANAGER_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "object.h"
#include "scene.h"

// �}�N����`
#define MOVE_LRUP			(0.75f)	// �΂ߏ�ړ�
#define MOVE_LR				(0.5f)	// �����ړ�
#define MOVE_LRDW			(0.25f)	// �΂ߏ�ړ�

// �O���錾
class CRenderer;
class CLight;
class CCamera;
class CInputKeyboard;
class CInputGamepad;
class CInputMouse;
class CSound;
class CDebugProc;
class CPlayer;
class CScore;
class CTimer;
class CBG;
class CMap;
class CTexture;
class CXLoad;
class CEdit;
class CElevation;
class CEnemyManager;
class CFade;
class CInstantFade;
class CPause;
class CPowerGauge;
class CResultManager;
class CRankingManager;

//==========================================================================
// �N���X��`
//==========================================================================
// �}�l�[�W���N���X��`
class CManager
{
public:
	CManager();
	~CManager();

	HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static CRenderer *GetRenderer(void);
	static CInputKeyboard *GetInputKeyboard(void);
	static CInputGamepad *GetInputGamepad(void);
	static CInputMouse *GetInputMouse(void);
	static CSound *GetSound(void);
	static CDebugProc *GetDebugProc(void);
	static CBG *GetBg(void);
	static CLight *GetLight(void);
	static CCamera *GetCamera(void);
	static CMap *GetMap(void);
	static CTexture *GetTexture(void);
	static CEdit *GetEdit(void);
	static CFade *GetFade(void);
	static CInstantFade *GetInstantFade(void);	// �J�ڂȂ��t�F�[�h�擾
	static CPause *GetPause(void);
	static CResultManager *GetResultManager(void);	// ���U���g�}�l�[�W���擾
	static CRankingManager *GetRankingManager(void);	// �����L���O�}�l�[�W���̃I�u�W�F�N�g
	static CScene *GetScene(void);		// �V�[���擾

	static void SetMode(CScene::MODE mode);						// ���̃��[�h�ݒ�
	static CScene::MODE GetMode(void);							// ���݂̃��[�h�擾
	static void SetEnableHitStop(int nCntHitStop);				// �q�b�g�X�g�b�v�̐ݒ�
	static bool IsWireframe(void) { return m_bWireframe; }		// ���C���[�t���[��
	static CScene::MODE GetOldMode(void) { return m_OldMode; }	// �O��̃��[�h�擾
private:

	static CRenderer *m_pRenderer;				// �����_���[�̃I�u�W�F�N�g
	static CInputKeyboard *m_pInputKeyboard;	// �L�[�{�[�h�̃I�u�W�F�N�g
	static CInputGamepad *m_pInputGamepad;		// �Q�[���p�b�h�̃I�u�W�F�N�g
	static CInputMouse *m_pInputMouse;			// �}�E�X�̃I�u�W�F�N�g
	static CSound *m_pSound;					// �T�E���h�̃I�u�W�F�N�g
	static CDebugProc *m_pDebugProc;			// �f�o�b�O�\���̃I�u�W�F�N�g
	static CBG *m_pBg;							// �w�i�̃I�u�W�F�N�g
	static CLight *m_pLight;					// ���C�g�̃I�u�W�F�N�g
	static CCamera *m_pCamera;					// �J�����̃I�u�W�F�N�g
	static CMap *m_pMap;						// �}�b�v�̃I�u�W�F�N�g
	static CTexture *m_pTexture;				// �e�N�X�`���̃I�u�W�F�N�g
	static CEdit *m_pEdit;						// �G�f�B�b�g�̃I�u�W�F�N�g
	static CScene *m_pScene;					// �V�[���̃I�u�W�F�N�g
	static CFade *m_pFade;						// �t�F�[�h�̃I�u�W�F�N�g
	static CInstantFade *m_pInstantFade;		// �J�ڂȂ��t�F�[�h�̃I�u�W�F�N�g
	static CPause *m_pPause;					// �|�[�Y�̃I�u�W�F�N�g
	static CResultManager *m_pResultManager;	// ���U���g�}�l�[�W���̃I�u�W�F�N�g
	static CRankingManager *m_pRankingManager;	// �����L���O�}�l�[�W���̃I�u�W�F�N�g

	static bool m_bWireframe;					// ���C���[�t���[��
	static bool m_bHitStop;						// �q�b�g�X�g�b�v�̔���
	static int m_nCntHitStop;					// �q�b�g�X�g�b�v�̃J�E���^�[
	static CScene::MODE m_OldMode;				// �O��̃��[�h
};


#endif