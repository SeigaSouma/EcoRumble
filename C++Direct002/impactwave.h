//=============================================================================
// 
//  ���b�V���V�����_�[�w�b�_�[ [impactwave.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _IMPACTWAVE_H_
#define _IMPACTWAVE_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "object3DMesh.h"

//==========================================================================
// �N���X��`
//==========================================================================
// ���b�V���V�����_�[�N���X��`
class CImpactWave : public CObject3DMesh
{
public:
	//�񋓌^��`
	typedef enum
	{
		TYPE_BLACK = 0,
		TYPE_BLACK2,
		TYPE_ORANGE,
		TYPE_ORANGE2,
		TYPE_BLUE,
		TYPE_GREEN,
		TYPE_PURPLE,
		TYPE_PURPLE2,
		TYPE_PURPLE3,
		TYPE_PINK,
		TYPE_SMOKE,
		TYPE_GIZA,
		TYPE_GIZA2,
		TYPE_GIZABLACK,
		TYPE_GIZAWHITE,
		TYPE_MAX
	}TYPE;

	CImpactWave(int nPriority = 6);
	~CImpactWave();

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetVtx(void);

	static CImpactWave *Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXCOLOR col, float fWidth, float fHeight, int nLife, float fMove, int nTexType, bool bAddBlend);
	CImpactWave *GetObject3DMesh(void);

private:
	int m_nTexIdx;	// �e�N�X�`���̃C���f�b�N�X�ԍ�
	int m_nLife;	// ����
	int m_nMaxLife;	// �ő����
	D3DXCOLOR m_colOrigin;	// ���̐F
	float m_fOutWidth;		// ����(�O)
	float m_fInWidth;		// ��(��)
	float m_fRotWidth;		// 1������������̊p�x����
	float m_fHeight;		// ����
	float m_fMove;			// �L���鑬�x
	bool m_bAddBlend;		// ���Z�����̔���
	static const char *m_apFilename[TYPE_MAX];		//�t�@�C���ǂݍ���
};



#endif