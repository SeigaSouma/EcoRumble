//=============================================================================
// 
//  ���w�b�_�[ [grassblades.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _GRASSBLADES_H_
#define _GRASSBLADES_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "object.h"

//==========================================================================
// �O���錾
//==========================================================================
class CMeshWall;

//==========================================================================
// �}�N����`
//==========================================================================
#define MAX_GRASSBLADES	(240)	// ���̍ő吔

//==========================================================================
// �N���X��`
//==========================================================================
// ���N���X��`
class CGrassBlades : public CObject
{
public:
	CGrassBlades(int nPriority = 7);
	~CGrassBlades();

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static CGrassBlades *Create(D3DXVECTOR3 pos);

private:

	// �����̍\���̒�`
	struct SGrassInfo
	{
		D3DXVECTOR3 rotDest;		// �ڕW�̌���
		D3DXVECTOR3 windDirection;	// ���̕���
		D3DXVECTOR3 windRot;		// ���̌���
		int nCntWind;				// ���̃J�E���^�[
		int nDirection;				// ����
		bool bHit;					// �������Ă邩
		CMeshWall *pMeshWall;		// ���b�V���E�H�[���̃|�C���^
	};

	// �����o�֐�
	void Falling(int nCntObj);
	void WindFalling(int nCntObj);

	// �����o�ϐ�
	SGrassInfo m_sGrassInfo[MAX_GRASSBLADES];	// �����
	static int m_nTexIdx;						// �e�N�X�`���̃C���f�b�N�X�ԍ�
};


#endif