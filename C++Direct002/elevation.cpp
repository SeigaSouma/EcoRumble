//=============================================================================
// 
//  起伏処理 [elevation.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "elevation.h"
#include "manager.h"
#include "renderer.h"
#include "game.h"
#include "texture.h"
#include "input.h"
#include "calculation.h"
#include "debugproc.h"
#include "particle.h"
#include "3D_effect.h"
#include "player.h"
#include "model.h"
#include "ballast.h"
#include "power_gauge.h"
#include "boss.h"
#include "enemy.h"
#include "targetpoint.h"
#include "impactwave.h"
#include "camera.h"
#include "enemymanager.h"
#include "enemybase.h"
#include "sound.h"
#include "tutorial.h"
#include "tutorialstep.h"
#include "ranking.h"

// 派生先
#include "elevation_outside.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define TEXTURE		"data\\TEXTURE\\glass.jpg"	// テクスチャ
#define MAX_MOVE	(8.0f)		// 移動量
#define MAX_RANGE	(200.0f)	// 範囲
#define MOVE_LEN	(1.0f)		// 幅移動量
#define MIN_HEIGHT	(-400.0f)	// 最低の高さ
#define MOVE		(5.0f)
#define IMPACTSHAKE_DIRECT	(30.0f)
#define IMPACTSHAKE_IMPACT	(20.0f)
#define SHAKETIME_DIRECT	(15 + 5)
#define SHAKETIME_IMPACT	(10 + 5)
#define SPAWN_WAITTIME		(20)
#if 1
#define CHANGE	(0)
#endif

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
CWaveManager::SetStageInfo CElevation::m_aSetInfo[mylib_const::MAX_WAVE_ENEMY] = {};	// 敵の配置情報
bool CElevation::m_bLoadInfo = false;		// 情報読み込み判定
bool CElevation::m_bSetSpawnCamera = false;	// スポーン時のカメラ設定判定
int CElevation::m_nCntSetSpawn = 0;			// スポーン時のカメラカウント

//==========================================================================
// コンストラクタ
//==========================================================================
CElevation::CElevation(int nPriority) : CObject3DMesh(nPriority)
{
	memset(&m_aWave, NULL, sizeof(m_aWave));	// 起伏の情報
	m_fBrushStrength = 0.0f;					// ブラシ強さ
	m_fBrushRange = 0.0f;						// 範囲
	m_bEdit = false;							// エディットON/OFF
	m_nTexIdx = 0;								// テクスチャのインデックス番号
	memset(&m_VtxPos, 0, sizeof(m_VtxPos));		// 頂点座標
	memset(&m_sFadeInfo, NULL, sizeof(m_sFadeInfo));		// フェード時の情報

	m_aInfo.TextureFileName = {};				// テクスチャファイル名
	m_aInfo.pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 位置
	m_aInfo.fWidthLength = 0.0f;				// 横長さ
	m_aInfo.fHeightLength = 0.0f;				// 縦長さ
	m_aInfo.nWidthBlock = 0;					// 横分割数
	m_aInfo.nHeightBlock = 0;					// 縦分割数
	m_aInfo.pVtxPos = NULL;						// 頂点座標

	for (int nCntStage = 0; nCntStage < mylib_const::MAX_STAGE; nCntStage++)
	{
		m_aInfo.pStageVtxPos[nCntStage] = NULL;	// ステージ毎の頂点座標
	}
	m_pTargetP = NULL;							// 目標の地点
	m_nSpawnByFrame = 0;						// フレーム毎のスポーンカウント
	m_bSpawnByFrame = false;					// フレーム毎のスポーン判定
	m_bHitStop_Direct = false;					// 直線波のヒットストップ判定
	m_bHitStop_Impact = false;					// 円形波のヒットストップ判定
}

//==========================================================================
// デストラクタ
//==========================================================================
CElevation::~CElevation()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CElevation *CElevation::Create(const char *pText, CObject::TYPE type)
{
	// 生成用のオブジェクト
	CElevation *pObjMeshField = NULL;

	if (pObjMeshField == NULL)
	{// NULLだったら

		// メモリの確保
		switch (type)
		{
		case CObject::TYPE_ELEVATION:
			pObjMeshField = DEBUG_NEW CElevation;
			break;

		case CObject::TYPE_ELEVATION_OUTSIDE:
			pObjMeshField = DEBUG_NEW CElevationOutside;
			break;

		default:
			break;
		}

		if (pObjMeshField != NULL)
		{// メモリの確保が出来ていたら

			// 初期化処理
			pObjMeshField->Init(pText);
		}

		return pObjMeshField;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CElevation::Init(void)
{
	//for (int nCntHeight = 0; nCntHeight < m_aInfo.nHeightBlock; nCntHeight++)
	//{// 縦の分割分繰り返す

	//	for (int nCntWidth = 0; nCntWidth < m_aInfo.nWidthBlock; nCntWidth++)
	//	{// 横の分割分繰り返す

	//		int nNowVtx = nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1));

	//		// 0ステージの情報にする
	//		m_aInfo.pVtxPos[nNowVtx] = m_aInfo.pStageVtxPos[0][nNowVtx];
	//		pVtxPos[nNowVtx] = m_aInfo.pVtxPos[nNowVtx];
	//	}
	//}

	return E_FAIL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CElevation::Init(const char *pText)
{
	HRESULT hr;

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// テクスチャの割り当て
	m_nTexIdx = CManager::GetTexture()->Regist(TEXTURE);

	// テクスチャの割り当て
	BindTexture(m_nTexIdx);

	// 種類設定
	SetType(TYPE_ELEVATION);

	// ロード処理
	hr = Load(pText);

	if (FAILED(hr))
	{// 失敗していたら
		return E_FAIL;
	}

	// 各種変数初期化
	SetPosition(m_aInfo.pos);				// 位置
	SetWidthBlock(m_aInfo.nWidthBlock);		// 幅部活
	SetHeightBlock(m_aInfo.nHeightBlock);	// 縦分割
	SetWidthLen(m_aInfo.fWidthLength);		// 縦長さ
	SetHeightLen(m_aInfo.fHeightLength);	// 横長さ
	m_fBrushStrength = MAX_MOVE;			// ブラシ強さ
	m_fBrushRange = MAX_RANGE;				// 範囲
	m_bHitStop_Direct = false;				// ヒットストップの判定
	m_bHitStop_Impact = false;				// 円形波のヒットストップ判定

	// オブジェクト3Dメッシュの初期化処理
	hr = CObject3DMesh::Init(CObject3DMesh::TYPE_FIELD);

	if (FAILED(hr))
	{// 失敗していたら
		return E_FAIL;
	}

	// 頂点数分メモリ確保
	int nVtxNum = GetNumVertex();
	m_sFadeInfo.pRot = DEBUG_NEW float[nVtxNum];
	memset(&m_sFadeInfo.pRot[0], 0, sizeof(float) * nVtxNum);

	if (CManager::GetMode() == CScene::MODE_RESULT)
	{
		for (int nCntHeight = 0; nCntHeight < m_aInfo.nHeightBlock + 1; nCntHeight++)
		{// 縦の分割分繰り返す

			for (int nCntWidth = 0; nCntWidth < m_aInfo.nWidthBlock + 1; nCntWidth++)
			{// 横の分割分繰り返す
				m_VtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y = m_aInfo.pStageVtxPos[3][nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y;
			}
		}
	}

	if (CManager::GetMode() == CScene::MODE_RANKING)
	{
		int nStage = CRanking::GetRandStage();
		for (int nCntHeight = 0; nCntHeight < m_aInfo.nHeightBlock + 1; nCntHeight++)
		{// 縦の分割分繰り返す

			for (int nCntWidth = 0; nCntWidth < m_aInfo.nWidthBlock + 1; nCntWidth++)
			{// 横の分割分繰り返す
				m_VtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y = m_aInfo.pStageVtxPos[nStage][nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y;
				m_aInfo.pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y = m_aInfo.pStageVtxPos[nStage][nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y;
			}
		}
	}

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CElevation::Uninit(void)
{
	// 頂点座標の破棄
	if (m_aInfo.pVtxPos != NULL)
	{
		delete[] m_aInfo.pVtxPos;
		m_aInfo.pVtxPos = NULL;
	}

	if (m_sFadeInfo.pRot != NULL)
	{
		delete[] m_sFadeInfo.pRot;
		m_sFadeInfo.pRot = NULL;
	}


	// ステージ毎頂点座標の破棄
	for (int nCntStage = 0; nCntStage < mylib_const::MAX_STAGE; nCntStage++)
	{
		if (m_aInfo.pStageVtxPos[nCntStage] != NULL)
		{
			delete[] m_aInfo.pStageVtxPos[nCntStage];
			m_aInfo.pStageVtxPos[nCntStage] = NULL;
		}
	}

	// 目標の地点消す
	if (m_pTargetP != NULL)
	{
		m_pTargetP->Uninit();
		m_pTargetP = NULL;
	}

	// 終了処理
	CObject3DMesh::Uninit();
}

//==========================================================================
// 解放処理
//==========================================================================
void CElevation::Release(void)
{
	// 頂点座標の破棄
	if (m_aInfo.pVtxPos != NULL)
	{
		delete[] m_aInfo.pVtxPos;
		m_aInfo.pVtxPos = NULL;
	}

	// ステージ毎頂点座標の破棄
	for (int nCntStage = 0; nCntStage < mylib_const::MAX_STAGE; nCntStage++)
	{
		if (m_aInfo.pStageVtxPos[nCntStage] != NULL)
		{
			delete[] m_aInfo.pStageVtxPos[nCntStage];
			m_aInfo.pStageVtxPos[nCntStage] = NULL;
		}
	}
}

//==========================================================================
// 情報登録
//==========================================================================
void CElevation::SetNaturalWaveInfo(CWaveManager::SetInfo *info, int nStage, int nWave, int nSpawnNum)
{
	for (int nCntSpawn = 0; nCntSpawn < nSpawnNum; nCntSpawn++)
	{
		// 情報渡す
		m_aSetInfo[nStage].aWaveInfo[nWave].Info[nCntSpawn] = info[nCntSpawn];
	}

	// 出現する数加算
	m_aSetInfo[nStage].aWaveInfo[nWave].nSetNum = nSpawnNum;
}

//==========================================================================
// 更新処理
//==========================================================================
void CElevation::Update(void)
{
	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();

	// コンボ演出中は抜ける
	if (CGame::GetEnemyManager()->GetState() == CEnemyManager::STATE_COMBOANIM)
	{
		return;
	}

	// 高さリセット
	if (CManager::GetMode() != CScene::MODE_RESULT)
	{
		memset(&m_VtxPos[0], 0, sizeof(m_VtxPos));
	}

	if (m_bSpawnByFrame == true)
	{
		// フレーム毎のスポーンカウント加算
		m_nSpawnByFrame++;
		if (m_nSpawnByFrame >= 5)
		{
			m_nSpawnByFrame = 0;
			m_bSpawnByFrame = false;
		}
	}

	if (pInputKeyboard->GetTrigger(DIK_F3) == true)
	{// 切り替え
		m_bEdit = m_bEdit ? false : true;

		// 目標の地点消す
		if (m_pTargetP != NULL)
		{
			m_pTargetP->Uninit();
			m_pTargetP = NULL;
		}
		// 目標の地点生成
		else if (m_pTargetP == NULL)
		{
			m_pTargetP = CTargetPoint::Create(D3DXVECTOR3(0.0f, 0.0f, 0.0f), 400.0f, 800.0f);
		}
	}

	// 頂点操作
	if (m_bEdit == true)
	{// エディット中なら
		UPVtxField(m_pTargetP->GetPosition());
	}
	else
	{
		// 状態更新処理
		UpdateState();
	}

	if (m_bSetSpawnCamera == true)
	{// 設定判定があれば

		m_nCntSetSpawn = (m_nCntSetSpawn + 1) % 3;
		if (m_nCntSetSpawn == 0)
		{
			// カメラ揺れ追加
			CManager::GetCamera()->SetShake(10, 5.0f, 0.0f);
		}
	}

#if 0
	for (int nCntPriority = 0; nCntPriority < mylib_const::PRIORITY_NUM; nCntPriority++)
	{
		// 先頭を保存
		CObject *pObj = CObject::GetTop(nCntPriority);

		while (pObj != NULL)
		{// NULLが来るまで無限ループ

			// 次のオブジェクトを一時保存
			CObject *pObjNext = pObj->GetNext();

			// 死亡の判定
			if (pObj->IsDeath() == true)
			{// 死亡フラグが立っていたら

				// 次のオブジェクトを代入
				pObj = pObjNext;
				continue;
			}

			// 種類の取得
			if (pObj->GetType() == CObject::TYPE_XFILE ||
				pObj->GetType() == CObject::TYPE_EDIT)
			{// 既定のタイプだったら

				D3DXVECTOR3 pos = pObj->GetPosition();
				D3DXVECTOR3 move = pObj->GetMove();
				bool bLand = false, bIn = false;

				// 高さ取得
				float fHeight = GetHeight(pos, move, bIn, pObj->GetType());

				if (fHeight > pos.y)
				{// 地面の方が自分より高かったら
					pos.y = fHeight;
					pObj->SetPosition(pos);
					move.y = 0.0f;

					if (bIn == true)
					{
						bLand = true;
					}
				}

				if (bLand == true)
				{// 乗っていたら
					move.y = 0.0f;
				}

				// 移動量設定
				pObj->SetMove(move);
			}

			// 次のオブジェクトを代入
			pObj = pObjNext;
		}
	}
#endif
	// 頂点情報設定
	SetVtx();

	// ヒットストップの判定
	if (m_bHitStop_Direct == true || m_bHitStop_Impact == true)
	{
		static int nCntHitStop = 0;
		nCntHitStop = (nCntHitStop + 1) % 5;

		// ヒットストップ有効
		if (nCntHitStop == 0)
		{
			if (m_bHitStop_Direct == true)
			{
				CManager::SetEnableHitStop(SHAKETIME_DIRECT - 5);
			}
			else if (m_bHitStop_Impact == true)
			{
				CManager::SetEnableHitStop(SHAKETIME_IMPACT - 5);
			}
			m_bHitStop_Direct = false;
			m_bHitStop_Impact = false;
		}
	}

}

//==========================================================================
// ステージ切り替え
//==========================================================================
void CElevation::StageSwitch(int nNowStage, int nNextStage, int nNowLife, int nMaxLife)
{

	// 頂点カラー取得
	D3DXCOLOR *pVtxCol = GetVtxCol();
	*pVtxCol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	if (nNowStage <= 0)
	{
		nNowStage = 0;
	}

	// 頂点座標取得
	D3DXVECTOR3 *pVtxPos = GetVtxPos();

	int nNoneTime = 60;
	float fHalfMaxLife = (float)((nMaxLife / 2) - nNoneTime);
	float fHalfAfterLife = (float)((nMaxLife / 2) + nNoneTime);
	float fRotMove = D3DX_PI * 0.05f;

	// 振動
	if (nNowLife % 3 == 0)
	{
		CManager::GetCamera()->SetShake(15, 5.0f, 0.0f);
	}

	if (GetType() == TYPE_ELEVATION)
	{
		int nRand;
		for (int i = 0; i < 10; i++)
		{
			nRand = Random(0, GetNumVertex() - 1);
			my_particle::Create(D3DXVECTOR3(pVtxPos[nRand].x, pVtxPos[nRand].y - 5.0f, pVtxPos[nRand].z), my_particle::TYPE_WAVESMOKE);
		}
	}

	float fMoveTime = (float)(fHalfMaxLife - nNowLife) / fHalfMaxLife;

	if (nNowLife < (nMaxLife / 2) - nNoneTime)
	{// 前半

		fMoveTime = (float)(fHalfMaxLife - nNowLife) / fHalfMaxLife;

		m_sFadeInfo.state = FADESTATE_INCREMENT;
		fRotMove *= -1;
	}
	else if (nNowLife >= (nMaxLife / 2) - nNoneTime && nNowLife < (nMaxLife / 2) + nNoneTime)
	{// 平らな時間

		fMoveTime = (float)(fHalfMaxLife - nNowLife) / fHalfMaxLife;

		m_sFadeInfo.state = FADESTATE_NONE;
		int nStartCnt = -nNoneTime;
		int nHalfCnt = 0;
		int nEndCnt = nNoneTime;
		int nLife = nNowLife - ((nMaxLife / 2) - nNoneTime);

		fRotMove *= -1;
		float fRotMoveDest = D3DX_PI * 0.05f;
		float fRotDiff = fRotMoveDest - fRotMove;
		fRotMove = (fRotDiff / (float)(nNoneTime * 2)) * nLife;
		RotNormalize(fRotMove);
		fRotMove -= D3DX_PI * 0.05f;
	}
	else
	{// 後半
		int nEndCnt = (nMaxLife - 1) - ((nMaxLife / 2) + nNoneTime);
		int nLife = nNowLife - ((nMaxLife / 2) + nNoneTime);

		fMoveTime = (float)(nLife) / (float)nEndCnt;
		m_sFadeInfo.state = FADESTATE_DECREMENT;
	}

	for (int nCntHeight = 0; nCntHeight < m_aInfo.nHeightBlock + 1; nCntHeight++)
	{// 縦の分割分繰り返す

		for (int nCntWidth = 0; nCntWidth < m_aInfo.nWidthBlock + 1; nCntWidth++)
		{// 横の分割分繰り返す

			int nNowVtx = nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1));
			float fHeightDest = m_aInfo.pStageVtxPos[nNextStage][nNowVtx].y;

			if (m_sFadeInfo.state == FADESTATE_INCREMENT)
			{// 前半
				fHeightDest = 200.0f;
				m_sFadeInfo.fLength = (5040.0f - 0.0f) * (1.0f - fMoveTime);
			}
			else if (m_sFadeInfo.state == FADESTATE_NONE)
			{// 平らな時間
				fHeightDest = 200.0f;
				m_sFadeInfo.fLength = 5040.0f;
			}
			else
			{// 後半
				int nLife = nNowLife - ((nMaxLife / 2) + nNoneTime);
				int nEnd = nMaxLife - ((nMaxLife / 2) + nNoneTime);
				float fDiff = (5040.0f - 0.0f) / (float)nEnd;
				m_sFadeInfo.fLength = fDiff * (nEnd - nLife);
			}

			float fDiffY;


			// 頂点の高さ補正
			if (m_sFadeInfo.state == FADESTATE_INCREMENT)
			{// 前半
				fDiffY = fHeightDest - m_aInfo.pStageVtxPos[nNowStage][nNowVtx].y;
				float f = 200.0f + fDiffY * -fMoveTime;
				m_aInfo.pVtxPos[nNowVtx].y = f;
			}
			else if (m_sFadeInfo.state == FADESTATE_NONE)
			{// 平らな時間
				m_aInfo.pVtxPos[nNowVtx].y = fHeightDest;
			}
			else
			{// 後半
				fDiffY = fHeightDest - 200.0f;
				m_aInfo.pVtxPos[nNowVtx].y = 200.0f + fDiffY * fMoveTime;
			}

			// 原点と頂点の距離
			float fNowLength =
				sqrtf((0.0f - pVtxPos[nNowVtx].x) * (0.0f - pVtxPos[nNowVtx].x)
					+ (0.0f - pVtxPos[nNowVtx].z) * (0.0f - pVtxPos[nNowVtx].z));

			// 割合
			float Wariai = fNowLength / (GetWidthLen() * 8.0f);
			float WaveWariai = fNowLength / m_sFadeInfo.fLength;

#ifdef CHANGE
			float fStrength = 300.0f;
#else
			float fStrength = 80.0f;
#endif

			if (m_sFadeInfo.state == FADESTATE_INCREMENT)
			{// 前半
				fStrength = fStrength * (1.0f - fMoveTime);
			}
			else if (m_sFadeInfo.state == FADESTATE_NONE)
			{// 平らな時間
				int n = 0;
			}
			else
			{// 後半
				fStrength = fStrength * (1.0f - fMoveTime);
			}

			// 波の高さ
			float fWaveStrength = fStrength;

			if (WaveWariai >= 1.0f)
			{// 範囲外は移動量ゼロ
				fWaveStrength = 0.0f;
			}

			// 波の向き
			float fRot = D3DX_PI * Wariai;

			// ウェーブさせる
			m_sFadeInfo.pRot[nNowVtx] += fRotMove;

			// 角度の正規化
			RotNormalize(fRot);
			RotNormalize(m_sFadeInfo.pRot[nNowVtx]);

			// 波の高さ設定
			float fHeight = sinf(fRot + m_sFadeInfo.pRot[nNowVtx]) * fWaveStrength;

			if (fHeightDest >= 0.0f && fHeight <= 0.0f)
			{// 前半 && マイナスの高さ
				fHeight = 0.0f;
			}

			if (m_sFadeInfo.state != FADESTATE_DECREMENT && m_aInfo.pVtxPos[nNowVtx].y >= 0.0f)
			{// 前半

				if (fHeight > m_aInfo.pVtxPos[nNowVtx].y)
				{// 波の方が高いやつだけウェーブ対象
					m_aInfo.pVtxPos[nNowVtx].y = fHeight;
				}

			}
			else if(m_sFadeInfo.state == FADESTATE_DECREMENT)
			{
				if (m_aInfo.pVtxPos[nNowVtx].y >= 0.0f)
				{// 凸

					if (fHeight != 0.0f)
					{
						int n = 0;
					}

					if (fHeight > m_aInfo.pVtxPos[nNowVtx].y)
					{// 波の方が高いやつだけウェーブ対象
						m_aInfo.pVtxPos[nNowVtx].y = fHeight;
					}

				}
			}

		}
	}
}

//==========================================================================
// 状態更新処理
//==========================================================================
void CElevation::UpdateState(void)
{
	for (int nCntWave = 0; nCntWave < mylib_const::MAX_WAVE; nCntWave++)
	{

		// 頂点カラー取得
		D3DXCOLOR *pVtxCol = GetVtxCol();
		*pVtxCol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		switch (m_aWave[nCntWave].state)
		{
		case STATE_NONE:

			break;

		case STATE_SET:

			// 何もない状態に戻す
			m_aWave[nCntWave].state = STATE_NONE;
			break;

		case STATE_WAVE:

			// 波の強さを減らしていく
			if (m_aWave[nCntWave].sSetWave.type != WAVETYPE_RAISE && m_aWave[nCntWave].nCntState >= 0)
			{// 自然の波以外

				if (m_aWave[nCntWave].sSetWave.playertype == TYPE_NATURAL)
				{// 自然の波
					m_aWave[nCntWave].sSetWave.fStrength =
						m_aWave[nCntWave].sSetWave.fStrengthOrigin * ((float)(m_aWave[nCntWave].nCntState) / (float)(m_aWave[nCntWave].nOriginCntState));
				}
				else if (m_aWave[nCntWave].nMoveLife > 0)
				{// 移動寿命が尽きてから

					if (m_aWave[nCntWave].sSetWave.type == WAVETYPE_RIPPLE)
					{// 波紋の時
						m_aWave[nCntWave].nCntState -= m_aWave[nCntWave].nMoveLife;
						m_aWave[nCntWave].nMoveLife = 0;
					}

					m_aWave[nCntWave].sSetWave.fStrength =
						m_aWave[nCntWave].sSetWave.fStrengthOrigin * ((float)(m_aWave[nCntWave].nCntState - 180) / (float)(m_aWave[nCntWave].nOriginCntState - 180));

					if (m_aWave[nCntWave].sSetWave.fStrength >= m_aWave[nCntWave].sSetWave.fStrengthOrigin * 0.5f)
					{// 規定値を下回らないように補正
						m_aWave[nCntWave].sSetWave.fStrength = m_aWave[nCntWave].sSetWave.fStrengthOrigin * 0.5f;
					}
				}
				else
				{
					m_aWave[nCntWave].sSetWave.fStrength =
						(m_aWave[nCntWave].sSetWave.fStrengthOrigin * 0.5f) * ((float)m_aWave[nCntWave].nCntState / (float)m_aWave[nCntWave].nOriginCntState);
				}
			}
			else if(m_aWave[nCntWave].sSetWave.type == WAVETYPE_RAISE && m_aWave[nCntWave].nCntState >= 0)
			{// 盛り上げの波

				switch (m_aWave[nCntWave].sSetWave.playertype)
				{
				case TYPE_NATURAL:
					m_aWave[nCntWave].sSetWave.fStrength =
						m_aWave[nCntWave].sSetWave.fStrengthOrigin * (1.0f - ((float)(m_aWave[nCntWave].nCntState / (float)m_aWave[nCntWave].nOriginCntState)));
					break;

				case TYPE_SPAWN:
				if(m_aWave[nCntWave].nMoveLife > 0)
				{// 移動寿命がある時

					float fOrigin = (float)m_aWave[nCntWave].nOriginCntState * 0.5f;	// 元のカウントの半分
					m_aWave[nCntWave].sSetWave.fStrength =
						m_aWave[nCntWave].sSetWave.fStrengthOrigin * (1.0f - ((float)(m_aWave[nCntWave].nMoveLife / (float)m_aWave[nCntWave].nOriginMoveLife)));

					if (m_aWave[nCntWave].WaveManagerInfo.nSpawnType == CEnemyManager::SPAWNTYPE_CIRCLE ||
						m_aWave[nCntWave].WaveManagerInfo.nSpawnType == CEnemyManager::SPAWNTYPE_CLOCK ||
						m_aWave[nCntWave].WaveManagerInfo.nSpawnType == CEnemyManager::SPAWNTYPE_BOTHCLOCK)
					{// 複数指定されているとき
						// 目標の距離設定
						CManager::GetCamera()->SetLenDest(CManager::GetCamera()->GetOriginDistance() + 300.0f);
					}
				}
				else if (m_aWave[nCntWave].nCntState <= m_aWave[nCntWave].nOriginCntState / 2 && m_aWave[nCntWave].nWaitTime >= 0)
				{// 出現するカウントの時

					// 待機時間減算
					m_aWave[nCntWave].nWaitTime--;

					// カメラ設定の判定オフ
					m_bSetSpawnCamera = false;
					m_nCntSetSpawn = 0;

					if (m_aWave[nCntWave].WaveManagerInfo.nSpawnType == CEnemyManager::SPAWNTYPE_CIRCLE ||
						m_aWave[nCntWave].WaveManagerInfo.nSpawnType == CEnemyManager::SPAWNTYPE_CLOCK ||
						m_aWave[nCntWave].WaveManagerInfo.nSpawnType == CEnemyManager::SPAWNTYPE_BOTHCLOCK)
					{// 複数指定されているとき
						// 目標の距離設定
						CManager::GetCamera()->SetLenDest(CManager::GetCamera()->GetOriginDistance() + 300.0f);
					}

					if (m_aWave[nCntWave].nWaitTime < 0)
					{// 待機時間がなくなったら

						// 敵設定
						SetEnemy(nCntWave);
					}
				}
				else if(m_aWave[nCntWave].nCntState <= m_aWave[nCntWave].nOriginCntState / 2 - SPAWN_WAITTIME && m_aWave[nCntWave].nWaitTime <= 0)
				{// 移動寿命がなくなったら
					float fOrigin = (float)m_aWave[nCntWave].nOriginCntState * 0.5f - SPAWN_WAITTIME;	// 元のカウントの半分
					m_aWave[nCntWave].sSetWave.fStrength =
						m_aWave[nCntWave].sSetWave.fStrengthOrigin * (((float)(m_aWave[nCntWave].nCntState / fOrigin)));
				}
					break;

				default:
					break;
				}
				
			}

			// 波処理
			Wave(nCntWave);

			if (m_aWave[nCntWave].nMoveLife > 0 && m_aWave[nCntWave].sSetWave.type != WAVETYPE_RAISE)
			{// 移動寿命があるうち

				// 当たり判定
				Collision(nCntWave);
			}

			// 減少時間
			int nDecrementTime = m_aWave[nCntWave].nOriginCntState / 2 - SPAWN_WAITTIME;

			// 状態遷移カウンター減算
			if (m_aWave[nCntWave].nWaitTime <= 0 || nDecrementTime <= m_aWave[nCntWave].nCntState)
			{
				m_aWave[nCntWave].nCntState--;
			}

			if (m_aWave[nCntWave].nCntState <= 0 && m_aWave[nCntWave].sSetWave.playertype != TYPE_NATURAL)
			{
				// 頂点保存状態に移行
				m_aWave[nCntWave].state = STATE_SET;
			}

			if (m_aWave[nCntWave].sSetWave.type == WAVETYPE_RAISE && m_aWave[nCntWave].sSetWave.playertype == TYPE_NATURAL &&
				m_aWave[nCntWave].nCntState % 15 == 0)
			{// 自然の波

				// 目印
				my_particle::Create(D3DXVECTOR3(
					m_aWave[nCntWave].sSetWave.pos.x + Random(-40, 40),
					m_aWave[nCntWave].sSetWave.pos.y,
					m_aWave[nCntWave].sSetWave.pos.z + Random(-40, 40)), my_particle::TYPE_NATURALWAVE_SPAWN);
			}

			// 移動寿命減算
			if (m_aWave[nCntWave].nWaitTime <= 0 || nDecrementTime <= m_aWave[nCntWave].nCntState)
			{
				m_aWave[nCntWave].nMoveLife--;
			}

			if (m_aWave[nCntWave].nMoveLife <= 0)
			{// 移動寿命がなくなったら

				m_aWave[nCntWave].nMoveLife = 0;

				if (m_aWave[nCntWave].sSetWave.playertype == TYPE_NATURAL && m_aWave[nCntWave].sSetWave.type == WAVETYPE_DIRECT)
				{// 自然の波 && 直線

					m_aWave[nCntWave].nCntState = m_aWave[nCntWave].nOriginCntState;
					m_aWave[nCntWave].nMoveLife = m_aWave[nCntWave].nOriginMoveLife;
					m_aWave[nCntWave].sSetWave.pos = m_aWave[nCntWave].sSetWave.posOrigin;
				}
			}

			break;
		}
	}
}

//==========================================================================
// 敵の設定
//==========================================================================
void CElevation::SetEnemy(int nCntWave)
{

	// 現在のステージ取得
	int nStage = CManager::GetScene()->GetWaveManager()->GetNowStage();

	CWaveManager::SetInfo info = m_aWave[nCntWave].WaveManagerInfo;

	int nNumEnemy = m_aWave[nCntWave].nNumEnemy;
	// 敵出現
	if (info.nSpawnType == CEnemyManager::SPAWNTYPE_CIRCLE ||
		info.nSpawnType == CEnemyManager::SPAWNTYPE_CLOCK ||
		info.nSpawnType == CEnemyManager::SPAWNTYPE_BOTHCLOCK)
	{// 複数指定されているとき
		nNumEnemy = 0;
	}

	if (info.nSpawnType == CEnemyManager::SPAWNTYPE_NORMAL)
	{
		int n = 0;
	}

	if (info.nSpawnType != CEnemyManager::SPAWNTYPE_CIRCLE &&
		info.nSpawnType != CEnemyManager::SPAWNTYPE_CLOCK &&
		info.nSpawnType != CEnemyManager::SPAWNTYPE_BOTHCLOCK && nNumEnemy == 0)
	{
		// 衝撃波生成
		CImpactWave::Create
		(
			D3DXVECTOR3(m_aWave[nCntWave].sSetWave.pos.x, m_aWave[nCntWave].sSetWave.pos.y + 50.0f, m_aWave[nCntWave].sSetWave.pos.z),	// 位置
			D3DXVECTOR3(0.0f, 0.0f, 0.0f),				// 向き
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.3f),			// 色
			100.0f,										// 幅
			10.0f,										// 高さ
			80,											// 寿命
			40.0f,										// 幅の移動量
			CImpactWave::TYPE_GREEN,					// テクスチャタイプ
			false										// 加算合成するか
		);
	}

	if (m_bSpawnByFrame == false &&
		(CManager::GetMode() == CScene::MODE_GAME || CManager::GetMode() == CScene::MODE_TUTORIAL))
	{
		// サウンド再生
		CManager::GetSound()->PlaySound(CSound::LABEL_SE_SPAWNEN);

		// サウンド再生
		CManager::GetSound()->PlaySound(CSound::LABEL_SE_SPAWN);
		m_bSpawnByFrame = true;
	}

	// 敵配置
	CGame::GetEnemyManager()->SetEnemy(m_aWave[nCntWave].nMyIdx, info.nType, nNumEnemy, m_aWave[nCntWave].nParentIdx, info, m_aWave[nCntWave].sSetWave.pos, m_aWave[nCntWave].sSetWave.rot, info.nMoveType);
}

//==========================================================================
// 描画処理
//==========================================================================
void CElevation::Draw(void)
{
	//  デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	if (CManager::IsWireframe() == true)
	{
		pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);	// ワイヤーフレームモード
	}

	// 描画処理
	CObject3DMesh::Draw();

	pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);	// 埋めるモード
}

//==========================================================================
// 高さ取得
//==========================================================================
float CElevation::GetHeight(D3DXVECTOR3& pos, D3DXVECTOR3& move, bool &bLand, CObject::TYPE type)
{
	// ベクトルと法線
	D3DXVECTOR3 vec1, vec2, nor;
	float fHeight = 0.0f;
	D3DXVECTOR3 *pVtxPos = GetVtxPos();

#if 0
	//for (int nCntHeight = 0; nCntHeight < m_aInfo.nHeightBlock; nCntHeight++)
	//{// 縦の分割分繰り返す

	//	for (int nCntWidth = 0; nCntWidth < m_aInfo.nWidthBlock; nCntWidth++)
	//	{// 横の分割分繰り返す

	//		if (bLand == true)
	//		{
	//			break;
	//		}

	//		// 今回の頂点
	//		int nNowPoint = (nCntWidth + 1) + (nCntHeight * (m_aInfo.nWidthBlock + 1));
	//		int nVerTexW = (m_aInfo.nWidthBlock + 1) + 1;

	//		int nLeft	= nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1));
	//		int nRight	= nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1)) + nVerTexW;

	//		if (CollisionTriangle(pVtxPos[nNowPoint], pVtxPos[nLeft], pVtxPos[nRight], pos, pos) == true)
	//		{// 三角に入っていたら

	//			// ベクトルを計算
	//			vec1 = pVtxPos[nRight] - pVtxPos[nNowPoint];
	//			vec2 = pVtxPos[nLeft] - pVtxPos[nNowPoint];

	//			// 外積を求める
	//			D3DXVec3Cross(&nor, &vec1, &vec2);

	//			// 外積の正規化をして法線にする
	//			D3DXVec3Normalize(&nor, &nor);

	//			if (nor.y != 0.0f)
	//			{// 法線が0.0fじゃなかったら

	//				// 高さを求める
	//				fHeight = ((pos.x - pVtxPos[nNowPoint].x) * nor.x + (pos.z - pVtxPos[nNowPoint].z) * nor.z + (pVtxPos[nNowPoint].y * -nor.y)) / -nor.y;
	//				fHeight += GetPosition().y;
	//				bLand = true;
	//				break;
	//			}
	//		}

	//		if (CollisionTriangle(pVtxPos[nNowPoint + m_aInfo.nWidthBlock], pVtxPos[nRight], pVtxPos[nLeft], pos, pos) == true)
	//		{// 三角に入っていたら

	//			// ベクトルを計算
	//			vec1 = pVtxPos[nLeft] - pVtxPos[nNowPoint + m_aInfo.nWidthBlock];
	//			vec2 = pVtxPos[nRight] - pVtxPos[nNowPoint + m_aInfo.nWidthBlock];

	//			// 外積を求める
	//			D3DXVec3Cross(&nor, &vec1, &vec2);

	//			// 外積の正規化をして法線にする
	//			D3DXVec3Normalize(&nor, &nor);

	//			if (nor.y != 0.0f)
	//			{// 法線が0.0fじゃなかったら

	//				// 高さを求める
	//				fHeight = ((pos.x - pVtxPos[nNowPoint + m_aInfo.nWidthBlock].x) * nor.x + (pos.z - pVtxPos[nNowPoint + m_aInfo.nWidthBlock].z) * nor.z + (pVtxPos[nNowPoint + m_aInfo.nWidthBlock].y * -nor.y)) / -nor.y;
	//				fHeight += GetPosition().y;
	//				bLand = true;
	//				break;
	//			}
	//		}
	//	}
	//}
#endif
	// フィールドの位置
	D3DXVECTOR3 posfield = GetPosition();

	// 長さ取得
	float fWidthLen = GetWidthLen();
	float fHeightLen = GetHeightLen();

	// 最大の長さ
	float fMaxWidthLen = fWidthLen * GetWidthBlock();
	float fMaxHeightLen = -fWidthLen * GetHeightBlock();

	// 判定する頂点の計算
	int nCntWidth = 0;
	int nCntHeight = 0;
	int nWidthPoint = ((pos.x - posfield.x) - fMaxWidthLen * 0.5f) / fWidthLen + GetWidthBlock();
	int nHeightPoint = GetHeightBlock() - ((pos.z - posfield.z) - fMaxHeightLen * 0.5f) / fHeightLen;

	for (int nCntH = 0; nCntH < 2; nCntH++)
	{
		for (int nCntW = 0; nCntW < 2; nCntW++)
		{
			// 横頂点
			nCntWidth = nWidthPoint + (nCntW - 1);
			nCntHeight = nHeightPoint + (nCntH - 1);

			if (nCntWidth < 0 || m_aInfo.nWidthBlock < nCntWidth)
			{// 範囲外で先頭
				continue;
			}

			if (nCntHeight < 0 || m_aInfo.nHeightBlock < nCntHeight)
			{// 範囲外で先頭
				continue;
			}

			// 今回の頂点
			int nNowPoint = (nCntWidth + 1) + (nCntHeight * (m_aInfo.nWidthBlock + 1));
			int nVerTexW = (m_aInfo.nWidthBlock + 1) + 1;

			int nLeft = nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1));
			int nRight = nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1)) + nVerTexW;

			if (nLeft >= GetNumVertex() || nRight >= GetNumVertex())
			{
				continue;
			}

			if (CollisionTriangle(pVtxPos[nNowPoint] + posfield, pVtxPos[nLeft] + posfield, pVtxPos[nRight] + posfield, pos, pos) == true)
			{// 三角に入っていたら

				// ベクトルを計算
				vec1 = pVtxPos[nRight] - pVtxPos[nNowPoint];
				vec2 = pVtxPos[nLeft] - pVtxPos[nNowPoint];

				// 外積を求める
				D3DXVec3Cross(&nor, &vec1, &vec2);

				// 外積の正規化をして法線にする
				D3DXVec3Normalize(&nor, &nor);

				if (nor.y != 0.0f)
				{// 法線が0.0fじゃなかったら

					// 高さを求める
					fHeight = ((pos.x - pVtxPos[nNowPoint].x - posfield.x) * nor.x + (pos.z - pVtxPos[nNowPoint].z - posfield.z) * nor.z + (pVtxPos[nNowPoint].y * -nor.y)) / -nor.y;
					fHeight += GetPosition().y;
					bLand = true;


					if (nor.y < 0.0f)
					{
						int n = 0;
					}
					/*if (nor.y <= 0.5f && nor.y >= -0.5f && fHeight > pos.y && (type == CObject::TYPE_PLAYER || type == CObject::TYPE_ENEMY))
					{
						pos.x -= move.x;
						pos.z -= move.z;
						move.x = 0.0f;
						move.z = 0.0f;
						int n = 0;
					}*/
				}
				break;
			}

			if (CollisionTriangle(pVtxPos[nNowPoint + GetWidthBlock()] + posfield, pVtxPos[nRight] + posfield, pVtxPos[nLeft] + posfield, pos, pos) == true)
			{// 三角に入っていたら

				// ベクトルを計算
				vec1 = pVtxPos[nLeft] - pVtxPos[nNowPoint + m_aInfo.nWidthBlock];
				vec2 = pVtxPos[nRight] - pVtxPos[nNowPoint + m_aInfo.nWidthBlock];

				// 外積を求める
				D3DXVec3Cross(&nor, &vec1, &vec2);

				// 外積の正規化をして法線にする
				D3DXVec3Normalize(&nor, &nor);

				if (nor.y != 0.0f)
				{// 法線が0.0fじゃなかったら

					// 高さを求める
					fHeight = ((pos.x - pVtxPos[nNowPoint + GetWidthBlock()].x - posfield.x) * nor.x + (pos.z - pVtxPos[nNowPoint + GetWidthBlock()].z - posfield.z) * nor.z + (pVtxPos[nNowPoint + GetWidthBlock()].y * -nor.y)) / -nor.y;
					fHeight += GetPosition().y;
					bLand = true;

					if (nor.y < 0.0f)
					{
						int n = 0;
					}


					/*if (nor.y <= 0.5f && nor.y >= -0.5f && fHeight >= pos.y && (type == CObject::TYPE_PLAYER || type == CObject::TYPE_ENEMY))
					{
						pos.x -= move.x;
						pos.z -= move.z;
						move.x = 0.0f;
						move.z = 0.0f;
						int n = 0;
					}*/
					break;
				}
			}

		}
	}

	if (bLand == true)
	{// 着地していたら

		return fHeight;
	}

	// 高さを取得
	return pos.y;
}

//==========================================================================
// 地面に当たったか
//==========================================================================
bool CElevation::IsHit(D3DXVECTOR3& pos)
{
	bool bHit = false;

	// ベクトルと法線
	D3DXVECTOR3 vec1 = D3DXVECTOR3(0.0f, 0.0f, 0.0f), vec2 = D3DXVECTOR3(0.0f, 0.0f, 0.0f), nor = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	float fHeight = pos.y;
	D3DXVECTOR3 *pVtxPos = GetVtxPos();
	D3DXVECTOR3 posfield = GetPosition();

	// 長さ取得
	float fWidthLen = GetWidthLen();
	float fHeightLen = GetHeightLen();

	// 最大の長さ
	float fMaxWidthLen = fWidthLen * m_aInfo.nWidthBlock + posfield.x;
	float fMaxHeightLen = -fWidthLen * m_aInfo.nHeightBlock + posfield.z;

	// 頂点決め打ち
	int nCntWidth = 0;
	int nCntHeight = 0;
	int nWidthPoint = (int)((pos.x - fMaxWidthLen * 0.5f) / fWidthLen) + m_aInfo.nWidthBlock;
	int nHeightPoint = m_aInfo.nHeightBlock - (int)((pos.z - fMaxHeightLen * 0.5f) / fHeightLen);

	for (int nCntH = 0; nCntH < 2; nCntH++)
	{
		for (int nCntW = 0; nCntW < 2; nCntW++)
		{
			// 横頂点
			nCntWidth = nWidthPoint + (nCntW - 1);
			nCntHeight = nHeightPoint + (nCntH - 1);

			if (nCntWidth < 0 || m_aInfo.nWidthBlock < nCntWidth)
			{// 範囲外で先頭
				continue;
			}

			if (nCntHeight < 0 || m_aInfo.nHeightBlock < nCntHeight)
			{// 範囲外で先頭
				continue;
			}

			// 今回の頂点
			int nNowPoint = (nCntWidth + 1) + (nCntHeight * (m_aInfo.nWidthBlock + 1));
			int nVerTexW = (m_aInfo.nWidthBlock + 1) + 1;

			int nLeft = nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1));
			int nRight = nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1)) + nVerTexW;

			if (CollisionTriangle(pVtxPos[nNowPoint], pVtxPos[nLeft], pVtxPos[nRight], pos, pos) == true)
			{// 三角に入っていたら

				// ベクトルを計算
				vec1 = pVtxPos[nRight] - pVtxPos[nNowPoint];
				vec2 = pVtxPos[nLeft] - pVtxPos[nNowPoint];

				// 外積を求める
				D3DXVec3Cross(&nor, &vec1, &vec2);

				// 外積の正規化をして法線にする
				D3DXVec3Normalize(&nor, &nor);

				if (nor.y != 0.0f)
				{// 法線が0.0fじゃなかったら

					// 高さを求める
					fHeight = ((pos.x - pVtxPos[nNowPoint].x) * nor.x + (pos.z - pVtxPos[nNowPoint].z) * nor.z + (pVtxPos[nNowPoint].y * -nor.y)) / -nor.y;
					fHeight += GetPosition().y;

					/*if (nor.y <= 0.5f && nor.y >= -0.5f && fHeight >= pos.y)
					{
						pos.x -= move.x;
						pos.z -= move.z;
						move.x = 0.0f;
						move.z = 0.0f;
						int n = 0;
					}*/
					break;
				}
			}

			if (CollisionTriangle(pVtxPos[nNowPoint + m_aInfo.nWidthBlock], pVtxPos[nRight], pVtxPos[nLeft], pos, pos) == true)
			{// 三角に入っていたら

				// ベクトルを計算
				vec1 = pVtxPos[nLeft] - pVtxPos[nNowPoint + m_aInfo.nWidthBlock];
				vec2 = pVtxPos[nRight] - pVtxPos[nNowPoint + m_aInfo.nWidthBlock];

				// 外積を求める
				D3DXVec3Cross(&nor, &vec1, &vec2);

				// 外積の正規化をして法線にする
				D3DXVec3Normalize(&nor, &nor);

				if (nor.y != 0.0f)
				{// 法線が0.0fじゃなかったら

					// 高さを求める
					fHeight = ((pos.x - pVtxPos[nNowPoint + m_aInfo.nWidthBlock].x) * nor.x + (pos.z - pVtxPos[nNowPoint + m_aInfo.nWidthBlock].z) * nor.z + (pVtxPos[nNowPoint + m_aInfo.nWidthBlock].y * -nor.y)) / -nor.y;
					fHeight += GetPosition().y;
					break;
				}
			}
		}
	}

	if (fHeight > pos.y)
	{// 前回の位置よりも下だったら(当たっていたら)
		bHit = true;
		pos.y = fHeight;
	}

	// 判定を返す
	return bHit;
}

//==========================================================================
// スライドの判定
//==========================================================================
float CElevation::Slide(D3DXVECTOR3& pos)
{

	float fVecY = 1.0f;

	// ベクトルと法線
	D3DXVECTOR3 vec1, vec2, nor;
	float fHeight = pos.y;
	D3DXVECTOR3 *pVtxPos = GetVtxPos();
	D3DXVECTOR3 posfield = GetPosition();

	// 長さ取得
	float fWidthLen = GetWidthLen();
	float fHeightLen = GetHeightLen();

	// 最大の長さ
	float fMaxWidthLen = fWidthLen * m_aInfo.nWidthBlock + posfield.x;
	float fMaxHeightLen = -fWidthLen * m_aInfo.nHeightBlock + posfield.z;

	// 頂点決め打ち
	int nCntWidth = 0;
	int nCntHeight = 0;
	int nWidthPoint = (int)((pos.x - fMaxWidthLen * 0.5f) / fWidthLen) + m_aInfo.nWidthBlock;
	int nHeightPoint = m_aInfo.nHeightBlock - (int)((pos.z - fMaxHeightLen * 0.5f) / fHeightLen);

	for (int nCntH = 0; nCntH < 2; nCntH++)
	{
		for (int nCntW = 0; nCntW < 2; nCntW++)
		{
			// 横頂点
			nCntWidth = nWidthPoint + (nCntW - 1);
			nCntHeight = nHeightPoint + (nCntH - 1);

			if (nCntWidth < 0 || m_aInfo.nWidthBlock < nCntWidth)
			{// 範囲外で先頭
				continue;
			}

			if (nCntHeight < 0 || m_aInfo.nHeightBlock < nCntHeight)
			{// 範囲外で先頭
				continue;
			}

			// 今回の頂点
			int nNowPoint = (nCntWidth + 1) + (nCntHeight * (m_aInfo.nWidthBlock + 1));
			int nVerTexW = (m_aInfo.nWidthBlock + 1) + 1;

			int nLeft = nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1));
			int nRight = nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1)) + nVerTexW;

			if (CollisionTriangle(pVtxPos[nNowPoint], pVtxPos[nLeft], pVtxPos[nRight], pos, pos) == true)
			{// 三角に入っていたら

				// ベクトルを計算
				vec1 = pVtxPos[nRight] - pVtxPos[nNowPoint];
				vec2 = pVtxPos[nLeft] - pVtxPos[nNowPoint];

				// 外積を求める
				D3DXVec3Cross(&nor, &vec1, &vec2);

				// 外積の正規化をして法線にする
				D3DXVec3Normalize(&nor, &nor);

				if (nor.y != 0.0f)
				{// 法線が0.0fじゃなかったら

					// 高さを求める
					fHeight = ((pos.x - pVtxPos[nNowPoint].x) * nor.x + (pos.z - pVtxPos[nNowPoint].z) * nor.z + (pVtxPos[nNowPoint].y * -nor.y)) / -nor.y;
					fHeight += GetPosition().y;

					if (fHeight >= pos.y)
					{// 地形の方がたかい(登ってる)
						fVecY = nor.y - 1.0f;
					}
					else
					{// 下ってる
						fVecY = 1.0f - nor.y + 0.25f;
					}

					/*if (nor.y <= 0.5f && nor.y >= -0.5f && fHeight >= pos.y)
					{
						pos.x -= move.x;
						pos.z -= move.z;
						move.x = 0.0f;
						move.z = 0.0f;
						int n = 0;
					}*/
					break;
				}
			}

			if (CollisionTriangle(pVtxPos[nNowPoint + m_aInfo.nWidthBlock], pVtxPos[nRight], pVtxPos[nLeft], pos, pos) == true)
			{// 三角に入っていたら

				// ベクトルを計算
				vec1 = pVtxPos[nLeft] - pVtxPos[nNowPoint + m_aInfo.nWidthBlock];
				vec2 = pVtxPos[nRight] - pVtxPos[nNowPoint + m_aInfo.nWidthBlock];

				// 外積を求める
				D3DXVec3Cross(&nor, &vec1, &vec2);

				// 外積の正規化をして法線にする
				D3DXVec3Normalize(&nor, &nor);

				if (nor.y != 0.0f)
				{// 法線が0.0fじゃなかったら

					// 高さを求める
					fHeight = ((pos.x - pVtxPos[nNowPoint + m_aInfo.nWidthBlock].x) * nor.x + (pos.z - pVtxPos[nNowPoint + m_aInfo.nWidthBlock].z) * nor.z + (pVtxPos[nNowPoint + m_aInfo.nWidthBlock].y * -nor.y)) / -nor.y;
					fHeight += GetPosition().y;

					if (fHeight >= pos.y)
					{// 地形の方がたかい(登ってる)
						fVecY = nor.y - 1.0f;
					}
					else
					{// 下ってる
						fVecY = 1.0f - nor.y + 0.25f;
					}
					break;
				}
			}
		}
	}

	// 判定を返す
	return fVecY;
}

//==========================================================================
// 頂点上げ下げ
//==========================================================================
void CElevation::UPVtxField(D3DXVECTOR3 pos)
{

	float m_fWidthLen = GetWidthLen();
	float m_fHeightLen = GetHeightLen();
	D3DXVECTOR3 *pVtxPos = GetVtxPos();

	// デバッグ表示
	CManager::GetDebugProc()->Print(
		"------------------[ 起伏エディット情報 ]------------------\n"
		"ブラシのサイズ：[1, 2] 【%f】\n"
		"ブラシの強さ：  [3, 4] 【%f】\n"
		"頂点上げ下げ：  [5, 6]\n"
		"幅拡縮：        [7, 8] 【%f, %f】\n"
		"まっ平：        [ ９ ]\n"
		"頂点リセット：  [delete]\n"
		"起伏情報保存：  [F9]\n\n", m_fBrushRange, m_fBrushStrength, m_fWidthLen, m_fHeightLen);

	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();

	if (pInputKeyboard->GetTrigger(DIK_F9) == true)
	{// セーブ
		Save();
	}

	// ブラシのサイズ
	if (pInputKeyboard->GetPress(DIK_1))
	{// 1で下げる
		m_fBrushRange -= 1.0f;
	}

	if (pInputKeyboard->GetPress(DIK_2))
	{// 2で上げる
		m_fBrushRange += 1.0f;
	}

	// ブラシの強さ
	if (pInputKeyboard->GetPress(DIK_3))
	{// 3で下げる
		m_fBrushStrength -= 0.01f;
	}

	if (pInputKeyboard->GetPress(DIK_4))
	{// 4で上げる
		m_fBrushStrength += 0.01f;
	}

	// フィールドの幅
	if (pInputKeyboard->GetPress(DIK_7))
	{// 7で上げる
		m_fWidthLen -= MOVE_LEN;
		m_fHeightLen -= MOVE_LEN;
	}

	if (pInputKeyboard->GetPress(DIK_8))
	{// 8で下げる
		m_fWidthLen += MOVE_LEN;
		m_fHeightLen += MOVE_LEN;
	}

	// 値の正規化
	ValueNormalize(m_fWidthLen, 99999999.0f, 10.0f);
	ValueNormalize(m_fHeightLen, 99999999.0f, 10.0f);

	// 値の正規化
	ValueNormalize(m_fBrushStrength, 99999999.0f, 0.1f);
	ValueNormalize(m_fBrushRange, 99999999.0f, 1.0f);

	// 頂点情報の設定
	for (int nCntHeight = 0; nCntHeight < m_aInfo.nHeightBlock + 1; nCntHeight++)
	{//縦の頂点数分繰り返す

		for (int nCntWidth = 0; nCntWidth < m_aInfo.nWidthBlock + 1; nCntWidth++)
		{// 横の頂点数分繰り返す

			if (pInputKeyboard->GetTrigger(DIK_DELETE))
			{// deleteでリセット
				for (int nCntWave = 0; nCntWave < mylib_const::MAX_WAVE; nCntWave++)
				{
					m_aWave[nCntWave].VtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y = 0.0f;
				}
				m_aInfo.pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y = 0.0f;
			}

			float fNowLength =
				sqrtf((pos.x - pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].x) * (pos.x - pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].x)
					+ (pos.z - pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].z) * (pos.z - pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].z));

			float Wariai = fNowLength / m_fBrushRange;

			if (Wariai >= 1.0f)
			{// 割合が1.0f以下だけ
				continue;
			}

			D3DXVECTOR3 move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

			if (pInputKeyboard->GetPress(DIK_5))
			{// 5で下げる
				move.y = -m_fBrushStrength - (-m_fBrushStrength * Wariai);
			}

			if (pInputKeyboard->GetPress(DIK_6))
			{// 6で上げる
				move.y = m_fBrushStrength - (m_fBrushStrength * Wariai);
			}

			if (pInputKeyboard->GetTrigger(DIK_9))
			{// 9でまっ平
				m_aInfo.pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y = pos.y;
				move.y = 0.0f;
			}

			m_aInfo.pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y += move.y;
			// 各頂点座標確認
			//pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y += move.y;
		}
	}

	// 頂点設定
	SetVtxPos(pVtxPos);
	SetWidthLen(m_fWidthLen);
	SetHeightLen(m_fHeightLen);

	if (m_pTargetP != NULL)
	{
		// 長さ変更
		m_pTargetP->SetWidthLen(m_fBrushRange);

		// カメラの情報取得
		CCamera *pCamera = CManager::GetCamera();

		// カメラの向き取得
		D3DXVECTOR3 Camerarot = pCamera->GetRotation();

		if (pInputKeyboard->GetPress(DIK_LEFT) == true)
		{// ←キーが押された,左移動

			if (pInputKeyboard->GetPress(DIK_UP) == true)
			{// A+W,左上移動

				pos.x += sinf(-D3DX_PI * MOVE_LRDW + Camerarot.y) * MOVE;
				pos.z += cosf(-D3DX_PI * MOVE_LRDW + Camerarot.y) * MOVE;
			}
			else if (pInputKeyboard->GetPress(DIK_DOWN) == true)
			{// A+S,左下移動

				pos.x += sinf(-D3DX_PI * MOVE_LRUP + Camerarot.y) * MOVE;
				pos.z += cosf(-D3DX_PI * MOVE_LRUP + Camerarot.y) * MOVE;
			}
			else
			{// A,左移動

				pos.x += sinf(-D3DX_PI * MOVE_LR + Camerarot.y) * MOVE;
				pos.z += cosf(-D3DX_PI * MOVE_LR + Camerarot.y) * MOVE;
			}
		}
		else if (pInputKeyboard->GetPress(DIK_RIGHT) == true)
		{// Dキーが押された,右移動

			if (pInputKeyboard->GetPress(DIK_UP) == true)
			{// D+W,右上移動

				pos.x += sinf(D3DX_PI * MOVE_LRDW + Camerarot.y) * MOVE;
				pos.z += cosf(D3DX_PI * MOVE_LRDW + Camerarot.y) * MOVE;
			}
			else if (pInputKeyboard->GetPress(DIK_DOWN) == true)
			{// D+S,右下移動

				pos.x += sinf(D3DX_PI * MOVE_LRUP + Camerarot.y) * MOVE;
				pos.z += cosf(D3DX_PI * MOVE_LRUP + Camerarot.y) * MOVE;
			}
			else
			{// D,右移動

				pos.x += sinf(D3DX_PI * MOVE_LR + Camerarot.y) * MOVE;
				pos.z += cosf(D3DX_PI * MOVE_LR + Camerarot.y) * MOVE;
			}
		}
		else if (pInputKeyboard->GetPress(DIK_UP) == true)
		{// Wが押された、奥移動

			pos.x += sinf(Camerarot.y) * MOVE;
			pos.z += cosf(Camerarot.y) * MOVE;
		}
		else if (pInputKeyboard->GetPress(DIK_DOWN) == true)
		{// Sが押された、手前移動

			pos.x += sinf(D3DX_PI + Camerarot.y) * MOVE;
			pos.z += cosf(D3DX_PI + Camerarot.y) * MOVE;
		}

		// 位置設定
		m_pTargetP->SetPosition(pos);
	}

}

//==========================================================================
// ダメージ処理
//==========================================================================
void CElevation::Damage(D3DXVECTOR3 pos, float fRange, float fStrength, DAMAGE damage)
{
	int nCntWave = 0;
	for (nCntWave = 0; nCntWave < mylib_const::MAX_WAVE; nCntWave++)
	{
		if (m_aWave[nCntWave].state == STATE_NONE)
		{
			m_aWave[nCntWave].sSetWave.pos = pos;
			m_aWave[nCntWave].sSetWave.fStrength = 0.0f;
			m_aWave[nCntWave].sSetWave.fStrengthOrigin = 0.0f;
			m_aWave[nCntWave].nCntState = 30;
			m_aWave[nCntWave].state = STATE_WAVE;
			m_aWave[nCntWave].nOriginCntState = 30;	// 状態遷移カウンター
			break;
		}
	}

	D3DXVECTOR3 *pVtxPos = GetVtxPos();

	// 頂点情報の設定
	for (int nCntHeight = 0; nCntHeight < m_aInfo.nHeightBlock + 1; nCntHeight++)
	{//縦の頂点数分繰り返す

		for (int nCntWidth = 0; nCntWidth < m_aInfo.nWidthBlock + 1; nCntWidth++)
		{// 横の頂点数分繰り返す

			float fNowLength = 
				sqrtf((pos.x - pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].x) * (pos.x - pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].x)
				+ (pos.z - pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].z) * (pos.z - pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].z));

			// 割合計算
			float Wariai = fNowLength / fRange;

			if (Wariai >= 1.0f)
			{// 割合が1.0f以下だけ
				continue;
			}

			D3DXVECTOR3 move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

			// 頂点を移動させる
			switch (damage)
			{
			case DAMAGE_RATIO:
				move.y = fStrength - (fStrength * Wariai);
				break;

			case DAMAGE_EQUAL:
				move.y = fStrength;
				break;
			}

			// 各頂点座標確認
			//pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y += move.y;

			// 波の高さ設定
			m_aWave[nCntWave].VtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y = move.y;

			//if (pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y <= MIN_HEIGHT)
			//{// 最低を下回ったら

			//	// 最低で固定
			//	pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y = MIN_HEIGHT;
			//}
			int n = 0;
		}
	}

	// 頂点設定
	SetVtxPos(pVtxPos);

}

//==========================================================================
// 波処理
//==========================================================================
void CElevation::SetWave(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXVECTOR2 fRange, float fRatio, float fStrength, float fWave, int nCntState, WAVETYPE type, TYPE playertype, int nLevel)
{
	for (int nCntWave = 0; nCntWave < mylib_const::MAX_WAVE; nCntWave++)
	{
		if (m_aWave[nCntWave].state == STATE_NONE)
		{
			m_aWave[nCntWave].sSetWave.pos = pos;
			m_aWave[nCntWave].sSetWave.posOrigin = pos;
			m_aWave[nCntWave].sSetWave.rot = rot;
			m_aWave[nCntWave].sSetWave.move = move;
			m_aWave[nCntWave].sSetWave.fRange = fRange;
			m_aWave[nCntWave].sSetWave.fRatio = fRatio;
			m_aWave[nCntWave].sSetWave.fStrength = fStrength;
			m_aWave[nCntWave].sSetWave.fStrengthOrigin = fStrength;
			m_aWave[nCntWave].sSetWave.type = type;
			m_aWave[nCntWave].sSetWave.playertype = playertype;
			m_aWave[nCntWave].sSetWave.fWave = fWave;
			m_aWave[nCntWave].sSetWave.fLength = m_aWave[nCntWave].sSetWave.fRatio * 1.5f;
			m_aWave[nCntWave].sSetWave.nLevel = nLevel;
			m_aWave[nCntWave].nMoveLife = nCntState;				// 移動寿命
			m_aWave[nCntWave].nOriginMoveLife = nCntState;				// 移動寿命
			m_aWave[nCntWave].nCntState = nCntState;			// 状態遷移カウンター
			m_aWave[nCntWave].nOriginCntState = nCntState;	// 状態遷移カウンター

			if (type == WAVETYPE_IMPACT && (playertype == TYPE_PLAYER || playertype == TYPE_ENEMY))
			{
				CManager::GetSound()->PlaySound(CSound::LABEL_SE_FIELD);
			}

			if (playertype != TYPE_NATURAL)
			{// 自然波の時
				m_aWave[nCntWave].nCntState += 180;			// 状態遷移カウンター
				m_aWave[nCntWave].nOriginCntState += 180;	// 状態遷移カウンター
			}

			if (playertype == TYPE_SPAWN)
			{// スポーンの起伏の時
				m_aWave[nCntWave].nCntState = nCntState * 2 + SPAWN_WAITTIME;		// 状態遷移カウンター
				m_aWave[nCntWave].nOriginCntState = nCntState * 2 + SPAWN_WAITTIME;	// 状態遷移カウンター
			}

			m_aWave[nCntWave].state = STATE_WAVE;
			m_aWave[nCntWave].fMaxWaveHeight = 0.0f;		// 波の最大高さ

			memset(&m_aWave[nCntWave].fWave[0], 0, sizeof(float) * 5600);
			break;
		}
	}
}

//==========================================================================
// スポーン波の設定
//==========================================================================
void CElevation::SetSpawnWave(int nIdx, int nParentID, int nNumEnemy, int nWaitTime, CWaveManager::SetInfo info, D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR2 Range, float fStrength, int nCntState)
{
	// カメラ設定の判定オン
	m_bSetSpawnCamera = true;

	for (int nCntWave = 0; nCntWave < mylib_const::MAX_WAVE; nCntWave++)
	{
		if (m_aWave[nCntWave].state != STATE_NONE)
		{// 何かしらの状態はスキップ
			continue;
		}

		m_aWave[nCntWave].WaveManagerInfo = info;						// ウェーブマネージャの情報
		m_aWave[nCntWave].nMyIdx = nIdx;								// 自分のインデックス番号
		m_aWave[nCntWave].nParentIdx = nParentID;						// 親のインデックス番号
		m_aWave[nCntWave].nNumEnemy = nNumEnemy;						// 何番目の敵か
		m_aWave[nCntWave].nWaitTime = nWaitTime;						// 待機時間
		m_aWave[nCntWave].sSetWave.pos = pos;							// 位置
		m_aWave[nCntWave].sSetWave.posOrigin = pos;						// 元の位置
		m_aWave[nCntWave].sSetWave.rot = rot;							// 向き
		m_aWave[nCntWave].sSetWave.move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);// 移動量
		m_aWave[nCntWave].sSetWave.fRange = Range;						// 範囲
		m_aWave[nCntWave].sSetWave.fRatio = 0.0f;						// 間隔
		m_aWave[nCntWave].sSetWave.fStrength = fStrength;				// 高さ
		m_aWave[nCntWave].sSetWave.fStrengthOrigin = fStrength;			// 元の高さ
		m_aWave[nCntWave].sSetWave.type = WAVETYPE_RAISE;				// 波の種類
		m_aWave[nCntWave].sSetWave.playertype = TYPE_SPAWN;				// 出現者の種類
		m_aWave[nCntWave].sSetWave.fWave = 0.0f;						// 波の広がり
		m_aWave[nCntWave].sSetWave.fLength = 0.0f;						// 中心点からの長さ
		m_aWave[nCntWave].sSetWave.nLevel = 0;							// レベル
		m_aWave[nCntWave].nMoveLife = nCntState;						// 移動寿命
		m_aWave[nCntWave].nOriginMoveLife = nCntState;					// 移動寿命
		m_aWave[nCntWave].nCntState = nCntState * 2 + SPAWN_WAITTIME;		// 状態遷移カウンター
		m_aWave[nCntWave].nOriginCntState = nCntState * 2 + SPAWN_WAITTIME;	// 状態遷移カウンター
		m_aWave[nCntWave].state = STATE_WAVE;								// 状態
		m_aWave[nCntWave].fMaxWaveHeight = 0.0f;							// 波の最大高さ

		// 高さリセット
		memset(&m_aWave[nCntWave].fWave[0], 0, sizeof(float) * 5600);
		break;
	}
}

//==========================================================================
// 波処理
//==========================================================================
void CElevation::Wave(int nCntWave)
{
	if (m_aWave[nCntWave].nMoveLife > 0)
	{// 寿命があるうち
		// 原点移動
		m_aWave[nCntWave].sSetWave.pos += m_aWave[nCntWave].sSetWave.move;
	}

#if 1
	// フィールドの情報
	D3DXVECTOR3 posfield = GetPosition();
	D3DXVECTOR3 pos = m_aWave[nCntWave].sSetWave.pos;
	D3DXVECTOR3 rot = m_aWave[nCntWave].sSetWave.rot;
	D3DXVECTOR2 Range = m_aWave[nCntWave].sSetWave.fRange;
	float fLength = sqrtf(Range.x * Range.x + Range.y * Range.y);	// 対角線の長さ
	float fAngle = atan2f(Range.x, Range.y);					// 対角線の向き

	// 長さ取得
	float fWidthLen = GetWidthLen();
	float fHeightLen = GetHeightLen();

	// 最大の長さ
	float fMaxWidthLen = fWidthLen * m_aInfo.nWidthBlock + posfield.x;
	float fMaxHeightLen = -fWidthLen * m_aInfo.nHeightBlock + posfield.z;

	// 頂点決め打ち
	int nWidthPoint = (int)((m_aWave[nCntWave].sSetWave.pos.x - fMaxWidthLen * 0.5f) / fWidthLen) + m_aInfo.nWidthBlock;
	int nHeightPoint = m_aInfo.nHeightBlock - (int)((m_aWave[nCntWave].sSetWave.pos.z - fMaxHeightLen * 0.5f) / fHeightLen);

	// 判定する範囲
	float fLeftLen = 0.0f;
	if (m_aWave[nCntWave].sSetWave.type == WAVETYPE_IMPACT)
	{// 円形波の時
		fLeftLen = m_aWave[nCntWave].sSetWave.fLength * 2.0f + m_aWave[nCntWave].sSetWave.fRatio;
	}
	else
	{// 円形波以外
		if (Range.x >= Range.y)
		{
			fLeftLen = Range.x * 2.0f;
		}
		else
		{
			fLeftLen = Range.y * 2.0f;
		}
	}

	// 周りを見る範囲
	int nNumVtx = (int)(fLeftLen / 63.0f);
	nNumVtx += 1;

	// 範囲外のものは全て0
	memset(&m_aWave[nCntWave].VtxPos[0], 0, sizeof(D3DXVECTOR3) * mylib_const::MAX_VTX);

	int nCntWidth = 0;
	int nCntHeight = 0;
	for (int nCntH = 0; nCntH < nNumVtx; nCntH++)
	{
		for (int nCntW = 0; nCntW < nNumVtx; nCntW++)
		{
			// 横頂点
			nCntWidth = -(nNumVtx / 2) + nWidthPoint + nCntW;
			nCntHeight = -(nNumVtx / 2) + nHeightPoint + nCntH;
			/*nCntWidth = nWidthPoint + (nCntW - 1);
			nCntHeight = nHeightPoint + (nCntH - 1);*/

			if (nCntWidth < 0 || m_aInfo.nWidthBlock < nCntWidth)
			{// 範囲外で先頭
				continue;
			}

			if (nCntHeight < 0 || m_aInfo.nHeightBlock < nCntHeight)
			{// 範囲外で先頭
				continue;
			}

			switch (m_aWave[nCntWave].sSetWave.type)
			{
			case WAVETYPE_RIPPLE:	// 波紋
				UpdateRippleWave(nCntWave, nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1)));
				break;

			case WAVETYPE_DIRECT:	// 直行

				if (nCntH == 0 && nCntW == 0 &&
					m_aWave[nCntWave].nMoveLife > 0 &&
					m_aWave[nCntWave].sSetWave.playertype != TYPE_NATURAL)
				{// 寿命があるうち && 自然の波以外全て

					// 地面音再生
					if (m_aWave[nCntWave].nMoveLife >= 50 && m_aWave[nCntWave].nMoveLife % 6 == 0)
					{
						CManager::GetSound()->PlaySound(CSound::LABEL_SE_FIELD);
					}

					// 衝撃波生成
					if (m_aWave[nCntWave].nMoveLife % 12 == 0)
					{
						// 出現位置
						D3DXVECTOR3 spawnPos = D3DXVECTOR3(pos.x + sinf(D3DX_PI + rot.y) * Range.y, m_aWave[nCntWave].fFrameMaxWaveHeight + 15.0f, pos.z + cosf(D3DX_PI + rot.y) * Range.y);

						// 衝撃波生成
						switch (m_aWave[nCntWave].sSetWave.playertype)
						{
						case TYPE_PLAYER:
							CImpactWave::Create
							(
								spawnPos,	// 位置
								D3DXVECTOR3(D3DX_PI * 0.5f, D3DX_PI + rot.y, 0.0f),				// 向き
								D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.2f),			// 色
								40.0f,										// 幅
								30.0f,										// 高さ
								36,											// 寿命
								10.0f,										// 幅の移動量
								CImpactWave::TYPE_BLACK2,					// テクスチャタイプ
								false										// 加算合成するか
							);
							break;

						case TYPE_ENEMY:
							CImpactWave::Create
							(
								spawnPos,	// 位置
								D3DXVECTOR3(D3DX_PI * 0.5f, D3DX_PI + rot.y, 0.0f),				// 向き
								D3DXCOLOR(1.0f, 0.0f, 1.0f, 0.3f),			// 色
								40.0f,										// 幅
								30.0f,										// 高さ
								36,											// 寿命
								10.0f,										// 幅の移動量
								CImpactWave::TYPE_PURPLE3,					// テクスチャタイプ
								false										// 加算合成するか
							);
							break;

						}
					}
#if MODE_SCREENSHOT
					// 瓦礫生成
					CBallast::Create(D3DXVECTOR3(pos.x + sinf(rot.y - fAngle) * fLength, pos.y + 15.0f, pos.z + cosf(rot.y - fAngle) * fLength), DEFAULTMOVE_BALLAST, 1);
					CBallast::Create(D3DXVECTOR3(pos.x + sinf(rot.y + fAngle) * fLength, pos.y + 15.0f, pos.z + cosf(rot.y + fAngle) * fLength), DEFAULTMOVE_BALLAST, 1);

					// 左上と右上の位置に煙
					my_particle::Create(D3DXVECTOR3(pos.x + Random(-200, 200), pos.y - 5.0f, pos.z + Random(-200, 200)), my_particle::TYPE_WAVESMOKE);
					my_particle::Create(D3DXVECTOR3(pos.x + Random(-200, 200), pos.y - 5.0f, pos.z + Random(-200, 200)), my_particle::TYPE_WAVESMOKE);

#else
					// 瓦礫生成
					CBallast::Create(D3DXVECTOR3(pos.x + sinf(rot.y - fAngle) * fLength, pos.y + 5.0f, pos.z + cosf(rot.y - fAngle) * fLength), 4);
					CBallast::Create(D3DXVECTOR3(pos.x + sinf(rot.y + fAngle) * fLength, pos.y + 5.0f, pos.z + cosf(rot.y + fAngle) * fLength), 4);

					// 左上と右上の位置に煙
					my_particle::Create(D3DXVECTOR3(pos.x + Random(-200, 200), pos.y + 5.0f, pos.z + Random(-200, 200)), my_particle::TYPE_WAVESMOKE);
					my_particle::Create(D3DXVECTOR3(pos.x + Random(-200, 200), pos.y + 5.0f, pos.z + Random(-200, 200)), my_particle::TYPE_WAVESMOKE);
					my_particle::Create(D3DXVECTOR3(pos.x + Random(-200, 200), pos.y + 5.0f, pos.z + Random(-200, 200)), my_particle::TYPE_WAVESMOKE);
					my_particle::Create(D3DXVECTOR3(pos.x + Random(-200, 200), pos.y + 5.0f, pos.z + Random(-200, 200)), my_particle::TYPE_WAVESMOKE);
#endif

				}

				// フレーム毎の最大高さリセット
				m_aWave[nCntWave].fFrameMaxWaveHeight = 0;

				// 直線波の更新処理
				UpdateDirectWave(nCntWave, nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1)));
				break;

			case WAVETYPE_IMPACT:	// 衝撃波

				if (nCntH == 0 && nCntW == 0 && m_aWave[nCntWave].nMoveLife > 0)
				{// 寿命があるうち

					// 地面音再生
					/*if (m_aWave[nCntWave].nMoveLife % 4 == 0)
					{
						CManager::GetSound()->PlaySound(CSound::LABEL_SE_FIELD);
					}*/

					// 広がらせる
					m_aWave[nCntWave].sSetWave.fLength += m_aWave[nCntWave].sSetWave.fWave;

					for (int nCnt = 0; nCnt < 16; nCnt++)
					{
						// 衝撃波と敵との向き
						float fRot = (D3DX_PI * 2.0f) / 16.0f;
						fRot *= nCnt;

						D3DXVECTOR3 pos = m_aWave[nCntWave].sSetWave.pos;

						// 敵の位置を押し出す
						D3DXVECTOR3 particlePos;
						particlePos.x = pos.x + sinf(D3DX_PI + fRot) * (m_aWave[nCntWave].sSetWave.fLength + m_aWave[nCntWave].sSetWave.fRatio + m_aWave[nCntWave].sSetWave.fRatio);
						particlePos.z = pos.z + cosf(D3DX_PI + fRot) * (m_aWave[nCntWave].sSetWave.fLength + m_aWave[nCntWave].sSetWave.fRatio + m_aWave[nCntWave].sSetWave.fRatio);

						// 左上と右上の位置に煙
						if (m_aWave[nCntWave].sSetWave.playertype == TYPE_PLAYER)
						{
							my_particle::Create(D3DXVECTOR3(particlePos.x + Random(-50, 50), pos.y - 5.0f, particlePos.z + Random(-50, 50)), my_particle::TYPE_IMPACTWAVESMOKE);
						}
					}
				}

				UpdateImpactWave(nCntWave, nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1)));
				break;

			case WAVETYPE_THRUST:	// 突き上げ
				UpdateThrustWave(nCntWave, nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1)));
				break;

			case WAVETYPE_RAISE:	// 盛り上げ
				UpdateRaiseField(nCntWave, nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1)));
				break;
			}
		}
	}

#else


	for (int nCntHeight = 0; nCntHeight < m_aInfo.nHeightBlock; nCntHeight++)
	{// 縦の分割分繰り返す

		for (int nCntWidth = 0; nCntWidth < m_aInfo.nWidthBlock; nCntWidth++)
		{// 横の分割分繰り返す

			switch (m_aWave[nCntWave].sSetWave.type)
			{
			case WAVETYPE_RIPPLE:	// 波紋
				UpdateRippleWave(nCntWave, nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1)));
				break;

			case WAVETYPE_DIRECT:	// 直行
				UpdateDirectWave(nCntWave, nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1)));
				break;

			case WAVETYPE_IMPACT:	// 衝撃波
				UpdateImpactWave(nCntWave, nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1)));
				break;

			case WAVETYPE_THRUST:	// 突き上げ
				UpdateThrustWave(nCntWave, nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1)));
				break;

			case WAVETYPE_RAISE:	// 盛り上げ
				UpdateRaiseField(nCntWave, nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1)));
				break;
			}

		}
	}
#endif
}

//==========================================================================
// 波紋更新処理
//==========================================================================
void CElevation::UpdateRippleWave(int nCntWave, int nVtxPoint)
{
	// 頂点座標取得
	D3DXVECTOR3 *pVtxPos = GetVtxPos();

	// 原点と頂点の距離
	float fNowLength =
		sqrtf((m_aWave[nCntWave].sSetWave.pos.x - pVtxPos[nVtxPoint].x) * (m_aWave[nCntWave].sSetWave.pos.x - pVtxPos[nVtxPoint].x)
			+ (m_aWave[nCntWave].sSetWave.pos.z - pVtxPos[nVtxPoint].z) * (m_aWave[nCntWave].sSetWave.pos.z - pVtxPos[nVtxPoint].z));

	// 割合
	float Wariai = fNowLength / m_aWave[nCntWave].sSetWave.fRatio;
	if (m_aWave[nCntWave].sSetWave.fRatio == 0.0f)
	{
		return;
	}

	float WaveWariai = fNowLength / m_aWave[nCntWave].sSetWave.fRange.x;

	// 波の高さ
	float fWaveStrength;
	fWaveStrength = -m_aWave[nCntWave].sSetWave.fStrength - (-m_aWave[nCntWave].sSetWave.fStrength * WaveWariai);

	if (WaveWariai >= 1.0f)
	{// 範囲外は移動量ゼロ
		fWaveStrength = 0.0f;
	}

	// 波の向き
	float fRot = D3DX_PI * Wariai;

	// ウェーブさせる
	m_aWave[nCntWave].fWave[nVtxPoint] -= m_aWave[nCntWave].sSetWave.fWave;

	// 角度の正規化
	RotNormalize(fRot);
	RotNormalize(m_aWave[nCntWave].fWave[nVtxPoint]);

	// 波の高さ設定
	m_aWave[nCntWave].VtxPos[nVtxPoint].y =
		sinf(fRot + m_aWave[nCntWave].fWave[nVtxPoint]) * fWaveStrength;

	// 高さリセット
	m_VtxPos[nVtxPoint].y += m_aWave[nCntWave].VtxPos[nVtxPoint].y;
}

//==========================================================================
// 直行波更新処理
//==========================================================================
void CElevation::UpdateDirectWave(int nCntWave, int nVtxPoint)
{

	// 頂点座標取得
	D3DXVECTOR3 *pVtxPos = GetVtxPos();

	// 頂点カラー取得
	D3DXCOLOR *pVtxCol = GetVtxCol();

	// 見づらいから計算用に代入
	D3DXVECTOR2 fRange = m_aWave[nCntWave].sSetWave.fRange;
	D3DXVECTOR3 pos = m_aWave[nCntWave].sSetWave.pos;
	D3DXVECTOR3 rot = m_aWave[nCntWave].sSetWave.rot;

	// 線分になる2点
	D3DXVECTOR2 Start = D3DXVECTOR2(pos.x + sinf(-D3DX_PI * 0.5f + rot.y) * fRange.x, pos.z + cosf(-D3DX_PI * 0.5f + rot.y) * fRange.x);
	D3DXVECTOR2 End = D3DXVECTOR2(pos.x + sinf(D3DX_PI * 0.5f + rot.y) * fRange.x, pos.z + cosf(D3DX_PI * 0.5f + rot.y) * fRange.x);

	// 判定する頂点
	D3DXVECTOR2 Point = D3DXVECTOR2(pVtxPos[nVtxPoint].x, pVtxPos[nVtxPoint].z);

	// 線分
	D3DXVECTOR2 Line = D3DXVECTOR2(End.x - Start.x, End.y - Start.y);

	// 頂点と線分との最短距離
	float fNowLength = Line.x * (Start.y - Point.y) - Line.y * (Start.x - Point.x);
	fNowLength = sqrt((fNowLength * fNowLength) / ((Line.x * Line.x) + (Line.y * Line.y)));

	// 割合
	float WaveWariai = fNowLength / m_aWave[nCntWave].sSetWave.fRange.y;

	// 波の高さ
	float fWaveStrength;
	fWaveStrength = -m_aWave[nCntWave].sSetWave.fStrength - (-m_aWave[nCntWave].sSetWave.fStrength * WaveWariai);

	// 高さの判定
	if (fWaveStrength <= 0.0f)
	{
		fWaveStrength = 0.0f;

		// デフォルト色に戻す
		//if (pVtxCol[nVtxPoint] != mylib_const::FIELDCOLOR_01)
		{
			pVtxCol[nVtxPoint] = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		}
		return;
	}

	float fLength = sqrtf(fRange.x * fRange.x + fRange.y * fRange.y);	// 対角線の長さ
	float fAngle = atan2f(fRange.x, fRange.y);					// 対角線の向き

	// 四角の判定
	if (CollisionSquare(pos, fRange, rot.y, pVtxPos[nVtxPoint]))
	{
		// 前は削る
		float fff = -fLength * 0.5f;
		if (CollisionSquare
		(
			D3DXVECTOR3(pos.x + sinf(D3DX_PI + rot.y) * fff + sinf(rot.y - fAngle) * fLength, pos.y, pos.z + cosf(D3DX_PI + rot.y) * fff + cosf(rot.y - fAngle) * fLength),	// 左上
			D3DXVECTOR3(pos.x + sinf(D3DX_PI + rot.y) * fff + sinf(rot.y + fAngle) * fLength, pos.y, pos.z + cosf(D3DX_PI + rot.y) * fff + cosf(rot.y + fAngle) * fLength),		// 右上
			D3DXVECTOR3(pos.x + sinf(D3DX_PI + rot.y) * fff + sinf(rot.y - D3DX_PI + fAngle) * fLength, pos.y, pos.z + cosf(D3DX_PI + rot.y) * fff + cosf(rot.y - D3DX_PI + fAngle) * fLength),	// 左下
			D3DXVECTOR3(pos.x + sinf(D3DX_PI + rot.y) * fff + sinf(rot.y + D3DX_PI - fAngle) * fLength, pos.y, pos.z + cosf(D3DX_PI + rot.y) * fff + cosf(rot.y + D3DX_PI - fAngle) * fLength),		// 右下
			pVtxPos[nVtxPoint]											// 判定する位置
		))
		{

		}
		else
		{
			fWaveStrength = 0.0f;

			// デフォルト色に戻す
			//if (pVtxCol[nVtxPoint] != mylib_const::FIELDCOLOR_01)
			{
				pVtxCol[nVtxPoint] = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			}
			return;
		}

	}
	else
	{
		// 高さを0
		fWaveStrength = 0.0f;

		// デフォルト色に戻す
		//if (pVtxCol[nVtxPoint] != mylib_const::FIELDCOLOR_01)
		{
			pVtxCol[nVtxPoint] = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		}
		return;
	}

	// 波の高さ設定
	m_aWave[nCntWave].VtxPos[nVtxPoint].y = fWaveStrength;
	
	// 最大値入れ替え
	if (m_aWave[nCntWave].fMaxWaveHeight < m_aWave[nCntWave].VtxPos[nVtxPoint].y)
	{// 今回の方が高かったら
		m_aWave[nCntWave].fMaxWaveHeight = m_aWave[nCntWave].VtxPos[nVtxPoint].y + m_aInfo.pVtxPos[nVtxPoint].y;
	}

	if (m_aWave[nCntWave].fFrameMaxWaveHeight < m_aWave[nCntWave].VtxPos[nVtxPoint].y)
	{
		m_aWave[nCntWave].fFrameMaxWaveHeight = m_aWave[nCntWave].VtxPos[nVtxPoint].y + m_aInfo.pVtxPos[nVtxPoint].y;
	}

	// 地面色に設定
	pVtxCol[nVtxPoint] = mylib_const::FIELDCOLOR_01;

	// 高さリセット
	m_VtxPos[nVtxPoint].y += m_aWave[nCntWave].VtxPos[nVtxPoint].y;
}

//==========================================================================
// 衝撃波更新処理
//==========================================================================
void CElevation::UpdateImpactWave(int nCntWave, int nVtxPoint)
{
	// 頂点座標取得
	D3DXVECTOR3 *pVtxPos = GetVtxPos();

	// 頂点カラー取得
	D3DXCOLOR *pVtxCol = GetVtxCol();

	// 見づらいから計算用に代入
	D3DXVECTOR2 fRange = m_aWave[nCntWave].sSetWave.fRange;
	D3DXVECTOR3 pos = m_aWave[nCntWave].sSetWave.pos;
	D3DXVECTOR3 rot = m_aWave[nCntWave].sSetWave.rot;
	float fRatio = m_aWave[nCntWave].sSetWave.fRatio;
	float fLength = m_aWave[nCntWave].sSetWave.fLength;

	// 原点と頂点の距離
	float fNowLength =
		sqrtf((pos.x - pVtxPos[nVtxPoint].x) * (pos.x - pVtxPos[nVtxPoint].x)
			+ (pos.z - pVtxPos[nVtxPoint].z) * (pos.z - pVtxPos[nVtxPoint].z));

	// 原点と最大範囲までの距離
	float fMaxLength = fLength + fRatio;

	// 原点と最小範囲までの距離
	float fMinLength = fLength - fRatio;

	// 範囲の長さ
	float fRangeLength = fMaxLength - fMinLength;

	// 判定時の長さ
	float fNowCollisionLen = fNowLength - fLength;

	// 割合
	float Wariai = fNowLength / fRatio;
	float WaveWariai = fNowCollisionLen / fRangeLength;

	if (WaveWariai < 0.0f)
	{// 範囲外は逆転
		WaveWariai *= -1;
	}

	// 波の高さ
	float fWaveStrength;
	fWaveStrength = -m_aWave[nCntWave].sSetWave.fStrength - (-m_aWave[nCntWave].sSetWave.fStrength * WaveWariai);

	if (fNowLength > fMaxLength || fNowLength < fMinLength)
	{// 範囲外は移動量ゼロ
		fWaveStrength = 0.0f;

		// デフォルト色に戻す
		//if (pVtxCol[nVtxPoint] != mylib_const::FIELDCOLOR_01)
		{
			pVtxCol[nVtxPoint] = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		}
		return;
	}

	// 波の高さ設定
	m_aWave[nCntWave].VtxPos[nVtxPoint].y = fWaveStrength;

	// 最大値入れ替え
	if (m_aWave[nCntWave].fMaxWaveHeight < m_aWave[nCntWave].VtxPos[nVtxPoint].y)
	{// 今回の方が高かったら
		m_aWave[nCntWave].fMaxWaveHeight = m_aWave[nCntWave].VtxPos[nVtxPoint].y + m_aInfo.pVtxPos[nVtxPoint].y;
	}

	// 地面色に設定
	pVtxCol[nVtxPoint] = mylib_const::FIELDCOLOR_01;

	// 高さリセット
	m_VtxPos[nVtxPoint].y += m_aWave[nCntWave].VtxPos[nVtxPoint].y;
}

//==========================================================================
// 突き上げ更新処理
//==========================================================================
void CElevation::UpdateThrustWave(int nCntWave, int nVtxPoint)
{
	// 頂点座標取得
	D3DXVECTOR3 *pVtxPos = GetVtxPos();

	// 原点と頂点の距離
	float fNowLength =
		sqrtf((m_aWave[nCntWave].sSetWave.pos.x - pVtxPos[nVtxPoint].x) * (m_aWave[nCntWave].sSetWave.pos.x - pVtxPos[nVtxPoint].x)
			+ (m_aWave[nCntWave].sSetWave.pos.z - pVtxPos[nVtxPoint].z) * (m_aWave[nCntWave].sSetWave.pos.z - pVtxPos[nVtxPoint].z));

	// 割合
	float Wariai = fNowLength / m_aWave[nCntWave].sSetWave.fRatio;
	float WaveWariai = fNowLength / m_aWave[nCntWave].sSetWave.fRange.x;

	// 波の高さ
	float fWaveStrength;
	fWaveStrength = m_aWave[nCntWave].sSetWave.fStrength;

	if (WaveWariai >= 1.0f)
	{// 範囲外は移動量ゼロ
		fWaveStrength = 0.0f;
	}

	// 波の向き
	float fRot = D3DX_PI * Wariai;

	// ウェーブさせる
	m_aWave[nCntWave].fWave[nVtxPoint] += (fWaveStrength - m_aWave[nCntWave].fWave[nVtxPoint]) * 0.2f;

	// 角度の正規化
	RotNormalize(fRot);
	RotNormalize(m_aWave[nCntWave].fWave[nVtxPoint]);

	// 波の高さ設定
	m_aWave[nCntWave].VtxPos[nVtxPoint].y = m_aWave[nCntWave].fWave[nVtxPoint];

	// 高さリセット
	m_VtxPos[nVtxPoint].y += m_aWave[nCntWave].VtxPos[nVtxPoint].y;
}

//==========================================================================
// 盛り上げ更新処理
//==========================================================================
void CElevation::UpdateRaiseField(int nCntWave, int nVtxPoint)
{
	// 頂点座標取得
	D3DXVECTOR3 *pVtxPos = GetVtxPos();

	// 頂点カラー取得
	D3DXCOLOR *pVtxCol = GetVtxCol();

	// 原点と頂点の距離
	float fNowLength =
		sqrtf((m_aWave[nCntWave].sSetWave.pos.x - pVtxPos[nVtxPoint].x) * (m_aWave[nCntWave].sSetWave.pos.x - pVtxPos[nVtxPoint].x)
			+ (m_aWave[nCntWave].sSetWave.pos.z - pVtxPos[nVtxPoint].z) * (m_aWave[nCntWave].sSetWave.pos.z - pVtxPos[nVtxPoint].z));

	// 割合
	float Wariai = fNowLength / m_aWave[nCntWave].sSetWave.fRatio;
	float WaveWariai = fNowLength / m_aWave[nCntWave].sSetWave.fRange.x;

	// 波の高さ
	float fWaveStrength = m_aWave[nCntWave].sSetWave.fStrength;

	if (WaveWariai >= 1.0f)
	{// 割合が1.0f以下だけ
		m_aWave[nCntWave].VtxPos[nVtxPoint].y = 0.0f;

		// デフォルト色に戻す
		if (pVtxCol[nVtxPoint] != mylib_const::FIELDCOLOR_01)
		{
			pVtxCol[nVtxPoint] = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		}
		return;
	}

	// 高さを求める
	fWaveStrength = fWaveStrength - (fWaveStrength * WaveWariai);

	// 波の高さ設定
	m_aWave[nCntWave].VtxPos[nVtxPoint].y = fWaveStrength;

	// 地面色に設定
	pVtxCol[nVtxPoint] = mylib_const::FIELDCOLOR_01;

	// 高さリセット
	m_VtxPos[nVtxPoint].y += m_aWave[nCntWave].VtxPos[nVtxPoint].y;
}

//==========================================================================
// 当たり判定処理
//==========================================================================
void CElevation::Collision(int nCntWave)
{
	// 見づらいから一時保存
	D3DXVECTOR2 fRange = m_aWave[nCntWave].sSetWave.fRange;
	D3DXVECTOR3 pos = m_aWave[nCntWave].sSetWave.pos;
	D3DXVECTOR3 move = m_aWave[nCntWave].sSetWave.move;
	D3DXVECTOR3 rot = m_aWave[nCntWave].sSetWave.rot;

	// 先頭を保存
	CObject *pObj = CObject::GetTop(mylib_const::ENEMY_PRIORITY);

	if (m_aWave[nCntWave].sSetWave.playertype != TYPE_NATURAL)
	{// 自然の波以外全て

		while (pObj != NULL)
		{// NULLが来るまで無限ループ

			// 次のオブジェクトを一時保存
			CObject *pObjNext = pObj->GetNext();

			// 死亡の判定
			if (pObj->IsDeath() == true)
			{// 死亡フラグが立っていたら

				// 次のオブジェクトを代入
				pObj = pObjNext;
				continue;
			}

			// 種類の取得
			switch (m_aWave[nCntWave].sSetWave.playertype)
			{
			case TYPE_PLAYER:

				if (pObj->GetType() != CObject::TYPE_ENEMY)
				{// 敵じゃなかったら

					if (pObj->GetType() != CObject::TYPE_BOSS)
					{// ボスじゃなかったら

						// 次のオブジェクトを代入
						pObj = pObjNext;
						continue;
					}
				}
				break;

			case TYPE_ENEMY:

				if (pObj->GetType() != CObject::TYPE_PLAYER)
				{// プレイヤーじゃなかったら

					// 次のオブジェクトを代入
					pObj = pObjNext;
					continue;
				}
				break;

			case TYPE_NATURAL:

				return;
				break;

			case TYPE_EFFECT:
				return;
				break;

			case TYPE_SPAWN:
				return;
				break;
			}

			// 敵の位置取得
			D3DXVECTOR3 TargetPos = pObj->GetPosition();
			D3DXVECTOR3 TargetPosOld = pObj->GetOldPosition();

			// 判定サイズ取得
			float fRadius = pObj->GetObjectChara()->GetRadius();

			// 波に当たってない状態に設定する
			pObj->GetObjectChara()->SetDisableWaveHit();

			switch (m_aWave[nCntWave].sSetWave.type)
			{
			case WAVETYPE_RIPPLE:	// 波紋
				break;

			case WAVETYPE_DIRECT:	// 直行
				CollisionDirectWave(nCntWave, pObj);
				break;

			case WAVETYPE_IMPACT:	// 衝撃波
				CollisionImpactWave(nCntWave, pObj);
				break;
			}

			// 次のオブジェクトを代入
			pObj = pObjNext;
		}

		if (m_aWave[nCntWave].sSetWave.type == TYPE_EFFECT)
		{
			int n = 0;
			return;
		}

		// 終わった後に波同士の確認
		switch (m_aWave[nCntWave].sSetWave.type)
		{
		case WAVETYPE_RIPPLE:	// 波紋
			break;

		case WAVETYPE_DIRECT:	// 直行
			for (int nCnt = 0; nCnt < mylib_const::MAX_WAVE; nCnt++)
			{
				if (m_aWave[nCnt].state != STATE_WAVE)
				{// 使われていなかったら
					continue;
				}

				if (m_aWave[nCnt].sSetWave.playertype == TYPE_EFFECT ||
					m_aWave[nCnt].sSetWave.playertype == TYPE_SPAWN)
				{// 判定なし波の場合
					continue;
				}

				if (nCnt == nCntWave)
				{// 自分と同じだったら
					continue;
				}

				// 敵の情報取得
				D3DXVECTOR3 TargetPos = m_aWave[nCnt].sSetWave.pos;
				D3DXVECTOR3 TargetPosOld = m_aWave[nCnt].sSetWave.pos - m_aWave[nCnt].sSetWave.move;
				D3DXVECTOR3 TargetRot = m_aWave[nCnt].sSetWave.rot;
				D3DXVECTOR2 TargetSize = m_aWave[nCnt].sSetWave.fRange;

				// 対角線の向き,長さ
				float fLength = sqrtf(fRange.x * fRange.x + fRange.y * fRange.y);
				float fAngle = atan2f(fRange.x, fRange.y);

				// 判定する四角
				D3DXVECTOR3 LeftUp = D3DXVECTOR3(pos.x + sinf(rot.y - fAngle) * fLength, pos.y, pos.z + cosf(rot.y - fAngle) * fLength);
				D3DXVECTOR3 RightUp = D3DXVECTOR3(pos.x + sinf(rot.y + fAngle) * fLength, pos.y, pos.z + cosf(rot.y + fAngle) * fLength);
				D3DXVECTOR3 LeftDown = D3DXVECTOR3(pos.x + sinf(rot.y - D3DX_PI + fAngle) * fLength, pos.y, pos.z + cosf(rot.y - D3DX_PI + fAngle) * fLength);
				D3DXVECTOR3 RightDown = D3DXVECTOR3(pos.x + sinf(rot.y + D3DX_PI - fAngle) * fLength, pos.y, pos.z + cosf(rot.y + D3DX_PI - fAngle) * fLength);

				// 左の頂点座標
				D3DXVECTOR3 pos0 = LeftDown;

				// 右の頂点座標
				D3DXVECTOR3 pos1 = RightDown;

				// 境界線のベクトル
				D3DXVECTOR3 vecLine;
				vecLine.x = pos1.x - pos0.x;
				vecLine.z = pos1.z - pos0.z;

				// プレイヤーの境界線のベクトル
				D3DXVECTOR3 vecLinePlayer;
				vecLinePlayer.x = TargetPos.x - TargetPosOld.x;
				vecLinePlayer.z = TargetPos.z - TargetPosOld.z;

				// プレイヤーと壁のベクトル
				D3DXVECTOR3 vecToPosPlayer;
				vecToPosPlayer.x = pos1.x - TargetPosOld.x;
				vecToPosPlayer.z = pos1.z - TargetPosOld.z;

				// 高さ保存
				float fStr = m_aWave[nCnt].sSetWave.fStrength;
				if (fStr > 0)
				{// 高さが逆だったら逆にする
					fStr *= -1;
				}

				bool bHit = false;	// 波同士の判定
				switch (m_aWave[nCnt].sSetWave.type)
				{
				case WAVETYPE_DIRECT:	// 直線
					// 矩形と矩形の判定
					bHit = CollisionSquareSquare2D(pos, TargetPos, fRange, TargetSize, rot.y, TargetRot.y);
					break;

				case WAVETYPE_IMPACT:	// 円
					// 円と矩形の判定
					bHit = CollisionCircleSquare2D(TargetPos, pos, rot, m_aWave[nCnt].sSetWave.fLength, fRange);
					break;

				case WAVETYPE_RAISE:	// 盛り上げ
					bHit = CollisionCircleSquare2D(TargetPos, pos, rot, m_aWave[nCnt].sSetWave.fRange.x, fRange);
					break;
				}

				if (bHit == false || m_aWave[nCntWave].sSetWave.fStrength >= fStr)
				{// 当たってない or 自分の高さが負けてる
					continue;
				}

				// 当たった相手の種類
				if (m_aWave[nCntWave].sSetWave.playertype != m_aWave[nCnt].sSetWave.playertype &&
					(m_aWave[nCnt].nMoveLife > 0 || m_aWave[nCnt].sSetWave.type == WAVETYPE_RAISE))
				{// 自分と同じやつじゃなかったら && 移動中

					if (m_aWave[nCntWave].sSetWave.playertype == TYPE_ENEMY && m_aWave[nCnt].sSetWave.playertype == TYPE_NATURAL)
					{//	敵と自然物ははじく
						continue;
					}

					switch (m_aWave[nCnt].sSetWave.type)
					{
					case WAVETYPE_DIRECT:	// 直線

						// 振動
						CManager::GetCamera()->SetShake(SHAKETIME_DIRECT, IMPACTSHAKE_DIRECT, 0.0f);

						// ヒットストップ有効
						if (m_aWave[nCntWave].sSetWave.playertype == TYPE_PLAYER && m_aWave[nCnt].sSetWave.playertype != TYPE_PLAYER)
						{
							m_bHitStop_Direct = true;
						}

						// 衝撃波生成
						CImpactWave::Create
						(
							D3DXVECTOR3(pos.x, pos.y + 50.0f, pos.z),	// 位置
							D3DXVECTOR3(D3DX_PI * 0.75f, 0.0f, 0.0f),	// 向き
							D3DXCOLOR(0.7f, 0.7f, 1.0f, 0.4f),			// 色
							140.0f,										// 幅
							1.0f,										// 高さ
							30,											// 寿命
							30.0f,										// 幅の移動量
							CImpactWave::TYPE_SMOKE,					// テクスチャタイプ
							true										// 加算合成するか
						);

						// 衝撃波生成
						CImpactWave::Create
						(
							D3DXVECTOR3(pos.x, pos.y + 50.0f, pos.z),	// 位置
							D3DXVECTOR3(0.0f, 0.0f, 0.0f),	// 向き
							D3DXCOLOR(0.7f, 0.7f, 1.0f, 0.4f),			// 色
							140.0f,										// 幅
							1.0f,										// 高さ
							30,											// 寿命
							30.0f,										// 幅の移動量
							CImpactWave::TYPE_SMOKE,					// テクスチャタイプ
							true										// 加算合成するか
						);

						// 衝撃波生成
						CImpactWave::Create
						(
							D3DXVECTOR3(pos.x, pos.y + 50.0f, pos.z),	// 位置
							D3DXVECTOR3(D3DX_PI * -0.75f, 0.0f, 0.0f),	// 向き
							D3DXCOLOR(0.7f, 0.7f, 1.0f, 0.4f),			// 色
							140.0f,										// 幅
							1.0f,										// 高さ
							30,											// 寿命
							30.0f,										// 幅の移動量
							CImpactWave::TYPE_SMOKE,					// テクスチャタイプ
							true										// 加算合成するか
						);
						break;

					case WAVETYPE_IMPACT:	// 円
						m_aWave[nCntWave].sSetWave.move.x *= 0.6f;
						m_aWave[nCntWave].sSetWave.move.z *= 0.6f;

						// 振動
						CManager::GetCamera()->SetShake(SHAKETIME_IMPACT, IMPACTSHAKE_IMPACT, 0.0f);

						// ヒットストップ有効
						if (m_aWave[nCntWave].sSetWave.playertype == TYPE_PLAYER && m_aWave[nCnt].sSetWave.playertype != TYPE_PLAYER)
						{
							m_bHitStop_Impact = true;
						}
						//CManager::SetEnableHitStop();

						// 衝撃波生成
						CImpactWave::Create
						(
							D3DXVECTOR3(pos.x, pos.y + 50.0f, pos.z),	// 位置
							D3DXVECTOR3(D3DX_PI * 0.75f, 0.0f, 0.0f),	// 向き
							D3DXCOLOR(0.7f, 0.7f, 1.0f, 0.4f),			// 色
							140.0f,										// 幅
							1.0f,										// 高さ
							30,											// 寿命
							30.0f,										// 幅の移動量
							CImpactWave::TYPE_SMOKE,					// テクスチャタイプ
							true										// 加算合成するか
						);

						// 衝撃波生成
						CImpactWave::Create
						(
							D3DXVECTOR3(pos.x, pos.y + 50.0f, pos.z),	// 位置
							D3DXVECTOR3(0.0f, 0.0f, 0.0f),	// 向き
							D3DXCOLOR(0.7f, 0.7f, 1.0f, 0.4f),			// 色
							140.0f,										// 幅
							1.0f,										// 高さ
							30,											// 寿命
							30.0f,										// 幅の移動量
							CImpactWave::TYPE_SMOKE,					// テクスチャタイプ
							true										// 加算合成するか
						);

						// 衝撃波生成
						CImpactWave::Create
						(
							D3DXVECTOR3(pos.x, pos.y + 50.0f, pos.z),	// 位置
							D3DXVECTOR3(D3DX_PI * -0.75f, 0.0f, 0.0f),	// 向き
							D3DXCOLOR(0.7f, 0.7f, 1.0f, 0.4f),			// 色
							140.0f,										// 幅
							1.0f,										// 高さ
							30,											// 寿命
							30.0f,										// 幅の移動量
							CImpactWave::TYPE_SMOKE,					// テクスチャタイプ
							true										// 加算合成するか
						);
						break;

					case WAVETYPE_RAISE:	// 盛り上げ

						// 振動
						CManager::GetCamera()->SetShake(SHAKETIME_IMPACT, IMPACTSHAKE_IMPACT, 0.0f);

						// ヒットストップ有効
						m_bHitStop_Impact = true;
						//CManager::SetEnableHitStop();

						// 衝撃波生成
						CImpactWave::Create
						(
							D3DXVECTOR3(pos.x, pos.y + 50.0f, pos.z),	// 位置
							D3DXVECTOR3(D3DX_PI * 0.75f, 0.0f, 0.0f),	// 向き
							D3DXCOLOR(0.7f, 0.7f, 1.0f, 0.4f),			// 色
							140.0f,										// 幅
							1.0f,										// 高さ
							30,											// 寿命
							30.0f,										// 幅の移動量
							CImpactWave::TYPE_SMOKE,					// テクスチャタイプ
							true										// 加算合成するか
						);

						// 衝撃波生成
						CImpactWave::Create
						(
							D3DXVECTOR3(pos.x, pos.y + 50.0f, pos.z),	// 位置
							D3DXVECTOR3(0.0f, 0.0f, 0.0f),	// 向き
							D3DXCOLOR(0.7f, 0.7f, 1.0f, 0.4f),			// 色
							140.0f,										// 幅
							1.0f,										// 高さ
							30,											// 寿命
							30.0f,										// 幅の移動量
							CImpactWave::TYPE_SMOKE,					// テクスチャタイプ
							true										// 加算合成するか
						);

						// 衝撃波生成
						CImpactWave::Create
						(
							D3DXVECTOR3(pos.x, pos.y + 50.0f, pos.z),	// 位置
							D3DXVECTOR3(D3DX_PI * -0.75f, 0.0f, 0.0f),	// 向き
							D3DXCOLOR(0.7f, 0.7f, 1.0f, 0.4f),			// 色
							140.0f,										// 幅
							1.0f,										// 高さ
							30,											// 寿命
							30.0f,										// 幅の移動量
							CImpactWave::TYPE_SMOKE,					// テクスチャタイプ
							true										// 加算合成するか
						);
						break;
					}


					switch (m_aWave[nCnt].sSetWave.playertype)
					{// 当たった相手のプレイヤー種類
					case TYPE_PLAYER:
						// 当たったやつを平坦にする
						memset(&m_aWave[nCnt].VtxPos[0], 0, sizeof(D3DXVECTOR3) * mylib_const::MAX_VTX);
						m_aWave[nCnt].state = STATE_NONE;
						break;

					case TYPE_ENEMY:
						// 当たったやつを平坦にする
						memset(&m_aWave[nCnt].VtxPos[0], 0, sizeof(D3DXVECTOR3) * mylib_const::MAX_VTX);
						m_aWave[nCnt].state = STATE_NONE;
						break;

					case TYPE_NATURAL:
						if (m_aWave[nCntWave].sSetWave.playertype != TYPE_PLAYER)
						{//	プレイヤーだけが使える
							continue;
						}

						// 当たったやつを平坦にする
						memset(&m_aWave[nCnt].VtxPos[0], 0, sizeof(D3DXVECTOR3) * mylib_const::MAX_VTX);
						m_aWave[nCnt].state = STATE_NONE;

						// 自分に当たったやつの要素を合成
						m_aWave[nCntWave].sSetWave.move += m_aWave[nCnt].sSetWave.move;
						m_aWave[nCntWave].sSetWave.fRange.x = m_aWave[nCnt].sSetWave.fRange.x;

						if (m_aWave[nCnt].sSetWave.fStrengthOrigin > 0)
						{// 高さが逆だったら逆にする
							m_aWave[nCnt].sSetWave.fStrengthOrigin *= -1;
						}

						// 元の高さに当たったやつの高さを足す
						m_aWave[nCntWave].sSetWave.fStrengthOrigin += m_aWave[nCnt].sSetWave.fStrengthOrigin;

						// 元の寿命に戻す
						m_aWave[nCntWave].nCntState = m_aWave[nCntWave].nOriginCntState;
						m_aWave[nCntWave].nMoveLife = m_aWave[nCntWave].nOriginMoveLife;
						break;
					}
				}
			}
			break;

		case WAVETYPE_IMPACT:
			break;

		}
	}
	else if (m_aWave[nCntWave].sSetWave.playertype == TYPE_NATURAL)
	{// 自然の波

	}
}

//==========================================================================
// 直線波の当たり判定処理
//==========================================================================
void CElevation::CollisionDirectWave(int nCntWave, CObject *pObj)
{
	// 見づらいから一時保存
	D3DXVECTOR2 fRange = m_aWave[nCntWave].sSetWave.fRange;
	D3DXVECTOR3 pos = m_aWave[nCntWave].sSetWave.pos;
	D3DXVECTOR3 move = m_aWave[nCntWave].sSetWave.move;
	D3DXVECTOR3 rot = m_aWave[nCntWave].sSetWave.rot;

	// 敵の位置取得
	D3DXVECTOR3 TargetPos = pObj->GetPosition();
	D3DXVECTOR3 TargetPosOld = pObj->GetOldPosition();
	D3DXVECTOR3 TargetRot = pObj->GetRotation();

	// 判定サイズ取得
	float fRadius = pObj->GetObjectChara()->GetRadius();

	if (TargetPos.y > m_aWave[nCntWave].fMaxWaveHeight)
	{// 波よりも上に居たら回避
		return;
	}

	float fLength = sqrtf(fRange.x * fRange.x + fRange.y * fRange.y);	// 対角線の長さ
	float fAngle = atan2f(fRange.x, fRange.y);							// 対角線の向き

	// 判定する四角
	D3DXVECTOR3 LeftUp = D3DXVECTOR3(pos.x + sinf(rot.y - fAngle) * fLength, pos.y, pos.z + cosf(rot.y - fAngle) * fLength);
	D3DXVECTOR3 RightUp = D3DXVECTOR3(pos.x + sinf(rot.y + fAngle) * fLength, pos.y, pos.z + cosf(rot.y + fAngle) * fLength);
	D3DXVECTOR3 LeftDown = D3DXVECTOR3(pos.x + sinf(rot.y - D3DX_PI + fAngle) * fLength, pos.y, pos.z + cosf(rot.y - D3DX_PI + fAngle) * fLength);
	D3DXVECTOR3 RightDown = D3DXVECTOR3(pos.x + sinf(rot.y + D3DX_PI - fAngle) * fLength, pos.y, pos.z + cosf(rot.y + D3DX_PI - fAngle) * fLength);

	// 左の頂点座標
	D3DXVECTOR3 pos0 = LeftDown;

	// 右の頂点座標
	D3DXVECTOR3 pos1 = RightDown;

	// 境界線のベクトル
	D3DXVECTOR3 vecLine;
	vecLine.x = pos1.x - pos0.x;
	vecLine.z = pos1.z - pos0.z;

	// プレイヤーの境界線のベクトル
	D3DXVECTOR3 vecLinePlayer;
	vecLinePlayer.x = TargetPos.x - TargetPosOld.x;
	vecLinePlayer.z = TargetPos.z - TargetPosOld.z;

	// プレイヤーと壁のベクトル
	D3DXVECTOR3 vecToPosPlayer;
	vecToPosPlayer.x = pos1.x - TargetPosOld.x;
	vecToPosPlayer.z = pos1.z - TargetPosOld.z;

	if (CollisionSquare(pos, fRange, rot.y, TargetPos) == false)
	{// 当たってない場合抜ける
		return;
	}

	// 波に当たっている状態に設定する
	pObj->GetObjectChara()->SetEnableWaveHit();

	if (pObj->GetType() == TYPE_BOSS && m_aWave[nCntWave].nCntState > 60)
	{
		m_aWave[nCntWave].nCntState = 60;
	}

	if (pObj->GetType() == TYPE_BOSS && pObj->Hit(0) == true)
	{// ボス && ノックバック中
		return;
	}

	if (pObj->GetType() == TYPE_BOSS && (pObj->GetObjectChara()->GetState() == CBoss::STATE_ATTACK || pObj->GetObjectChara()->GetState() == CBoss::STATE_KNOCKBACK))
	{
		return;
	}

	if (pObj->GetType() == CObject::TYPE_ENEMY && pObj->GetObjectChara()->GetState() == CEnemy::STATE_BURY)
	{// 敵が埋まっているとき
		return;
	}

	if (pObj->GetType() == CObject::TYPE_ENEMY && pObj->GetObjectChara()->GetState() == CEnemy::STATE_KNOCKBACK)
	{// 敵が埋まっているとき
		return;
	}

	// ヒット処理
	switch (m_aWave[nCntWave].sSetWave.playertype)
	{
	case TYPE_PLAYER:

		if (pObj->GetObjectChara()->GetState() != CEnemy::STATE_DMG &&
			pObj->GetObjectChara()->GetState() != CEnemy::STATE_KNOCKBACK &&
			pObj->GetObjectChara()->GetState() != CEnemy::STATE_DEAD &&
			pObj->GetObjectChara()->GetState() != CEnemy::STATE_SPAWN &&
			pObj->GetObjectChara()->GetState() != CEnemy::STATE_FADEOUT)
		{
			// 衝撃波と敵との向き
			float fRot = atan2f((pos.x - TargetPos.x), (pos.z - TargetPos.z));

			m_aWave[nCntWave].sSetWave.fWave;
			pObj->SetMove(D3DXVECTOR3(
				sinf(D3DX_PI + fRot) * 20.0f,
				15.0f,
				cosf(D3DX_PI + fRot) * 20.0f));

			// 向きを正面にする
			pObj->SetRotation(D3DXVECTOR3(TargetRot.x, D3DX_PI + rot.y, TargetRot.z));
		}

		if (pObj->Hit(mylib_const::DIRECTWAVE_DMG))
		{
			if (CManager::GetMode() == CScene::MODE_TUTORIAL)
			{// チュートリアルだったら
				CTutorial::GetStep()->SetStep(CTutorialStep::STEP_DIRECTWAVE);
			}
			break;
		}
		break;

	case TYPE_ENEMY:

		if (pObj->GetObjectChara()->GetState() != CPlayer::STATE_KNOCKBACK &&
			pObj->GetObjectChara()->GetState() != CPlayer::STATE_DEAD &&
			pObj->GetObjectChara()->GetState() != CPlayer::STATE_INVINCIBLE)
		{
			// 移動のベクトルを求める
			float fMove = sqrtf(move.x * move.x + move.z * move.z);
			fMove *= 0.6f;

			pObj->SetMove(D3DXVECTOR3(
				sinf(D3DX_PI + rot.y) * fMove,
				15.0f,
				cosf(D3DX_PI + rot.y) * fMove));

			// 向きを正面にする
			pObj->SetRotation(D3DXVECTOR3(TargetRot.x, D3DX_PI + rot.y, TargetRot.z));
		}

		if (pObj->Hit(mylib_const::DIRECTWAVE_DMG))
		{
			//my_particle::Create(TargetPos, my_particle::TYPE_OFFSETTING);
		}
		break;
	}

	// 面積の最大値
	float fMaxAreaPlayer = (vecLinePlayer.z * vecLine.x) - (vecLinePlayer.x * vecLine.z);
	if (fMaxAreaPlayer == 0.0f)
	{// 動いてないやつは抜ける
		return;
	}

	switch (m_aWave[nCntWave].sSetWave.playertype)
	{
	case TYPE_PLAYER:
		if (pObj->GetObjectChara()->GetState() == CEnemy::STATE_DMG ||
			pObj->GetObjectChara()->GetState() == CEnemy::STATE_KNOCKBACK ||
			pObj->GetObjectChara()->GetState() == CEnemy::STATE_DEAD ||
			pObj->GetObjectChara()->GetState() != CEnemy::STATE_SPAWN ||
			pObj->GetObjectChara()->GetState() == CEnemy::STATE_FADEOUT)
		{
			int n = 0;
			return;
		}
		break;

	case TYPE_ENEMY:
		if (pObj->GetObjectChara()->GetState() == CPlayer::STATE_DMG ||
			pObj->GetObjectChara()->GetState() == CPlayer::STATE_KNOCKBACK ||
			pObj->GetObjectChara()->GetState() == CPlayer::STATE_DEAD ||
			pObj->GetObjectChara()->GetState() == CPlayer::STATE_INVINCIBLE)
		{
			return;
		}
		break;
	}

	// 今回の面積
	float fNowAreaPlayer = (vecToPosPlayer.z * vecLine.x) - (vecToPosPlayer.x * vecLine.z);

	// 割合
	float fRatePlayer = fNowAreaPlayer / fMaxAreaPlayer;

	// 交点からはみ出た分
	D3DXVECTOR3 CollisionPointPlayer = D3DXVECTOR3(0.0f, TargetPos.y, 0.0f);
	CollisionPointPlayer.x = (TargetPos.x + (vecLinePlayer.x * (fRatePlayer - 1.0f)));
	CollisionPointPlayer.z = (TargetPos.z + (vecLinePlayer.z * (fRatePlayer - 1.0f)));

	// 法線ベクトル(境界線ベクトルのXとZ反転)
	D3DXVECTOR3 vecNor = D3DXVECTOR3(vecLine.z, 0.0f, -vecLine.x);

	// ベクトルの正規化
	D3DXVec3Normalize(&vecNor, &vecNor);

	// プレイヤーの逆移動量
	D3DXVECTOR3 PlayerInverceMove;
	PlayerInverceMove.z = vecLinePlayer.z * (fRatePlayer - 1.0f);
	PlayerInverceMove.x = vecLinePlayer.x * (fRatePlayer - 1.0f);

	// 内積(壁の法線とプレイヤーの逆移動量)
	float fDot = (PlayerInverceMove.x * vecNor.x) + (PlayerInverceMove.z * vecNor.z);

	// 壁ずり移動量
	D3DXVECTOR3 MoveWall = vecNor * fDot;

	// ぶつかった点に補正
	TargetPos += MoveWall + (vecNor * 0.1f);

	// 位置設定
	pObj->SetPosition(TargetPos);

	if (pObj->GetType() != TYPE_BOSS && pObj->GetObjectChara()->GetState() != CEnemy::STATE_DEAD)
	{// ボス && ノックバック中
		pObj->SetMove(D3DXVECTOR3(0.0f, pObj->GetMove().y, 0.0f));
	}
}

//==========================================================================
// 円形波の当たり判定処理
//==========================================================================
void CElevation::CollisionImpactWave(int nCntWave, CObject *pObj)
{
	// 見づらいから一時保存
	D3DXVECTOR2 fRange = m_aWave[nCntWave].sSetWave.fRange;
	D3DXVECTOR3 pos = m_aWave[nCntWave].sSetWave.pos;
	D3DXVECTOR3 move = m_aWave[nCntWave].sSetWave.move;
	D3DXVECTOR3 rot = m_aWave[nCntWave].sSetWave.rot;

	// 敵の位置取得
	D3DXVECTOR3 TargetPos = pObj->GetPosition();
	D3DXVECTOR3 TargetPosOld = pObj->GetOldPosition();
	D3DXVECTOR3 TargetRot = pObj->GetRotation();

	// 判定サイズ取得
	float fRadius = pObj->GetObjectChara()->GetRadius();

	if (TargetPos.y > m_aWave[nCntWave].fMaxWaveHeight)
	{// 波よりも上に居たら回避
		return;
	}

	// 最大円の判定
	bool bHit = CircleRange(pos, TargetPos, m_aWave[nCntWave].sSetWave.fLength - m_aWave[nCntWave].sSetWave.fRatio * 2.0f, fRadius);

	if (bHit == false)
	{// 当たっていなかったら
		return;
	}

	if (pObj->GetType() == TYPE_BOSS && pObj->GetObjectChara()->GetState() == CEnemy::STATE_KNOCKBACK)
	{// ノックバック中だったら
		return;
	}

	if (pObj->GetType() == TYPE_BOSS && pObj->GetObjectChara()->GetState() == CBoss::STATE_ATTACK)
	{
		return;
	}

	// 波に当たっている状態に設定する
	pObj->GetObjectChara()->SetEnableWaveHit();

	// 敵との距離
	float fLength =
		sqrtf((pos.x - TargetPos.x) * (pos.x - TargetPos.x)
			+ (pos.z - TargetPos.z) * (pos.z - TargetPos.z));

	// 衝撃波と敵との向き
	float fRot = atan2f((pos.x - TargetPos.x), (pos.z - TargetPos.z));

	// 敵の位置を押し出す
	TargetPos.x = pos.x + sinf(D3DX_PI + fRot) * (m_aWave[nCntWave].sSetWave.fLength + m_aWave[nCntWave].sSetWave.fRatio + m_aWave[nCntWave].sSetWave.fRatio);
	TargetPos.z = pos.z + cosf(D3DX_PI + fRot) * (m_aWave[nCntWave].sSetWave.fLength + m_aWave[nCntWave].sSetWave.fRatio + m_aWave[nCntWave].sSetWave.fRatio);

	if (pObj->GetType() == CObject::TYPE_ENEMY && pObj->GetObjectChara()->GetState() == CEnemy::STATE_BURY)
	{// 敵が埋まっているとき

		// 位置設定
		pObj->SetPosition(D3DXVECTOR3(TargetPos.x, TargetPos.y + 10.0f, TargetPos.z));

		pObj->SetMove(D3DXVECTOR3(
			sinf(D3DX_PI + fRot) * 15.0f,
			20.0f,
			cosf(D3DX_PI + fRot) * 15.0f));

		D3DXVECTOR3 MoveRot = pObj->GetRotation();
		pObj->SetRotation(D3DXVECTOR3(MoveRot.x, fRot, MoveRot.z));
	}
	else
	{
		switch (m_aWave[nCntWave].sSetWave.playertype)
		{
		case TYPE_PLAYER:
			if (pObj->GetObjectChara()->GetState() != CEnemy::STATE_DMG &&
				pObj->GetObjectChara()->GetState() != CEnemy::STATE_KNOCKBACK &&
				pObj->GetObjectChara()->GetState() != CEnemy::STATE_DEAD &&
				pObj->GetObjectChara()->GetState() != CEnemy::STATE_SPAWN &&
				pObj->GetObjectChara()->GetState() != CEnemy::STATE_FADEOUT)
			{
				// 位置設定
				pObj->SetPosition(D3DXVECTOR3(TargetPos.x, TargetPos.y + 5.0f, TargetPos.z));

				// 向きを正面にする
				pObj->SetRotation(D3DXVECTOR3(TargetRot.x, D3DX_PI + fRot, TargetRot.z));

				// ノックバック移動量
				pObj->SetMove(D3DXVECTOR3(
					sinf(D3DX_PI + fRot) * 10.0f,
					10.0f,
					cosf(D3DX_PI + fRot) * 10.0f));

				if (pObj->Hit(mylib_const::IMPACTWAVE_DMG))
				{
					if (CManager::GetMode() == CScene::MODE_TUTORIAL)
					{// チュートリアルだったら
						CTutorial::GetStep()->SetStep(CTutorialStep::STEP_IMPACTWAVE);
					}
					return;
				}
			}
			break;

		case TYPE_ENEMY:
			if (pObj->GetObjectChara()->GetState() != CPlayer::STATE_DMG &&
				pObj->GetObjectChara()->GetState() != CPlayer::STATE_KNOCKBACK &&
				pObj->GetObjectChara()->GetState() != CPlayer::STATE_DEAD &&
				pObj->GetObjectChara()->GetState() != CPlayer::STATE_INVINCIBLE)
			{
				// 位置設定
				pObj->SetPosition(D3DXVECTOR3(TargetPos.x, TargetPos.y + 5.0f, TargetPos.z));

				// 向きを正面にする
				pObj->SetRotation(D3DXVECTOR3(TargetRot.x, D3DX_PI + fRot, TargetRot.z));

				// ノックバック移動量
				pObj->SetMove(D3DXVECTOR3(
					sinf(D3DX_PI + fRot) * 4.0f,
					15.0f,
					cosf(D3DX_PI + fRot) * 4.0f));

				if (pObj->Hit(mylib_const::IMPACTWAVE_DMG))
				{
					return;
				}
			}
			break;

		default:
			break;
		}
	}

	//********************************************
	// ボスの処理
	//********************************************
	if (pObj->GetType() != TYPE_BOSS)
	{// ボス以外
		return;
	}

	// ボスの処理
	if (m_aWave[nCntWave].nCntState > 10)
	{
		m_aWave[nCntWave].nCntState = 10;
	}

	float fMove;
	switch (m_aWave[nCntWave].sSetWave.nLevel)
	{
	case CPowerGauge::LEVEL_1:
		fMove = 30.0f;
		break;

	case CPowerGauge::LEVEL_2:
		fMove = 50.0f;
		break;

	case CPowerGauge::LEVEL_3:
		fMove = 100.0f;
		break;

	case CPowerGauge::LEVEL_MAX:
		fMove = 150.0f;
		break;
	}
	move = D3DXVECTOR3(sinf(D3DX_PI + fRot) * fMove, pObj->GetMove().y, cosf(D3DX_PI + fRot) * fMove);

	// 移動量設定
	pObj->SetMove(move);

}

//==========================================================================
// ウェーブ毎の配置処理
//==========================================================================
void CElevation::SetWaveNaturalwave(int nStage, int nWave)
{
	// 敵拠点データ取得
	CEnemyBase::BaseData *pEnemyBase = CGame::GetEnemyBase()->GetBaseData(nStage);

	// 波配置
	for (int nCntWave = 0; nCntWave < m_aSetInfo[nStage].aWaveInfo[nWave].nSetNum; nCntWave++)
	{
		// 配置情報
		CWaveManager::SetInfo info = m_aSetInfo[nStage].aWaveInfo[nWave].Info[nCntWave];

		// 波発生
		switch (info.nType)
		{
		case 0:
			CGame::GetObject3DMesh()->SetWave
			(
				pEnemyBase[info.nBase].pos + info.pos,	// 位置
				info.rot,								// 向き
				D3DXVECTOR3(
					0.0f,
					0.0f,
					0.0f),								// 移動量
				D3DXVECTOR2(200.0f, 200.0f),			// 判定範囲
				0.0f,									// 波発生の頻度
				180.0f,									// 波の高さ
				0.0f,									// 波の広がり
				60,										// 発生時間
				CElevation::WAVETYPE_RAISE,				// 種類
				CElevation::TYPE_NATURAL,				// プレイヤーの種類
				0										// レベル
			);
			break;

		case 1:
			CGame::GetObject3DMesh()->SetWave
			(
				pEnemyBase[info.nBase].pos + info.pos,				// 位置
				info.rot,											// 向き
				D3DXVECTOR3(
					sinf(D3DX_PI + GetRotation().y) * 10.0f,
					0.0f,
					cosf(D3DX_PI + GetRotation().y) * 10.0f),		// 移動量
				D3DXVECTOR2(200.0f, 300.0f),						// 判定範囲
				CGame::GetObject3DMesh()->GetWidthLen() * 5.0f,		// 波発生の頻度
				-300.0f,											// 波の高さ
				D3DX_PI * 0.0f,										// 波打つ頻度
				150,												// 発生時間
				CElevation::WAVETYPE_DIRECT,						// 種類
				CElevation::TYPE_NATURAL,							// プレイヤーの種類
				CPowerGauge::LEVEL_1
			);
			break;
		}
	}
}

//==========================================================================
// 頂点情報設定処理
//==========================================================================
void CElevation::SetVtx(void)
{

	D3DXVECTOR3 *pVtxPos = GetVtxPos();
	D3DXVECTOR3 *pVtxNor = GetVtxNor();
	D3DXVECTOR3 vec1, vec2, nor;
	D3DXVECTOR3 VtxRight, VtxLeft, VtxNow;
	float fWidthLen = GetWidthLen();
	float fHeightLen = GetHeightLen();

	for (int nCntHeight = 0; nCntHeight < m_aInfo.nHeightBlock + 1; nCntHeight++)
	{// 縦の分割分繰り返す

		for (int nCntWidth = 0; nCntWidth < m_aInfo.nWidthBlock + 1; nCntWidth++)
		{// 横の分割分繰り返す

			m_VtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].x = pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].x;
			m_VtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].z = pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].z;

			//// 高さリセット
			//m_VtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y = 0.0f;
			//for (int nCntWave = 0; nCntWave < mylib_const::MAX_WAVE; nCntWave++)
			//{// 全ての波の高さを合成
			//	m_VtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y += m_aWave[nCntWave].VtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y;
			//}

			// 合計値を代入する
			pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))] = m_VtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))];
			pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y += m_aInfo.pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y;

			// 今回の頂点
			int nNowPoint = (nCntWidth + 1) + (nCntHeight * (m_aInfo.nWidthBlock + 1));
			int nVerTexW = (m_aInfo.nWidthBlock + 1) + 1;

			int nLeft = nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1));
			int nRight = nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1)) + nVerTexW;

			if (nNowPoint >= (m_aInfo.nWidthBlock + 1) * (m_aInfo.nHeightBlock + 1))
			{
				continue;
			}

			if (nRight >= (m_aInfo.nWidthBlock + 1) * (m_aInfo.nHeightBlock + 1))
			{// 頂点数超えたら

				// 頂点座標の設定
				VtxRight = D3DXVECTOR3(
					(fWidthLen * nCntWidth) - ((fWidthLen * m_aInfo.nWidthBlock) * 0.5f),
					0.0f,
					-((fHeightLen * nCntHeight) - ((fHeightLen * m_aInfo.nHeightBlock) * 0.5f)));
			}
			else
			{
				VtxRight = pVtxPos[nRight];
			}

			if (nLeft >= (m_aInfo.nWidthBlock + 1) * (m_aInfo.nHeightBlock + 1))
			{// 頂点数超えたら

				// 頂点座標の設定
				VtxLeft = D3DXVECTOR3(
					(fWidthLen * nCntWidth) - ((fWidthLen * m_aInfo.nWidthBlock) * 0.5f),
					0.0f,
					-((fHeightLen * nCntHeight) - ((fHeightLen * m_aInfo.nHeightBlock) * 0.5f)));
			}
			else
			{
				VtxLeft = pVtxPos[nLeft];
			}

			if (nNowPoint >= (m_aInfo.nWidthBlock + 1) * (m_aInfo.nHeightBlock + 1))
			{// 頂点数超えたら

				// 頂点座標の設定
				VtxNow = D3DXVECTOR3(
					(fWidthLen * nCntWidth) - ((fWidthLen * m_aInfo.nWidthBlock) * 0.5f),
					0.0f,
					-((fHeightLen * nCntHeight) - ((fHeightLen * m_aInfo.nHeightBlock) * 0.5f)));
			}
			else
			{
				VtxNow = pVtxPos[nNowPoint];
			}

			// ベクトルを計算
			vec1 = VtxRight - VtxNow;
			vec2 = VtxLeft - VtxNow;

			//// 頂点の計算した数を加算
			//nCntCalVtx[nRight]++;
			//nCntCalVtx[nLeft]++;
			//nCntCalVtx[nNowPoint]++;

			//if (nRight == 0 || nLeft == 0)
			//{
			//	int n = 0;
			//}

			// 外積を求める
			D3DXVec3Cross(&nor, &vec1, &vec2);

			// 外積の正規化をして法線にする
			D3DXVec3Normalize(&nor, &nor);

			//// 法線を加算する
			//vecCalVtx[nNowPoint] += nor;

			// 法線
			pVtxNor[nNowPoint] = nor;
		}
	}

	//for (int nCntHeight = 0; nCntHeight < m_aInfo.nHeightBlock; nCntHeight++)
	//{// 縦の分割分繰り返す

	//	for (int nCntWidth = 0; nCntWidth < m_aInfo.nWidthBlock; nCntWidth++)
	//	{// 横の分割分繰り返す

	//		// 法線を加算する
	//		vecCalVtx[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].x /= (float)nCntCalVtx[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))];
	//		vecCalVtx[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y /= (float)nCntCalVtx[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))];
	//		vecCalVtx[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].z /= (float)nCntCalVtx[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))];

	//		if (nCntCalVtx[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))] == 6)
	//		{
	//			int n = 0;
	//		}

	//		// 外積の正規化をして法線にする
	//		D3DXVec3Normalize(&vecCalVtx[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))], &vecCalVtx[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))]);

	//		// 法線
	//		pVtxNor[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))] = vecCalVtx[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))];
	//	}
	//}

	// 頂点情報更新
	CObject3DMesh::SetVtx();
}

//==========================================================================
// ロード処理
//==========================================================================
HRESULT CElevation::Load(const char *pText)
{
	char aComment[mylib_const::MAX_STRING] = {};	//コメント用

	FILE *pFile = NULL;	// ファイルポインタを宣言

	// ファイルを開く
	pFile = fopen("data\\TEXT\\elevation_field.txt", "r");

	if (pFile == NULL)
	{// ファイルが開けなかった場合
		fclose(pFile);
		return E_FAIL;
	}

	int nCntStage = 0;	// ステージのカウント

	while (1)
	{// END_SCRIPTが来るまで繰り返す

		// 文字列の読み込み
		fscanf(pFile, "%s", &aComment[0]);

		if (strcmp(&aComment[0], "TEXTURE_FILENAME") == 0)
		{// TEXTYPEが来たら種類読み込み

			fscanf(pFile, "%s", &aComment[0]);	// =の分
			fscanf(pFile, "%s", &aComment[0]);	// テクスチャファイル名

			// ファイル名保存
			m_aInfo.TextureFileName = aComment;
		}

		// メッシュフィールドの設定
		if (strcmp(&aComment[0], "FIELDSET") == 0)
		{// モデルの読み込みを開始

			while (strcmp(&aComment[0], "END_FIELDSET"))
			{// END_FIELDSETが来るまで繰り返し

				fscanf(pFile, "%s", &aComment[0]);	// 確認する

				if (strcmp(&aComment[0], "POS") == 0)
				{// POSが来たら位置読み込み

					fscanf(pFile, "%s", &aComment[0]);	// =の分
					fscanf(pFile, "%f", &m_aInfo.pos.x);		// X座標
					fscanf(pFile, "%f", &m_aInfo.pos.y);		// Y座標
					fscanf(pFile, "%f", &m_aInfo.pos.z);		// Z座標
				}

				if (strcmp(&aComment[0], "BLOCK") == 0)
				{// BLOCKが来たら向き読み込み

					fscanf(pFile, "%s", &aComment[0]);			// =の分
					fscanf(pFile, "%d", &m_aInfo.nWidthBlock);	// 横の分割数
					fscanf(pFile, "%d", &m_aInfo.nHeightBlock);	// 縦の分割数
				}

				if (strcmp(&aComment[0], "SIZE") == 0)
				{// SIZEが来たら向き読み込み

					fscanf(pFile, "%s", &aComment[0]);		// =の分
					fscanf(pFile, "%f", &m_aInfo.fWidthLength);	// 横の長さ
					fscanf(pFile, "%f", &m_aInfo.fHeightLength);	// 縦の長さ
				}

				if (strcmp(&aComment[0], "VTXPOS") == 0)
				{// VTXPOSが来たら頂点読み込み

					// 頂点数
					int nVtxNum = (m_aInfo.nHeightBlock + 1) * (m_aInfo.nWidthBlock + 1);
					int nCntVtx = 0;

					// 頂点数でメモリ確保
					if (nCntStage == 0)
					{
						m_aInfo.pVtxPos = DEBUG_NEW D3DXVECTOR3[nVtxNum];
					}
					m_aInfo.pStageVtxPos[nCntStage] = DEBUG_NEW D3DXVECTOR3[nVtxNum];

					// 頂点読み込み用
					std::string VtxPos;

					while (1)
					{// END_VTXPOSが来るまで繰り返し

						fscanf(pFile, "%s", (char*)VtxPos.c_str());

						if (strcmp(VtxPos.c_str(), "END_VTXPOS") == 0)
						{// 途中終了されてたら

							for (int nCnt = nCntVtx; nCnt < nVtxNum; nCnt++)
							{
								// 0クリア
								m_aInfo.pVtxPos[nCnt].y = 0.0f;	// 高さ
								m_aInfo.pStageVtxPos[nCntStage][nCnt].y = 0.0f;	// 代入
							}
							break;
						}

						// float変換
						if (nCntStage == 0)
						{
							m_aInfo.pVtxPos[nCntVtx].y = 0.0f;	// 高さ
						}
						m_aInfo.pStageVtxPos[nCntStage][nCntVtx].y = std::strtof(VtxPos.c_str(), nullptr);	// 代入
						nCntVtx++;
					}
					// ステージ数加算
					nCntStage++;
				}

			}// END_FIELDSETのかっこ
		}

		if (strcmp(&aComment[0], "END_SCRIPT") == 0)
		{// 終了文字でループを抜ける

			break;
		}
	}

	// ファイルを閉じる
	fclose(pFile);
	return S_OK;

}

//==========================================================================
// セーブ処理
//==========================================================================
void CElevation::Save(void)
{
	FILE *pFile = NULL;	// ファイルポインタを宣言

	// ファイルを開く
	pFile = fopen("data\\TEXT\\elevation_field_save.txt", "w");

	if (pFile == NULL)
	{// ファイルが開けなかった場合
		return;
	}

	// ファイルに書き出す
	fprintf(pFile,
		"#------------------------------------------------------------------------------\n"
		"# テクスチャファイル名\n"
		"#------------------------------------------------------------------------------\n");

	// テクスチャファイル名
	fprintf(pFile, "TEXTURE_FILENAME = %s\t\t\n", m_aInfo.TextureFileName.c_str());
	
	// フィールドの設置
	fprintf(pFile,
		"\n"
		"#==============================================================================\n"
		"# フィールドの設置\n"
		"#==============================================================================\n");

	float m_fWidthLen = GetWidthLen();
	float m_fHeightLen = GetHeightLen();

	fprintf(pFile,
		"FIELDSET\n"
		"\tPOS = %.2f %.2f %.2f\n"
		"\tBLOCK = %d %d\n"
		"\tSIZE = %.2f %.2f\n"
		"\tVTXPOS\n", m_aInfo.pos.x, m_aInfo.pos.y, m_aInfo.pos.z,
		m_aInfo.nWidthBlock, m_aInfo.nHeightBlock,
		m_fWidthLen, m_fHeightLen);

	// 頂点取得
	D3DXVECTOR3 *pVtxPos = GetVtxPos();

	// 頂点情報の設定
	for (int nCntHeight = 0; nCntHeight < m_aInfo.nHeightBlock + 1; nCntHeight++)
	{//縦の頂点数分繰り返す

		fprintf(pFile, "\t");
		for (int nCntWidth = 0; nCntWidth < m_aInfo.nWidthBlock + 1; nCntWidth++)
		{// 横の頂点数分繰り返す

			fprintf(pFile, "%.2f ", pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y);
		}
		fprintf(pFile, "\n");
	}
	fprintf(pFile, "\nEND_VTXPOS\n");

	fprintf(pFile, "\nEND_FIELDSET\n");
	fprintf(pFile, "\nEND_SCRIPT		# この行は絶対消さないこと！");

	//ファイルを閉じる
	fclose(pFile);
}

//==========================================================================
// エディットの判定
//==========================================================================
bool CElevation::IsEdit(void)
{
	return m_bEdit;
}

//==========================================================================
// 起伏の情報取得
//==========================================================================
CElevation *CElevation::GetElevation(void)
{
	return this;
}
