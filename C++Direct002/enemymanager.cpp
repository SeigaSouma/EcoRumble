//=============================================================================
// 
//  敵のマネージャ処理 [enemymanager.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "debugproc.h"
#include "enemymanager.h"
#include "enemybase.h"
#include "wavemanager.h"
#include "calculation.h"
#include "manager.h"
#include "game.h"
#include "renderer.h"
#include "enemy.h"
#include "boss.h"
#include "power_gauge.h"
#include "particle.h"
#include "fade.h"
#include "elevation.h"
#include "player.h"
#include "impactwave.h"
#include "camera.h"
#include "sound.h"

// 継承先
#include "tutorial_enemymanager.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define SPAWN_STRENGTH	(100.0f)	// スポーン時の高さ
#define KILL_INT		(30)		// 倒したインターバル

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
CEnemyManager::Pattern CEnemyManager::m_aPattern[mylib_const::MAX_PATTEN_ENEMY] = {};			// 読み込みデータ
CWaveManager::SetStageInfo CEnemyManager::m_aSetEnemyInfo[mylib_const::MAX_WAVE_ENEMY] = {};	// 敵の配置情報
std::string CEnemyManager::sMotionFileName[mylib_const::MAX_PATTEN_ENEMY] = {};					// モーションファイル名
int CEnemyManager::m_nPatternNum = 0;	// 出現パターン数
CEnemy *CEnemyManager::m_pEnemy[mylib_const::MAX_OBJ] = {};
CBoss *CEnemyManager::m_pBoss[mylib_const::MAX_OBJ] = {};
int CEnemyManager::m_nNumAll = 0;		// 総数
int CEnemyManager::m_nNumChara = 0;		// 敵の種類の総数
int CEnemyManager::m_nCntSpawn = 0;		// 出現カウント
int CEnemyManager::m_nParent[mylib_const::MAX_OBJ] = {};	// 敵の親ポインタ
bool CEnemyManager::m_bLoadPattern = false;	// パターン読み込み判定
int CEnemyManager::m_nNumTotalKill = 0;	// まとめて倒した数
int CEnemyManager::m_nIntervalKill = 0;	// 倒したカウントのインターバル
bool CEnemyManager::m_bHitStop = false;	// ヒットストップの判定
CEnemyManager::STATE CEnemyManager::m_state = CEnemyManager::STATE_NONE;		// 状態

//==========================================================================
// コンストラクタ
//==========================================================================
CEnemyManager::CEnemyManager()
{
	// 値のクリア
	for (int nCntEnemy = 0; nCntEnemy < mylib_const::MAX_OBJ; nCntEnemy++)
	{
		// 親のID
		m_nParent[nCntEnemy] = -1;
	}
	m_state = STATE_NONE;	// 状態

	// 総数リセット
	m_nNumAll = 0;
}

//==========================================================================
// デストラクタ
//==========================================================================
CEnemyManager::~CEnemyManager()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CEnemyManager *CEnemyManager::Create(const std::string pTextFile)
{
	// 生成用のオブジェクト
	CEnemyManager *pModel = NULL;

	if (pModel == NULL)
	{// NULLだったら

		// メモリの確保
		switch (CManager::GetMode())
		{
		case CScene::MODE_TUTORIAL:
			pModel = DEBUG_NEW CTutorialEnemyManager;
			break;

		default:
			pModel = DEBUG_NEW CEnemyManager;
			break;
		}

		if (pModel != NULL)
		{// メモリの確保が出来ていたら

			// 初期化処理
			HRESULT hr = pModel->ReadText(pTextFile);

			if (FAILED(hr))
			{// 失敗していたら
				return NULL;
			}

			// 初期化処理
			hr = pModel->Init();

			if (FAILED(hr))
			{// 失敗していたら
				return NULL;
			}
		}

		return pModel;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CEnemyManager::Init(void)
{
	// 出現情報読み込み
	//HRESULT hr;

	//if (FAILED(hr))
	//{// 失敗していたら
	//	return E_FAIL;
	//}

	for (int nCntEnemy = 0; nCntEnemy < mylib_const::MAX_OBJ; nCntEnemy++)
	{
		// 親のID
		m_nParent[nCntEnemy] = -1;
	}

	// 総数リセット
	m_nNumAll = 0;
	m_nCntSpawn = 0;

	//SetBoss(4, D3DXVECTOR3(0.0f, 50.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CEnemyManager::Uninit(void)
{
	for (int nCntEnemy = 0; nCntEnemy < mylib_const::MAX_OBJ; nCntEnemy++)
	{
		if (m_pEnemy[nCntEnemy] != NULL)
		{
			//delete m_pEnemy[nCntEnemy];
			m_pEnemy[nCntEnemy] = NULL;
			m_nParent[nCntEnemy] = -1;
		}
	}
}

//==========================================================================
// 破棄
//==========================================================================
void CEnemyManager::Release(int nIdx)
{
	if (m_pEnemy[nIdx] != NULL)
	{
		//delete m_pEnemy[nIdx];
		//memset(&m_pEnemy[nIdx], NULL, sizeof(m_pEnemy[nIdx]));
		m_pEnemy[nIdx] = NULL;
		m_nParent[nIdx] = -1;
	}

	// 総数減算
	m_nNumAll--;
}

//==========================================================================
// 破棄
//==========================================================================
void CEnemyManager::ReleaseBoss(int nIdx)
{
	if (m_pBoss[nIdx] != NULL)
	{
		m_pBoss[nIdx] = NULL;
	}

	m_nNumAll--;
}

//==========================================================================
// 情報登録
//==========================================================================
void CEnemyManager::SetEnemyInfo(CWaveManager::SetInfo *info, int nStage, int nWave, int nSpawnNum)
{
	for (int nCntSpawn = 0; nCntSpawn < nSpawnNum; nCntSpawn++)
	{
		// 情報渡す
		m_aSetEnemyInfo[nStage].aWaveInfo[nWave].Info[nCntSpawn] = info[nCntSpawn];
	}

	// 出現する数加算
	m_aSetEnemyInfo[nStage].aWaveInfo[nWave].nSetNum = nSpawnNum;
}

//==========================================================================
// 倒したフラグを設定
//==========================================================================
void CEnemyManager::SetKillFlag(void)
{
	// まとめて倒した数加算
	m_nNumTotalKill++;

	// 倒した時のインターバルリセット
	m_nIntervalKill = KILL_INT;

	if (m_nNumTotalKill >= mylib_const::COMBONUM_3)
	{// 3段階目を超えたら
		// 目標の距離設定
		CManager::GetCamera()->SetLenDest(CManager::GetCamera()->GetOriginDistance() + 700.0f, 90, 5.0f);

		if (m_bHitStop == false)
		{
			//CManager::SetEnableHitStop(8);

			// 振動
			CManager::GetCamera()->SetShake(20, 30.0f, 0.0f);

			// スラッシュ衝撃波設定
			D3DXVECTOR3 pos = CManager::GetScene()->GetPlayer()->GetPosition();
			pos.y += 200.0f;
			SetSlashImpact(pos);

			m_bHitStop = true;
		}
	}
}

//==========================================================================
// 更新処理
//==========================================================================
void CEnemyManager::Update(void)
{
	// ウェーブ情報取得
	int nNowWave = CManager::GetScene()->GetWaveManager()->GetNowWave();
	int nMaxWave = CManager::GetScene()->GetWaveManager()->GetMaxWave();

	int nSpawnNum = 0;
	if (nNowWave < nMaxWave - 1)
	{
		nSpawnNum = CManager::GetScene()->GetWaveManager()->GetRespawnThreshold();
	}

	if (m_nNumAll <= nSpawnNum && CManager::GetScene()->GetWaveManager()->IsWaveFrag() == false)
	{// 敵がいなくなったら

		// ウェーブ終了フラグを立てる
		CManager::GetScene()->GetWaveManager()->SetEnableWaveEnd();
	}

	// インターバル減算
	m_nIntervalKill--;

	if (m_nIntervalKill <= 0 && m_state != STATE_COMBOANIM)
	{// 最低値で固定

		if (m_bHitStop == true)
		{// ヒットストップまでいっていたら
			m_state = STATE_COMBOANIM;

			// コンボ入り
			CManager::GetSound()->PlaySound(CSound::LABEL_SE_COMBOIN);
		}

		m_nNumTotalKill = 0;
		m_nIntervalKill = 0;
		m_bHitStop = false;
	}

	if (m_state == STATE_COMBOANIM)
	{// コンボ時のアニメーション状態の時
		UpdateComboAnimation();
	}

	// テキストの描画
	CManager::GetDebugProc()->Print(
		"---------------- 敵情報 ----------------\n"
		"【残り人数】[%d]\n", m_nNumAll);
}

//==========================================================================
// ウェーブ毎の配置処理
//==========================================================================
void CEnemyManager::SetWaveEnemy(int nStage, int nWave)
{
	// 敵拠点データ取得
	CEnemyBase::BaseData *pEnemyBase = CGame::GetEnemyBase()->GetBaseData(nStage);

	for (int nCntEnemy = 0; nCntEnemy < m_aSetEnemyInfo[nStage].aWaveInfo[nWave].nSetNum; nCntEnemy++)
	{
		// 今回の情報一時保存
		CWaveManager::SetInfo info = m_aSetEnemyInfo[nStage].aWaveInfo[nWave].Info[nCntEnemy];

		// 敵出現
		SetEnemy(info.nType, pEnemyBase[info.nBase].pos + info.pos, D3DXVECTOR3(0.0f, 0.0f, 0.0f), pEnemyBase[info.nBase].rot, info.nMoveType);
	}
}

//==========================================================================
// スポーンの起伏設定
//==========================================================================
void CEnemyManager::SetSpawnWave(int nStage, int nWave)
{
	// スポーン時の起伏設定
	CElevation *pElevation = CGame::GetObject3DMesh();

	// 敵拠点データ取得
	CEnemyBase::BaseData *pEnemyBase = CGame::GetEnemyBase()->GetBaseData(nStage);

	int aParent[mylib_const::MAX_PATTEN_ENEMY][mylib_const::MAX_ENEMY] = {};
	int nToatalCount = 0;

	for (int nCntPattern = 0; nCntPattern < m_aSetEnemyInfo[nStage].aWaveInfo[nWave].nSetNum; nCntPattern++)
	{
		// 今回の情報一時保存
		CWaveManager::SetInfo info = m_aSetEnemyInfo[nStage].aWaveInfo[nWave].Info[nCntPattern];
		int nPattern = info.nType;

		switch (info.nSpawnType)
		{
		case SPAWNTYPE_NORMAL:
			SetSpawnWaveNormal(nStage, nWave, nCntPattern, &aParent[nCntPattern][0]);
			break;

		case SPAWNTYPE_ORDER:
			SetSpawnWaveOrder(nStage, nWave, nCntPattern, &aParent[nCntPattern][0]);
			break;

		case SPAWNTYPE_CIRCLE:
			SetSpawnWaveCircle(nStage, nWave, nCntPattern, &aParent[nCntPattern][0]);
			break;

		case SPAWNTYPE_CLOCK:
			SetSpawnWaveClock(nStage, nWave, nCntPattern, &aParent[nCntPattern][0]);
			break;

		case SPAWNTYPE_BOTHCLOCK:
			SetSpawnWaveBothClock(nStage, nWave, nCntPattern, &aParent[nCntPattern][0]);
			break;

		default:
			SetSpawnWaveNormal(nStage, nWave, nCntPattern, &aParent[nCntPattern][0]);
			break;
		}

#if 0
		// 今回の情報一時保存
		CWaveManager::SetInfo info = m_aSetEnemyInfo[nStage].aWaveInfo[nWave].Info[nCntPattern];
		CEnemyBase::BaseData EnemyBase = pEnemyBase[info.nBase];
		int nPattern = info.nType;
		int nNumSpawn = m_aPattern[nPattern].nNumEnemy;	// スポーンする数

		if (info.nSpawnType == SPAWNTYPE_CIRCLE ||
			info.nSpawnType ==  SPAWNTYPE_CLOCK)
		{// 複数指定されているとき
			nNumSpawn = info.nSpawnNum;
		}

		// 1分割の角度
		int nNumDivision = nNumSpawn;

		if (info.nSpawnType == SPAWNTYPE_BOTHCLOCK)
		{// 両回りの時

			// 出現数の半分
			nNumDivision = nNumSpawn / 2;
		}

		// 1分割あたりの角度
		float fAngleOneDgree = (D3DX_PI * 2.0f) / nNumDivision;

		int nCntNULL = 0;
		for (int nCntEnemy = 0; nCntEnemy < nNumSpawn; nCntEnemy++)
		{// 1体ずつセット

			for (nCntNULL = nCntNULL; nCntNULL < mylib_const::MAX_OBJ; nCntNULL++)
			{
				if (m_nParent[nCntNULL] != -1)
				{// 情報が入ってたら
					continue;
				}

				// 親の番号保存
				m_nParent[m_nNumAll] = nCntNULL;

				// 親番号保存
				aParent[nCntPattern][nCntEnemy] = nCntNULL;

				// 敵の情報
				int nWaitTime = (int)(45.0f / (float)nNumDivision);

				// 敵の情報
				EnemyData enemyData = m_aPattern[nPattern].EnemyData[nCntEnemy];

				if (info.nSpawnType == SPAWNTYPE_CIRCLE ||
					info.nSpawnType == SPAWNTYPE_CLOCK ||
					info.nSpawnType == SPAWNTYPE_BOTHCLOCK)
				{// 複数指定されているとき
					enemyData = m_aPattern[nPattern].EnemyData[0];
				}

				// 出現位置
				D3DXVECTOR3 spawnPos;

				if (info.nSpawnType == SPAWNTYPE_CIRCLE || info.nSpawnType == SPAWNTYPE_CLOCK || info.nSpawnType == SPAWNTYPE_BOTHCLOCK)
				{// 円 or 時計回りだったら

					// プレイヤーの位置取得
					D3DXVECTOR3 PlayerPos = CManager::GetScene()->GetPlayer()->GetPosition();

					// プレイヤーの周囲に設定
					spawnPos = D3DXVECTOR3(
						PlayerPos.x + sinf(D3DX_PI + fAngleOneDgree * nCntEnemy) * info.pos.x,
						0.0f,
						PlayerPos.z + cosf(D3DX_PI + fAngleOneDgree * nCntEnemy) * info.pos.x);

					switch (info.nSpawnType)
					{
					case SPAWNTYPE_CIRCLE:
						nWaitTime = 0;
						break;

					case SPAWNTYPE_CLOCK:
						nWaitTime = nWaitTime * nCntEnemy;
						break;

					case SPAWNTYPE_BOTHCLOCK:
						nWaitTime = nWaitTime * nCntEnemy;
						break;

					default:
						break;
					}

				}
				else
				{
					// 待機時間
					if (info.nSpawnType == SPAWNTYPE_ORDER)
					{
						nWaitTime = nWaitTime * nCntEnemy;
					}
					else
					{
						nWaitTime = enemyData.nWaitTime;
					}

					// 計算用マトリックス
					D3DXMATRIX mtxRot, mtxTrans, mtxWorld;
					D3DXMATRIX mtxRotOrigin;

					// マトリックスの初期化
					D3DXMatrixIdentity(&mtxWorld);
					D3DXMatrixIdentity(&mtxRotOrigin);

					// 元の向きを反映する
					D3DXMatrixRotationYawPitchRoll(&mtxRotOrigin, EnemyBase.rot.y, EnemyBase.rot.x, EnemyBase.rot.z);

					// 向きを反映する
					D3DXMatrixRotationYawPitchRoll(&mtxRot, 0.0f, 0.0f, 0.0f);
					D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRot);

					// パターン内の位置を反映する
					D3DXMatrixTranslation(&mtxTrans, enemyData.pos.x, enemyData.pos.y, enemyData.pos.z);
					D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTrans);
					D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRotOrigin);

					// スポーン時の向きを掛け合わせる
					spawnPos = D3DXVECTOR3(mtxWorld._41, mtxWorld._42, mtxWorld._43);

					// 位置設定
					spawnPos = D3DXVECTOR3(spawnPos.x + EnemyBase.pos.x + info.pos.x, 0.0f, spawnPos.z + EnemyBase.pos.z + info.pos.z);
				}

				// タイプ別の範囲
				D3DXVECTOR2 range = D3DXVECTOR2(200.0f, 200.0f);
				float fStrength = SPAWN_STRENGTH;

				switch ((CEnemy::TYPE)enemyData.nType)
				{
				case CEnemy::TYPE_POWER:
					break;

				case CEnemy::TYPE_CROWD:
					range = D3DXVECTOR2(80.0f, 80.0f);
					fStrength = SPAWN_STRENGTH * 0.5f;
					break;

				case CEnemy::TYPE_JUMP:
					range = D3DXVECTOR2(125.0f, 125.0f);
					fStrength = SPAWN_STRENGTH * 0.75f;
					break;

				case CEnemy::TYPE_BOMB:
					break;

				case CEnemy::TYPE_SPEED:
					break;

				case CEnemy::TYPE_IMPACT:
					break;

				case CEnemy::TYPE_IMPACTCENTER:
					break;
				}

				// 親のID計算
				int nParent = -1;
				if (enemyData.nParent >= 0)
				{
					nParent = aParent[nCntPattern][enemyData.nParent]/* + nToatalCount*/;
					//nParent = m_nParent[nParent];
				}
				else
				{
					nParent = -1;
				}

				// 波発生
				pElevation->SetSpawnWave(
					nCntNULL,				// 自分のインデックス番号
					nParent,				// 親のインデックス番号
					nCntEnemy,				// 何番目の敵か
					nWaitTime,				// 待機時間
					info,					// ウェーブマネージャの情報
					spawnPos,				// 発生地点
					range,					// 範囲
					fStrength,				// 高さ
					60						// 持続時間
				);

				// 総数加算
				m_nNumAll++;

				// 親番号保存
				aParent[nCntPattern][nCntEnemy] = nCntNULL;
				break;
			}
		}
#endif
		nToatalCount += m_aPattern[nPattern].nNumEnemy;
	}
}

//==========================================================================
// 通常湧き
//==========================================================================
void CEnemyManager::SetSpawnWaveNormal(int nStage, int nWave, int nCntPattern, int *pParent)
{
	// スポーン時の起伏設定
	CElevation *pElevation = CGame::GetObject3DMesh();

	// 敵拠点データ取得
	CEnemyBase::BaseData *pEnemyBase = CGame::GetEnemyBase()->GetBaseData(nStage);

	// 今回の情報一時保存
	CWaveManager::SetInfo info = m_aSetEnemyInfo[nStage].aWaveInfo[nWave].Info[nCntPattern];
	CEnemyBase::BaseData EnemyBase = pEnemyBase[info.nBase];
	int nPattern = info.nType;
	int nNumSpawn = m_aPattern[nPattern].nNumEnemy;	// スポーンする数

	int nCntNULL = 0;
	for (int nCntEnemy = 0; nCntEnemy < nNumSpawn; nCntEnemy++)
	{// 1体ずつセット

		for (nCntNULL = nCntNULL; nCntNULL < mylib_const::MAX_OBJ; nCntNULL++)
		{
			if (m_nParent[nCntNULL] != -1)
			{// 情報が入ってたら
				continue;
			}

			// 親の番号保存
			m_nParent[nCntNULL] = nCntNULL;

			// 親番号保存
			pParent[nCntEnemy] = nCntNULL;

			// 敵の情報
			EnemyData enemyData = m_aPattern[nPattern].EnemyData[nCntEnemy];

			// 待機時間
			int nWaitTime = enemyData.nWaitTime;

			// 計算用マトリックス
			D3DXMATRIX mtxRot, mtxTrans, mtxWorld;
			D3DXMATRIX mtxRotOrigin;

			// マトリックスの初期化
			D3DXMatrixIdentity(&mtxWorld);
			D3DXMatrixIdentity(&mtxRotOrigin);

			
			// 向きを反映する
			D3DXMatrixRotationYawPitchRoll(&mtxRotOrigin, EnemyBase.rot.y + info.rot.y, EnemyBase.rot.x, EnemyBase.rot.z);

			// 向きを反映する
			D3DXMatrixRotationYawPitchRoll(&mtxRot, info.rot.y, info.rot.x, info.rot.z);
			D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRot);

			// パターン内の位置を反映する
			D3DXMatrixTranslation(&mtxTrans, enemyData.pos.x, enemyData.pos.y, enemyData.pos.z);
			D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTrans);
			D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRotOrigin);

			// スポーン時の向きを掛け合わせる
			D3DXVECTOR3 spawnPos = D3DXVECTOR3(mtxWorld._41, mtxWorld._42, mtxWorld._43);
			D3DXVECTOR3 spawnRot = D3DXVECTOR3(EnemyBase.rot.x, EnemyBase.rot.y + info.rot.y, EnemyBase.rot.z);

			// 位置設定
			spawnPos = D3DXVECTOR3(spawnPos.x + EnemyBase.pos.x + info.pos.x, 0.0f, spawnPos.z + EnemyBase.pos.z + info.pos.z);

			// タイプ別の範囲
			D3DXVECTOR2 range = D3DXVECTOR2(200.0f, 200.0f);
			float fStrength = SPAWN_STRENGTH;

			switch ((CEnemy::TYPE)enemyData.nType)
			{
			case CEnemy::TYPE_POWER:
				break;

			case CEnemy::TYPE_CROWD:
				range = D3DXVECTOR2(80.0f, 80.0f);
				fStrength = SPAWN_STRENGTH * 0.75f;
				break;

			case CEnemy::TYPE_JUMP:
				range = D3DXVECTOR2(125.0f, 125.0f);
				fStrength = SPAWN_STRENGTH * 0.75f;
				break;

			case CEnemy::TYPE_BOMB:
				break;

			case CEnemy::TYPE_SPEED:
				break;

			case CEnemy::TYPE_IMPACT:
				break;

			case CEnemy::TYPE_IMPACTCENTER:
				break;
			}

			// 親のID計算
			int nParent = -1;
			if (enemyData.nParent >= 0)
			{
				nParent = pParent[enemyData.nParent]/* + nToatalCount*/;
				//nParent = m_nParent[nParent];
			}
			else
			{
				nParent = -1;
			}

			// 波発生
			pElevation->SetSpawnWave(
				nCntNULL,				// 自分のインデックス番号
				nParent,				// 親のインデックス番号
				nCntEnemy,				// 何番目の敵か
				nWaitTime,				// 待機時間
				info,					// ウェーブマネージャの情報
				spawnPos,				// 発生地点
				spawnRot,				// 向き
				range,					// 範囲
				fStrength,				// 高さ
				60						// 持続時間
			);

			// 総数加算
			m_nNumAll++;

			// 親番号保存
			pParent[nCntEnemy] = nCntNULL;
			break;
		}
	}
}

//==========================================================================
// 順番湧き
//==========================================================================
void CEnemyManager::SetSpawnWaveOrder(int nStage, int nWave, int nCntPattern, int *pParent)
{
	// スポーン時の起伏設定
	CElevation *pElevation = CGame::GetObject3DMesh();

	// 敵拠点データ取得
	CEnemyBase::BaseData *pEnemyBase = CGame::GetEnemyBase()->GetBaseData(nStage);

	// 今回の情報一時保存
	CWaveManager::SetInfo info = m_aSetEnemyInfo[nStage].aWaveInfo[nWave].Info[nCntPattern];
	CEnemyBase::BaseData EnemyBase = pEnemyBase[info.nBase];
	int nPattern = info.nType;
	int nNumSpawn = m_aPattern[nPattern].nNumEnemy;	// スポーンする数

	int nCntNULL = 0;
	for (int nCntEnemy = 0; nCntEnemy < nNumSpawn; nCntEnemy++)
	{// 1体ずつセット

		for (nCntNULL = nCntNULL; nCntNULL < mylib_const::MAX_OBJ; nCntNULL++)
		{
			if (m_nParent[nCntNULL] != -1)
			{// 情報が入ってたら
				continue;
			}

			// 親の番号保存
			m_nParent[nCntNULL] = nCntNULL;

			// 親番号保存
			pParent[nCntEnemy] = nCntNULL;

			// 敵の情報
			EnemyData enemyData = m_aPattern[nPattern].EnemyData[nCntEnemy];

			// 待機時間
			int nWaitTime = (int)(45.0f / (float)nNumSpawn) * nCntEnemy;

			// 計算用マトリックス
			D3DXMATRIX mtxRot, mtxTrans, mtxWorld;
			D3DXMATRIX mtxRotOrigin;

			// マトリックスの初期化
			D3DXMatrixIdentity(&mtxWorld);
			D3DXMatrixIdentity(&mtxRotOrigin);

			// 元の向きを反映する
			D3DXMatrixRotationYawPitchRoll(&mtxRotOrigin, EnemyBase.rot.y + info.rot.y, EnemyBase.rot.x, EnemyBase.rot.z);

			// 向きを反映する
			D3DXMatrixRotationYawPitchRoll(&mtxRot, info.rot.y, info.rot.x, info.rot.z);
			D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRot);

			// パターン内の位置を反映する
			D3DXMatrixTranslation(&mtxTrans, enemyData.pos.x, enemyData.pos.y, enemyData.pos.z);
			D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTrans);
			D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRotOrigin);

			// スポーン時の向きを掛け合わせる
			D3DXVECTOR3 spawnPos = D3DXVECTOR3(mtxWorld._41, mtxWorld._42, mtxWorld._43);
			D3DXVECTOR3 spawnRot = D3DXVECTOR3(EnemyBase.rot.x, EnemyBase.rot.y + info.rot.y, EnemyBase.rot.z);

			// 位置設定
			spawnPos = D3DXVECTOR3(spawnPos.x + EnemyBase.pos.x + info.pos.x, 0.0f, spawnPos.z + EnemyBase.pos.z + info.pos.z);

			// タイプ別の範囲
			D3DXVECTOR2 range = D3DXVECTOR2(200.0f, 200.0f);
			float fStrength = SPAWN_STRENGTH;

			switch ((CEnemy::TYPE)enemyData.nType)
			{
			case CEnemy::TYPE_POWER:
				break;

			case CEnemy::TYPE_CROWD:
				range = D3DXVECTOR2(80.0f, 80.0f);
				fStrength = SPAWN_STRENGTH * 0.75f;
				break;

			case CEnemy::TYPE_JUMP:
				range = D3DXVECTOR2(125.0f, 125.0f);
				fStrength = SPAWN_STRENGTH * 0.75f;
				break;

			case CEnemy::TYPE_BOMB:
				break;

			case CEnemy::TYPE_SPEED:
				break;

			case CEnemy::TYPE_IMPACT:
				break;

			case CEnemy::TYPE_IMPACTCENTER:
				break;
			}

			// 親のID計算
			int nParent = -1;
			if (enemyData.nParent >= 0)
			{
				nParent = pParent[enemyData.nParent]/* + nToatalCount*/;
				//nParent = m_nParent[nParent];
			}
			else
			{
				nParent = -1;
			}

			// 波発生
			pElevation->SetSpawnWave(
				nCntNULL,				// 自分のインデックス番号
				nParent,				// 親のインデックス番号
				nCntEnemy,				// 何番目の敵か
				nWaitTime,				// 待機時間
				info,					// ウェーブマネージャの情報
				spawnPos,				// 発生地点
				spawnRot,				// 向き
				range,					// 範囲
				fStrength,				// 高さ
				60						// 持続時間
			);

			// 総数加算
			m_nNumAll++;

			// 親番号保存
			pParent[nCntEnemy] = nCntNULL;
			break;
		}
	}
}

//==========================================================================
// 円一気湧き
//==========================================================================
void CEnemyManager::SetSpawnWaveCircle(int nStage, int nWave, int nCntPattern, int *pParent)
{
	// スポーン時の起伏設定
	CElevation *pElevation = CGame::GetObject3DMesh();

	// 敵拠点データ取得
	CEnemyBase::BaseData *pEnemyBase = CGame::GetEnemyBase()->GetBaseData(nStage);

	// 今回の情報一時保存
	CWaveManager::SetInfo info = m_aSetEnemyInfo[nStage].aWaveInfo[nWave].Info[nCntPattern];
	CEnemyBase::BaseData EnemyBase = pEnemyBase[info.nBase];
	int nPattern = info.nType;
	int nNumSpawn = info.nSpawnNum;	// スポーンする数

	// 1分割あたりの角度
	float fAngleOneDgree = (D3DX_PI * 2.0f) / nNumSpawn;

	int nCntNULL = 0;
	for (int nCntEnemy = 0; nCntEnemy < nNumSpawn; nCntEnemy++)
	{// 1体ずつセット

		for (nCntNULL = nCntNULL; nCntNULL < mylib_const::MAX_OBJ; nCntNULL++)
		{
			if (m_nParent[nCntNULL] != -1)
			{// 情報が入ってたら
				continue;
			}

			// 親の番号保存
			m_nParent[nCntNULL] = nCntNULL;

			// 親番号保存
			pParent[nCntEnemy] = nCntNULL;

			// 待機時間
			int nWaitTime = 0;

			// 敵の情報
			EnemyData enemyData = m_aPattern[nPattern].EnemyData[0];

			// プレイヤーの位置取得
			D3DXVECTOR3 PlayerPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			if (CManager::GetScene()->GetPlayer() != NULL)
			{
				PlayerPos = CManager::GetScene()->GetPlayer()->GetPosition();
			}

			// プレイヤーの周囲に設定
			D3DXVECTOR3 spawnPos = D3DXVECTOR3(
				PlayerPos.x + sinf(D3DX_PI + fAngleOneDgree * nCntEnemy) * info.pos.x,
				0.0f,
				PlayerPos.z + cosf(D3DX_PI + fAngleOneDgree * nCntEnemy) * info.pos.x);

			// タイプ別の範囲
			D3DXVECTOR2 range = D3DXVECTOR2(200.0f, 200.0f);
			float fStrength = SPAWN_STRENGTH;

			switch ((CEnemy::TYPE)enemyData.nType)
			{
			case CEnemy::TYPE_POWER:
				break;

			case CEnemy::TYPE_CROWD:
				range = D3DXVECTOR2(80.0f, 80.0f);
				fStrength = SPAWN_STRENGTH * 0.75f;
				break;

			case CEnemy::TYPE_JUMP:
				range = D3DXVECTOR2(125.0f, 125.0f);
				fStrength = SPAWN_STRENGTH * 0.75f;
				break;

			case CEnemy::TYPE_BOMB:
				break;

			case CEnemy::TYPE_SPEED:
				break;

			case CEnemy::TYPE_IMPACT:
				break;

			case CEnemy::TYPE_IMPACTCENTER:
				break;
			}

			// 親のID計算
			int nParent = -1;
			if (enemyData.nParent >= 0)
			{
				nParent = pParent[enemyData.nParent]/* + nToatalCount*/;
				//nParent = m_nParent[nParent];
			}
			else
			{
				nParent = -1;
			}

			D3DXVECTOR3 SetRot = D3DXVECTOR3(0.0f, D3DX_PI + fAngleOneDgree * nCntEnemy, 0.0f);

			// 波発生
			pElevation->SetSpawnWave(
				nCntNULL,				// 自分のインデックス番号
				nParent,				// 親のインデックス番号
				nCntEnemy,				// 何番目の敵か
				nWaitTime,				// 待機時間
				info,					// ウェーブマネージャの情報
				spawnPos,				// 発生地点
				SetRot,					// 向き
				range,					// 範囲
				fStrength,				// 高さ
				60						// 持続時間
			);

			// 総数加算
			m_nNumAll++;

			// 親番号保存
			pParent[nCntEnemy] = nCntNULL;
			break;
		}
	}
}

//==========================================================================
// 時計回り湧き
//==========================================================================
void CEnemyManager::SetSpawnWaveClock(int nStage, int nWave, int nCntPattern, int *pParent)
{
	// スポーン時の起伏設定
	CElevation *pElevation = CGame::GetObject3DMesh();

	// 敵拠点データ取得
	CEnemyBase::BaseData *pEnemyBase = CGame::GetEnemyBase()->GetBaseData(nStage);

	// 今回の情報一時保存
	CWaveManager::SetInfo info = m_aSetEnemyInfo[nStage].aWaveInfo[nWave].Info[nCntPattern];
	CEnemyBase::BaseData EnemyBase = pEnemyBase[info.nBase];
	int nPattern = info.nType;
	int nNumSpawn = info.nSpawnNum;	// スポーンする数

	// 1分割あたりの角度
	float fAngleOneDgree = (D3DX_PI * 2.0f) / nNumSpawn;

	int nCntNULL = 0;
	for (int nCntEnemy = 0; nCntEnemy < nNumSpawn; nCntEnemy++)
	{// 1体ずつセット

		for (nCntNULL = nCntNULL; nCntNULL < mylib_const::MAX_OBJ; nCntNULL++)
		{
			if (m_nParent[nCntNULL] != -1)
			{// 情報が入ってたら
				continue;
			}

			// 親の番号保存
			m_nParent[nCntNULL] = nCntNULL;

			// 親番号保存
			pParent[nCntEnemy] = nCntNULL;

			// 待機時間
			int nWaitTime = (int)(45.0f / (float)nNumSpawn) * nCntEnemy;

			// 敵の情報
			EnemyData enemyData = m_aPattern[nPattern].EnemyData[0];

			// プレイヤーの位置取得
			D3DXVECTOR3 PlayerPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			if (CManager::GetScene()->GetPlayer() != NULL)
			{
				PlayerPos = CManager::GetScene()->GetPlayer()->GetPosition();
			}

			// プレイヤーの周囲に設定
			D3DXVECTOR3 spawnPos = D3DXVECTOR3(
				PlayerPos.x + sinf(D3DX_PI + fAngleOneDgree * nCntEnemy) * info.pos.x,
				0.0f,
				PlayerPos.z + cosf(D3DX_PI + fAngleOneDgree * nCntEnemy) * info.pos.x);

			// タイプ別の範囲
			D3DXVECTOR2 range = D3DXVECTOR2(200.0f, 200.0f);
			float fStrength = SPAWN_STRENGTH;

			switch ((CEnemy::TYPE)enemyData.nType)
			{
			case CEnemy::TYPE_POWER:
				break;

			case CEnemy::TYPE_CROWD:
				range = D3DXVECTOR2(80.0f, 80.0f);
				fStrength = SPAWN_STRENGTH * 0.75f;
				break;

			case CEnemy::TYPE_JUMP:
				range = D3DXVECTOR2(125.0f, 125.0f);
				fStrength = SPAWN_STRENGTH * 0.75f;
				break;

			case CEnemy::TYPE_BOMB:
				break;

			case CEnemy::TYPE_SPEED:
				break;

			case CEnemy::TYPE_IMPACT:
				break;

			case CEnemy::TYPE_IMPACTCENTER:
				break;
			}

			// 親のID計算
			int nParent = -1;
			if (enemyData.nParent >= 0)
			{
				nParent = pParent[enemyData.nParent];
			}
			else
			{
				nParent = -1;
			}

			D3DXVECTOR3 SetRot = D3DXVECTOR3(0.0f, D3DX_PI + fAngleOneDgree * nCntEnemy, 0.0f);
			// 波発生
			pElevation->SetSpawnWave(
				nCntNULL,				// 自分のインデックス番号
				nParent,				// 親のインデックス番号
				nCntEnemy,				// 何番目の敵か
				nWaitTime,				// 待機時間
				info,					// ウェーブマネージャの情報
				spawnPos,				// 発生地点
				SetRot,					// 向き
				range,					// 範囲
				fStrength,				// 高さ
				60						// 持続時間
			);

			// 総数加算
			m_nNumAll++;

			// 親番号保存
			pParent[nCntEnemy] = nCntNULL;
			break;
		}
	}
}

//==========================================================================
// 両回り湧き
//==========================================================================
void CEnemyManager::SetSpawnWaveBothClock(int nStage, int nWave, int nCntPattern, int *pParent)
{
	// スポーン時の起伏設定
	CElevation *pElevation = CGame::GetObject3DMesh();

	// 敵拠点データ取得
	CEnemyBase::BaseData *pEnemyBase = CGame::GetEnemyBase()->GetBaseData(nStage);

	// 今回の情報一時保存
	CWaveManager::SetInfo info = m_aSetEnemyInfo[nStage].aWaveInfo[nWave].Info[nCntPattern];
	CEnemyBase::BaseData EnemyBase = pEnemyBase[info.nBase];
	int nPattern = info.nType;
	int nNumSpawn = info.nSpawnNum;	// スポーンする数

	// 分割数
	int nNumDivision = nNumSpawn / 2;

	// 1分割あたりの角度
	float fAngleOneDgree = D3DX_PI / (float)nNumDivision;

	int nCntNULL = 0, nCntDivision = 0;
	for (int nCntEnemy = 0; nCntEnemy < nNumSpawn; nCntEnemy++)
	{// 1体ずつセット

		for (nCntNULL = nCntNULL; nCntNULL < mylib_const::MAX_OBJ; nCntNULL++)
		{
			if (m_nParent[nCntNULL] != -1)
			{// 情報が入ってたら
				continue;
			}

			// 親の番号保存
			m_nParent[nCntNULL] = nCntNULL;

			// 親番号保存
			pParent[nCntEnemy] = nCntNULL;

			// 待機時間
			int nWaitTime = (int)(50.0f / (float)nNumDivision) * nCntDivision;

			// 敵の情報
			EnemyData enemyData = m_aPattern[nPattern].EnemyData[0];

			// プレイヤーの位置取得
			D3DXVECTOR3 PlayerPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			if (CManager::GetScene()->GetPlayer() != NULL)
			{
				PlayerPos = CManager::GetScene()->GetPlayer()->GetPosition();
			}

			// 向き
			int nAngle = 1;
			if (nCntEnemy % 2 == 0)
			{
				nAngle = -1;
			}

			// プレイヤーの周囲に設定
			D3DXVECTOR3 spawnPos = D3DXVECTOR3(
				PlayerPos.x + sinf(D3DX_PI + (fAngleOneDgree * nCntDivision) * nAngle) * info.pos.x,
				0.0f,
				PlayerPos.z + cosf(D3DX_PI + (fAngleOneDgree * nCntDivision) * nAngle) * info.pos.x);

			// タイプ別の範囲
			D3DXVECTOR2 range = D3DXVECTOR2(200.0f, 200.0f);
			float fStrength = SPAWN_STRENGTH;

			switch ((CEnemy::TYPE)enemyData.nType)
			{
			case CEnemy::TYPE_POWER:
				break;

			case CEnemy::TYPE_CROWD:
				range = D3DXVECTOR2(80.0f, 80.0f);
				fStrength = SPAWN_STRENGTH * 0.75f;
				break;

			case CEnemy::TYPE_JUMP:
				range = D3DXVECTOR2(125.0f, 125.0f);
				fStrength = SPAWN_STRENGTH * 0.75f;
				break;

			case CEnemy::TYPE_BOMB:
				break;

			case CEnemy::TYPE_SPEED:
				break;

			case CEnemy::TYPE_IMPACT:
				break;

			case CEnemy::TYPE_IMPACTCENTER:
				break;
			}

			// 親のID計算
			int nParent = -1;
			if (enemyData.nParent >= 0)
			{
				nParent = pParent[enemyData.nParent];
			}
			else
			{
				nParent = -1;
			}

			D3DXVECTOR3 SetRot = D3DXVECTOR3(0.0f, D3DX_PI + (fAngleOneDgree * nCntDivision) * nAngle, 0.0f);
			// 波発生
			pElevation->SetSpawnWave(
				nCntNULL,				// 自分のインデックス番号
				nParent,				// 親のインデックス番号
				nCntEnemy,				// 何番目の敵か
				nWaitTime,				// 待機時間
				info,					// ウェーブマネージャの情報
				spawnPos,				// 発生地点
				SetRot,					// 向き
				range,					// 範囲
				fStrength,				// 高さ
				60						// 持続時間
			);

			// 総数加算
			m_nNumAll++;

			// 親番号保存
			pParent[nCntEnemy] = nCntNULL;

			if (nCntEnemy % 2 == 0)
			{
				nCntDivision++;
			}
			break;
		}
	}
}

//==========================================================================
// 敵配置
//==========================================================================
void CEnemyManager::SetEnemy(int nPattern, D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 rotBase, int movetype)
{
	if (mylib_const::MAX_PATTEN_ENEMY <= nPattern)
	{
		return;
	}

	int nCntNULL = 0;
	int aParent[mylib_const::MAX_PATTEN_ENEMY] = {};

	for (int nCntEnemy = 0; nCntEnemy < m_aPattern[nPattern].nNumEnemy; nCntEnemy++)
	{
		for (int nCntNULL = 0; nCntNULL < mylib_const::MAX_OBJ; nCntNULL++)
		{
			if (m_pEnemy[nCntNULL] == NULL)
			{// NULLだったら

				// 計算用マトリックス
				D3DXMATRIX mtxRot, mtxTrans, mtxWorld;
				D3DXMATRIX mtxRotOrigin;

				// マトリックスの初期化
				D3DXMatrixIdentity(&mtxWorld);
				D3DXMatrixIdentity(&mtxRotOrigin);

				// 元の向きを反映する
				D3DXMatrixRotationYawPitchRoll(&mtxRotOrigin, rotBase.y, rotBase.x, rotBase.z);

				// 向きを反映する
				D3DXMatrixRotationYawPitchRoll(&mtxRot, rot.y, rot.x, rot.z);
				D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRot);

				// スポーン位置を反映する
				/*D3DXMatrixTranslation(&mtxTrans, pos.x, pos.y, pos.z);
				D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTrans);*/

				// パターン内の位置を反映する
				D3DXMatrixTranslation(&mtxTrans, m_aPattern[nPattern].EnemyData[nCntEnemy].pos.x, m_aPattern[nPattern].EnemyData[nCntEnemy].pos.y, m_aPattern[nPattern].EnemyData[nCntEnemy].pos.z);
				D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTrans);
				D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRotOrigin);

				// スポーン時の向きを掛け合わせる
				//D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRotOrigin);
				D3DXVECTOR3 spawnPos = D3DXVECTOR3(mtxWorld._41, mtxWorld._42, mtxWorld._43);

				// 敵の生成
				m_pEnemy[nCntNULL] = CEnemy::Create(
					nCntNULL,																	// インデックス番号
					sMotionFileName[m_aPattern[nPattern].EnemyData[nCntEnemy].nType].c_str(),	// ファイル名
					D3DXVECTOR3(spawnPos.x + pos.x, -200.0f, spawnPos.z + pos.z),				// 位置
					(CEnemy::TYPE)m_aPattern[nPattern].EnemyData[nCntEnemy].nType);				// 種類

				if (m_pEnemy[nCntNULL] == NULL)
				{// 失敗していたら

					delete m_pEnemy[nCntNULL];
					m_pEnemy[nCntNULL] = NULL;
					break;
				}

				// 向き設定
				m_pEnemy[nCntNULL]->SetRotation(rotBase);
				m_pEnemy[nCntNULL]->SetRotDest(rotBase.y);

				// 行動の種類設定
				m_pEnemy[nCntNULL]->SetMoveType((CEnemy::MOVETYPE)movetype);

				// 親モデルの設定
				if (m_aPattern[nPattern].EnemyData[nCntEnemy].nParent >= 0)
				{
					m_pEnemy[nCntNULL]->SetParent(m_pEnemy[aParent[m_aPattern[nPattern].EnemyData[nCntEnemy].nParent]]);
				}
				else
				{// 親が自分の場合

					// NULL設定
					m_pEnemy[nCntNULL]->SetParent(NULL);
				}

				// 親番号保存
				aParent[nCntEnemy] = nCntNULL;

				break;
			}
		}
	}
}

//==========================================================================
// 敵配置
//==========================================================================
void CEnemyManager::SetEnemy(int nIdx, int nPattern, int nCntEnemy, int nParent, CWaveManager::SetInfo setinfo, D3DXVECTOR3 pos, D3DXVECTOR3 rot, int movetype)
{
	if (m_pEnemy[nIdx] != NULL)
	{// 既に情報が入ってたら
		return;
	}

	int nType = m_aPattern[nPattern].EnemyData[nCntEnemy].nType;

	if (setinfo.nChara != -1)
	{// 何かしら設定されてたら
		nType = setinfo.nChara;
	}

	// 敵の生成
	m_pEnemy[nIdx] = CEnemy::Create(
		nIdx,							// インデックス番号
		sMotionFileName[nType].c_str(),	// ファイル名
		pos,							// 位置
		(CEnemy::TYPE)m_aPattern[nPattern].EnemyData[nCntEnemy].nType,	// 種類
		nType							// キャラの種類
	);


	if (m_pEnemy[nIdx] == NULL)
	{// 失敗していたら

		delete m_pEnemy[nIdx];
		m_pEnemy[nIdx] = NULL;
		return;
	}

	// 向き設定
	m_pEnemy[nIdx]->SetRotation(rot);
	m_pEnemy[nIdx]->SetRotDest(rot.y);

	if ((CEnemy::MOVETYPE)movetype == CEnemy::MOVETYPE_PLAYERRUSH)
	{// プレイヤー突撃だったら

		// プレイヤー情報
		CPlayer *pPlayer = CManager::GetScene()->GetPlayer();

		if (pPlayer != NULL)
		{// NULLじゃないとき

			// 親の位置取得
			D3DXVECTOR3 posPlayer = pPlayer->GetPosition();

			// 目標の角度を求める
			float fRotDest = atan2f((pos.x - posPlayer.x), (pos.z - posPlayer.z));

			m_pEnemy[nIdx]->SetOriginRotation(D3DXVECTOR3(rot.x, fRotDest, rot.z));
			m_pEnemy[nIdx]->SetRotation(D3DXVECTOR3(rot.x, fRotDest, rot.z));
			m_pEnemy[nIdx]->SetRotDest(fRotDest);
		}
	}
	else
	{// それ以外
		m_pEnemy[nIdx]->SetOriginRotation(rot);
	}

	// 行動の種類設定
	m_pEnemy[nIdx]->SetMoveType((CEnemy::MOVETYPE)movetype);

	// 行動の種類設定
	m_pEnemy[nIdx]->SetSpawnType((SPAWNTYPE)setinfo.nSpawnType);

	// 隊列設定
	if (setinfo.nSpawnType == 0)
	{
		int n = 0;
	}

	// 親モデルの設定
	if (nParent >= 0)
	{
		m_pEnemy[nIdx]->SetParent(m_pEnemy[nParent]);
	}
	else
	{// 親が自分の場合

		// NULL設定
		m_pEnemy[nIdx]->SetParent(NULL);
	}

	// 敵の情報
	EnemyData enemyData = m_aPattern[nPattern].EnemyData[nCntEnemy];
	D3DXVECTOR3 posParent = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	if (enemyData.nParent != -1)
	{// 親がいる場合
		posParent = m_aPattern[nPattern].EnemyData[enemyData.nParent].pos;
	}

	// 隊列の位置設定
	m_pEnemy[nIdx]->SetFormationPos(m_aPattern[nPattern].EnemyData[nCntEnemy].pos, posParent);
}

//==========================================================================
// ボス配置
//==========================================================================
void CEnemyManager::SetBoss(int nPattern, D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	if (mylib_const::MAX_PATTEN_ENEMY <= nPattern)
	{
		return;
	}

	int nCntNULL = 0;

	for (int nCntEnemy = 0; nCntEnemy < m_aPattern[nPattern].nNumEnemy; nCntEnemy++)
	{
		for (int nCntNULL = 0; nCntNULL < mylib_const::MAX_OBJ; nCntNULL++)
		{
			if (m_pBoss[nCntNULL] == NULL)
			{// NULLだったら

				// 敵の生成
				m_pBoss[nCntNULL] = CBoss::Create(
					nCntNULL,																	// インデックス番号
					sMotionFileName[m_aPattern[nPattern].EnemyData[nCntEnemy].nType].c_str(),	// ファイル名
					m_aPattern[nPattern].EnemyData[nCntEnemy].pos + pos							// 位置
					);

				if (m_pBoss[nCntNULL] == NULL)
				{// 失敗していたら

					delete m_pBoss[nCntNULL];
					m_pBoss[nCntNULL] = NULL;
					break;
				}

				// 向き設定
				m_pBoss[nCntNULL]->SetRotation(rot);

				// 総数加算
				m_nNumAll++;
				break;
			}
		}
	}
}


//==========================================================================
// コンボ時の演出
//==========================================================================
void CEnemyManager::UpdateComboAnimation(void)
{

	// 目標の距離設定
	CManager::GetCamera()->SetLenDest(CManager::GetCamera()->GetOriginDistance() + 1000.0f, 90, 5.0f);

	static int nnn = 0;
	static int nIntKill = 0;

	nnn = (nnn + 1) % 2;
	nIntKill = (nIntKill + 1) % 4;

	if (nnn != 0)
	{
		return;
	}


	// 振動
	CManager::GetCamera()->SetShake(3, 10.0f, 0.0f);

	int nCntKill = 0;	// 倒された敵の数

	for (int nCntEnemy = 0; nCntEnemy < mylib_const::MAX_OBJ; nCntEnemy++)
	{
		if (m_pEnemy[nCntEnemy] == NULL)
		{// NULLだったら
			continue;
		}

		if (m_pEnemy[nCntEnemy]->IsDeath() == true)
		{// 死亡フラグが立っていたら
			continue;
		}

		if (m_pEnemy[nCntEnemy]->GetState() != CEnemy::STATE_DEAD)
		{// 死亡状態以外の敵
			continue;
		}

		// 倒された敵の数加算
		nCntKill++;

		// スラッシュ衝撃波設定
		SetSlashImpact(m_pEnemy[nCntEnemy]->GetPosition());

		// パーティクル生成
		my_particle::Create(m_pEnemy[nCntEnemy]->GetPosition(), my_particle::TYPE_ENEMYKILL_COMBO);

		// 終了処理
		m_pEnemy[nCntEnemy]->Kill();
		m_pEnemy[nCntEnemy]->Uninit();

		// サウンド再生
		if (nIntKill == 0)
		{
			CManager::GetSound()->PlaySound(CSound::LABEL_SE_COMBOKILL);
		}
		break;
	}

	if (nCntKill == 0)
	{// 終わっていたら
		m_state = STATE_NONE;
	}

}

//==========================================================================
// スラッシュ衝撃波設定
//==========================================================================
void CEnemyManager::SetSlashImpact(D3DXVECTOR3 pos)
{
	D3DXCOLOR col = D3DXCOLOR(1.0f, 0.6f, 0.3f, 0.5f);
	CImpactWave::TYPE type = CImpactWave::TYPE_SMOKE;
	float fMove = 20.0f;
	float fWidth = 100.0f;

#if 1
	// 衝撃波生成
	CImpactWave::Create
	(
		D3DXVECTOR3(pos.x, pos.y + 50.0f, pos.z),	// 位置
		D3DXVECTOR3(D3DX_PI * 0.75f, 0.0f, 0.0f),	// 向き
		col,			// 色
		fWidth,										// 幅
		1.0f,										// 高さ
		30,											// 寿命
		fMove,										// 幅の移動量
		type,					// テクスチャタイプ
		true										// 加算合成するか
	);

	// 衝撃波生成
	CImpactWave::Create
	(
		D3DXVECTOR3(pos.x, pos.y + 50.0f, pos.z),	// 位置
		D3DXVECTOR3(0.0f, 0.0f, 0.0f),	// 向き
		col,			// 色
		fWidth,										// 幅
		1.0f,										// 高さ
		30,											// 寿命
		fMove,										// 幅の移動量
		type,					// テクスチャタイプ
		true										// 加算合成するか
	);

	// 衝撃波生成
	CImpactWave::Create
	(
		D3DXVECTOR3(pos.x, pos.y + 50.0f, pos.z),	// 位置
		D3DXVECTOR3(D3DX_PI * -0.75f, 0.0f, 0.0f),	// 向き
		col,			// 色
		fWidth,										// 幅
		1.0f,										// 高さ
		30,											// 寿命
		fMove,										// 幅の移動量
		type,					// テクスチャタイプ
		true										// 加算合成するか
	);
#endif
}

//==========================================================================
// パターン数
//==========================================================================
int CEnemyManager::GetPatternNum(void)
{
	return m_nPatternNum;
}

//==========================================================================
// 倒した間隔取得
//==========================================================================
int CEnemyManager::GetIntervalKill(void)
{
	return m_nIntervalKill;
}

//==========================================================================
// 間隔内に倒した数取得
//==========================================================================
int CEnemyManager::GetNumIntervalKill(void)
{
	return m_nNumTotalKill;
}

//==========================================================================
// 敵の総数取得
//==========================================================================
int CEnemyManager::GetNumAll(void)
{
	return m_nNumAll;
}

//==========================================================================
// テキスト読み込み処理
//==========================================================================
HRESULT CEnemyManager::ReadText(const std::string pTextFile)
{

	if (m_bLoadPattern == true)
	{// 既に読み込まれていたら
		return S_OK;
	}

	FILE *pFile = NULL;	// ファイルポインタを宣言

	// ファイルを開く
	pFile = fopen(pTextFile.c_str(), "r");

	if (pFile == NULL)
	{//ファイルが開けた場合
		return E_FAIL;
	}

	// 読み込み判定ON
	m_bLoadPattern = true;

	char aComment[MAX_COMMENT];	// コメント
	int nType = 0;				// 配置する種類
	int nCntPatten = 0;			// パターンのカウント
	int nCntFileName = 0;

	while (1)
	{// END_SCRIPTが来るまで繰り返す

		// 文字列の読み込み
		fscanf(pFile, "%s", &aComment[0]);

		// キャラクター数の設定
		if (strcmp(aComment, "NUM_CHARACTER") == 0)
		{// NUM_CHARACTERがきたら

			fscanf(pFile, "%s", &aComment[0]);	// =の分
			fscanf(pFile, "%d", &m_nNumChara);	// キャラクター数
		}

		while (nCntFileName != m_nNumChara)
		{// モデルの数分読み込むまで繰り返し

			// 文字列の読み込み
			fscanf(pFile, "%s", &aComment[0]);

			// モデル名の設定
			if (strcmp(aComment, "MOTION_FILENAME") == 0)
			{// NUM_MODELがきたら

				fscanf(pFile, "%s", &aComment[0]);	// =の分
				fscanf(pFile, "%s", &aComment[0]);	// ファイル名

				// ファイル名保存
				sMotionFileName[nCntFileName] = aComment;

				nCntFileName++;	// ファイル数加算
			}
		}

		// 各パターンの設定
		if (strcmp(aComment, "PATTERNSET") == 0)
		{// 配置情報の読み込みを開始

			int nCntEnemy = 0;			// 敵の配置カウント

			while (strcmp(aComment, "END_PATTERNSET") != 0)
			{// END_PATTERNSETが来るまで繰り返し

				fscanf(pFile, "%s", &aComment[0]);	//確認する

				if (strcmp(aComment, "ENEMYSET") == 0)
				{// ENEMYSETで敵情報の読み込み開始

					while (strcmp(aComment, "END_ENEMYSET") != 0)
					{// END_ENEMYSETが来るまで繰り返す

						fscanf(pFile, "%s", &aComment[0]);	// 確認する

						if (strcmp(aComment, "TYPE") == 0)
						{// TYPEが来たらキャラファイル番号読み込み

							fscanf(pFile, "%s", &aComment[0]);	// =の分
							fscanf(pFile, "%d", &m_aPattern[nCntPatten].EnemyData[nCntEnemy].nType);	// キャラファイル番号
						}

						if (strcmp(aComment, "PARENT") == 0)
						{// PARENTが来たら親の番号読み込み

							fscanf(pFile, "%s", &aComment[0]);	// =の分
							fscanf(pFile, "%d", &m_aPattern[nCntPatten].EnemyData[nCntEnemy].nParent);	// 親の番号
						}

						if (strcmp(aComment, "WAITTIME") == 0)
						{// WAITTIMEが来たら待機時間読み込み

							fscanf(pFile, "%s", &aComment[0]);	// =の分
							fscanf(pFile, "%d", &m_aPattern[nCntPatten].EnemyData[nCntEnemy].nWaitTime);	// 待機時間
						}

						if (strcmp(aComment, "POS") == 0)
						{// POSが来たら位置読み込み

							fscanf(pFile, "%s", &aComment[0]);		// =の分
							fscanf(pFile, "%f", &m_aPattern[nCntPatten].EnemyData[nCntEnemy].pos.x);	// X座標
							fscanf(pFile, "%f", &m_aPattern[nCntPatten].EnemyData[nCntEnemy].pos.y);	// Y座標
							fscanf(pFile, "%f", &m_aPattern[nCntPatten].EnemyData[nCntEnemy].pos.z);	// Z座標
						}

					}// END_ENEMYSETのかっこ

					nCntEnemy++;	// 敵のカウントを加算
					m_aPattern[nCntPatten].nNumEnemy++;	// 敵のカウントを加算
				}
			}// END_PATTERNSETのかっこ

			nCntPatten++;	// パターン加算
		}

		if (strcmp(aComment, "END_SCRIPT") == 0)
		{// 終了文字でループを抜ける
			break;
		}
	}

	// パターン数
	m_nPatternNum = nCntPatten - 1;

	// ファイルを閉じる
	fclose(pFile);

	return S_OK;
}
