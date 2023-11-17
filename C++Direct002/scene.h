//=============================================================================
// 
//  �V�[���w�b�_�[ [scene.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _SCENE_H_
#define _SCENE_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "elevation.h"

//==========================================================================
// �O���錾
//==========================================================================
class CXLoad;
class CWaveManager;
class CEnemyManager;
class CEnemyBase;
class CPlayer;

//==========================================================================
// �N���X��`
//==========================================================================
// �V�[���N���X��`
class CScene
{
public:
	// ���[�h�̗񋓌^��`
	enum MODE
	{
		MODE_NONE = 0,	// �^�C�g��
		MODE_TITLE,		// �^�C�g��
		MODE_TUTORIAL,	// �`���[�g���A��
		MODE_GAME,		// �Q�[��
		MODE_RESULT,	// ���U���g
		MODE_RANKING,	// �����L���O
		MODE_MAX
	};

	CScene();
	~CScene();

	// �I�[�o�[���C�h����֐�
	virtual HRESULT Init(void);
	virtual void Uninit(void);
	virtual void Update(void);
	virtual void Draw(void);

	static CScene *Create(MODE mode);
	static CXLoad *GetXLoad(void);				// X�t�@�C���̃f�[�^�擾
	static CElevation *GetObject3DMesh(void);	// �I�u�W�F�N�g3D�̎擾
	static CElevation *GetElevationOutside(int nIdx);		// �O���N���̒n�ʂ̃I�u�W�F�N�g
	CWaveManager *GetWaveManager(void);
	static CEnemyManager *GetEnemyManager(void);
	static CEnemyBase *GetEnemyBase(void);
	MODE GetMode(void);		// ���݂̃��[�h�擾

	virtual CPlayer *GetPlayer(void);
	virtual CPlayer **GetPlayer(int n);
private:

	MODE m_mode;				// ���݂̃��[�h
	CWaveManager *m_pWaveManager;		// �E�F�[�u�Ǘ��̃I�u�W�F�N�g
	static CXLoad *m_pXLoad;	// X�t�@�C���̃I�u�W�F�N�g
	static CElevation *m_pObject3DMesh;			// �I�u�W�F�N�g3D���b�V���̃I�u�W�F�N�g
	static CElevation *m_pElevationOutside[CElevation::POSTYPE_MAX];	// �O���N���̒n�ʂ̃I�u�W�F�N�g
	static CEnemyManager *m_pEnemyManager;		// �G�}�l�[�W���̃I�u�W�F�N�g
	static CEnemyBase *m_pEnemyBase;			// �G���_�̃I�u�W�F�N�g
};



#endif