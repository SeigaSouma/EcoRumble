//=============================================================================
//
// カメラ処理 [camera.h]
// Author : 相馬靜雅
//
//=============================================================================
#ifndef _CAMERA_H_		// このマクロ定義がされていなかったら
#define _CAMERA_H_		// 二重インクルード防止のマクロを定義する

#include "main.h"
#include "scene.h"

//==========================================================================
// クラス定義
//==========================================================================
// カメラクラス定義
class CCamera
{
private:

	// 列挙型定義
	enum STATE
	{
		CAMERASTATE_NONE = 0,	// 通常
		CAMERASTATE_SHAKE,		// 振動
	};

public:

	// 列挙型定義
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
	void SetRotation(const D3DXVECTOR3 rot);	// 向き設定
	D3DXVECTOR3 GetRotation(void) const;		// 向き取得
	float GetOriginDistance(void);				// 元になるカメラの距離取得
	void SetShake(int nTime, float fLength, float fLengthY);	// 振動設定
	void SetLenDest(float fLength, int nCntTime = 120, float DecrementValue = 2.0f, float fCorrection = 0.1f);	// 目標の長さ設定
	void SetTargetPos(const D3DXVECTOR3 pos);	// 目標の位置設定
	CHASESTATE GetChaseState(void);				// 追跡状態取得
	int GetChaseIndex(void);					// 追い掛ける敵のインデックス番号取得
	void Reset(CScene::MODE mode);	// リセット
private:

	// メンバ変数
	void UpdateByMode(void);	// モード別更新処理
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
	void ResetGame(void);	// リセット
	void ResetTitle(void);	// リセット
	void ResetResult(void);	// リセット
	void ResetRanking(void);	// リセット

	D3DXMATRIX	m_mtxProjection;	// プロジェクションマトリックス
	D3DXMATRIX	m_mtxView;			// ビューマトリックス
	D3DVIEWPORT9 m_viewport;		// ビューポート
	D3DXVECTOR3 m_posV;				// 視点(カメラの位置)
	D3DXVECTOR3 m_posR;				// 注視点(見たい場所)
	D3DXVECTOR3 m_posVDest;			// 目標の視点
	D3DXVECTOR3 m_posRDest;			// 目標の注視点
	D3DXVECTOR3 m_vecU;				// 上方向ベクトル
	D3DXVECTOR3 m_move;				// 移動量
	D3DXVECTOR3 m_rot;				// 向き
	D3DXVECTOR3 m_rotVDest;			// 目標の視点の向き
	D3DXVECTOR3 m_TargetPos;		// 目標の位置
	float m_fDistance;				// 距離
	float m_fDestDistance;			// 目標の距離
	float m_fOriginDistance;		// 元の距離
	float m_nShakeLength;			// 揺れの長さ
	float m_nShakeLengthY;			// Yの揺れの長さ
	float m_fMoveShake;				// 揺れの移動量
	float m_fMoveShakeY;			// Yの揺れの移動量
	float m_fHeightMaxDest;			// カメラの最大高さの目標
	float m_fHeightMax;				// カメラの最大高さ
	float m_fDiffHeight;			// 高さの差分
	float m_fDiffHeightSave;		// 高さの差分保存用
	float m_fDiffHeightDest;		// 目標の高さの差分
	bool m_bFollow;					// 追従するかどうか
	int m_nCntState;				// 状態カウンター
	int m_nCntShaseState;			// 追跡状態カウンター
	int m_nCntDistance;				// 距離カウンター
	int m_nOriginCntDistance;		// 元の距離カウンター
	int m_nChaseIdxEnemy;			// 追い掛けるインデックス番号
	float m_fDistanceCorrection;	// 距離の慣性補正係数
	float m_fDistanceDecrementValue;// 距離の減少係数
	STATE m_state;					// 状態
	CHASESTATE m_chaseState;		// 追跡状態
};

#endif