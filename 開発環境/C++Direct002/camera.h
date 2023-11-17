//=============================================================================
//
// �J�������� [camera.h]
// Author : ���n�Ή�
//
//=============================================================================
#ifndef _CAMERA_H_		// ���̃}�N����`������Ă��Ȃ�������
#define _CAMERA_H_		// ��d�C���N���[�h�h�~�̃}�N�����`����

#include "main.h"
#include "scene.h"

//==========================================================================
// �N���X��`
//==========================================================================
// �J�����N���X��`
class CCamera
{
private:

	// �񋓌^��`
	enum STATE
	{
		CAMERASTATE_NONE = 0,	// �ʏ�
		CAMERASTATE_SHAKE,		// �U��
	};

public:

	// �񋓌^��`
	enum CHASESTATE
	{
		CHASESTATE_NONE = 0,
		CHASESTATE_ENEMY,
		CHASESTATE_MAX
	};

	CCamera();
	~CCamera();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void SetCamera(void);
	void SetRotation(const D3DXVECTOR3 rot);	// �����ݒ�
	D3DXVECTOR3 GetRotation(void) const;		// �����擾
	float GetOriginDistance(void);				// ���ɂȂ�J�����̋����擾
	void SetShake(int nTime, float fLength, float fLengthY);	// �U���ݒ�
	void SetLenDest(float fLength, int nCntTime = 120, float DecrementValue = 2.0f, float fCorrection = 0.1f);	// �ڕW�̒����ݒ�
	void SetTargetPos(const D3DXVECTOR3 pos);	// �ڕW�̈ʒu�ݒ�
	CHASESTATE GetChaseState(void);				// �ǐՏ�Ԏ擾
	int GetChaseIndex(void);					// �ǂ��|����G�̃C���f�b�N�X�ԍ��擾
	void Reset(CScene::MODE mode);	// ���Z�b�g
private:

	// �����o�ϐ�
	void UpdateByMode(void);	// ���[�h�ʍX�V����
	void MoveCameraInput(void);
	void MoveCameraStick(void);
	void MoveCameraMouse(void);
	void MoveCameraVR(void);
	void MoveCameraV(void);
	void MoveCameraR(void);
	void MoveCameraDistance(void);
	void SetCameraV(void);
	void SetCameraVTitle(void);
	void SetCameraVGame(void);
	void SetCameraVResult(void);
	void SetCameraR(void);
	void SetCameraRTitle(void);
	void SetCameraRGame(void);
	void SetCameraRResult(void);
	void MoveCameraFollow(void);
	void Shake(void);
	void UpdateState(void);
	void UpdateChaseState(void);
	void ResetGame(void);	// ���Z�b�g
	void ResetTitle(void);	// ���Z�b�g
	void ResetResult(void);	// ���Z�b�g
	void ResetRanking(void);	// ���Z�b�g

	D3DXMATRIX	m_mtxProjection;	// �v���W�F�N�V�����}�g���b�N�X
	D3DXMATRIX	m_mtxView;			// �r���[�}�g���b�N�X
	D3DVIEWPORT9 m_viewport;		// �r���[�|�[�g
	D3DXVECTOR3 m_posV;				// ���_(�J�����̈ʒu)
	D3DXVECTOR3 m_posR;				// �����_(�������ꏊ)
	D3DXVECTOR3 m_posVDest;			// �ڕW�̎��_
	D3DXVECTOR3 m_posRDest;			// �ڕW�̒����_
	D3DXVECTOR3 m_vecU;				// ������x�N�g��
	D3DXVECTOR3 m_move;				// �ړ���
	D3DXVECTOR3 m_rot;				// ����
	D3DXVECTOR3 m_rotVDest;			// �ڕW�̎��_�̌���
	D3DXVECTOR3 m_TargetPos;		// �ڕW�̈ʒu
	float m_fDistance;				// ����
	float m_fDestDistance;			// �ڕW�̋���
	float m_fOriginDistance;		// ���̋���
	float m_nShakeLength;			// �h��̒���
	float m_nShakeLengthY;			// Y�̗h��̒���
	float m_fMoveShake;				// �h��̈ړ���
	float m_fMoveShakeY;			// Y�̗h��̈ړ���
	float m_fHeightMaxDest;			// �J�����̍ő卂���̖ڕW
	float m_fHeightMax;				// �J�����̍ő卂��
	float m_fDiffHeight;			// �����̍���
	float m_fDiffHeightSave;		// �����̍����ۑ��p
	float m_fDiffHeightDest;		// �ڕW�̍����̍���
	bool m_bFollow;					// �Ǐ]���邩�ǂ���
	int m_nCntState;				// ��ԃJ�E���^�[
	int m_nCntShaseState;			// �ǐՏ�ԃJ�E���^�[
	int m_nCntDistance;				// �����J�E���^�[
	int m_nOriginCntDistance;		// ���̋����J�E���^�[
	int m_nChaseIdxEnemy;			// �ǂ��|����C���f�b�N�X�ԍ�
	float m_fDistanceCorrection;	// �����̊����␳�W��
	float m_fDistanceDecrementValue;// �����̌����W��
	STATE m_state;					// ���
	CHASESTATE m_chaseState;		// �ǐՏ��
};

#endif