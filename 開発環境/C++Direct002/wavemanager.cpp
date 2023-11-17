//=============================================================================
// 
//  敵のマネージャ処理 [wavemanager.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "debugproc.h"
#include "wavemanager.h"
#include "calculation.h"
#include "manager.h"
#include "game.h"
#include "renderer.h"
#include "enemymanager.h"
#include "elevation.h"
#include "fade.h"
#include "stageswitch.h"
#include "camera.h"
#include "sound.h"

//==========================================================================
// マクロ定義
//==========================================================================

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================

//==========================================================================
// コンストラクタ
//==========================================================================
CWaveManager::CWaveManager()
{
	// 値のクリア
	m_nNowWave = 0;			// 現在のウェーブ
	m_nNowStage = 0;		// 現在のステージ
	memset(&m_nNumWave[0], 0, sizeof(m_nNumWave));	// ウェーブの総数
	//m_nNumWave = 0;		// ウェーブの総数
	m_nNumStage = 0;		// ステージの総数
	m_bEndWave = false;		// ウェーブ終了のフラグ
	m_bEndStage = false;	// ステージ終了のフラグ
	memset(&m_nRespawnThreshold[0], 0, sizeof(m_nRespawnThreshold));	// ウェーブ毎の敵リスポーン規定値
}

//==========================================================================
// デストラクタ
//==========================================================================
CWaveManager::~CWaveManager()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CWaveManager *CWaveManager::Create(const std::string pTextFile)
{
	// 生成用のオブジェクト
	CWaveManager *pModel = NULL;

	if (pModel == NULL)
	{// NULLだったら

		// メモリの確保
		pModel = DEBUG_NEW CWaveManager;

		if (pModel != NULL)
		{// メモリの確保が出来ていたら

			// 初期化処理
			HRESULT hr = pModel->Init();

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
HRESULT CWaveManager::Init(void)
{
	HRESULT hr;

	// 出現情報読み込み
	switch (CManager::GetMode())
	{
	case CScene::MODE_TITLE:
		hr = ReadText("data\\TEXT\\set_enemy_info_title.txt");
		break;

	case CScene::MODE_TUTORIAL:
		hr = ReadText("data\\TEXT\\set_enemy_info_tutorial.txt");
		break;

	case CScene::MODE_GAME:
		hr = ReadText("data\\TEXT\\set_enemy_info.txt");
		break;

	case CScene::MODE_RESULT:
		return S_OK;
		break;

	case CScene::MODE_RANKING:
		hr = ReadText("data\\TEXT\\set_enemy_info_title.txt");
		break;

	default:
		return E_FAIL;
		break;
	}

	if (FAILED(hr))
	{// 失敗していたら
		return E_FAIL;
	}

	// ウェーブ終了のフラグ建てる
	m_bEndWave = true;

	// ステージ終了のフラグ建てる
	m_bEndStage = false;

	// 各種変数初期化
	m_nNowWave = -1;
	m_nNowStage = 0;

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CWaveManager::Uninit(void)
{

}

//==========================================================================
// 更新処理
//==========================================================================
void CWaveManager::Update(void)
{
	if (CManager::GetMode() != CScene::MODE_GAME)
	{
		return;
	}

	if (m_bEndWave == false)
	{// ウェーブ中だったら
		return;
	}

	if (m_bEndStage == false)
	{// ステージ変更中
		return;
	}

	// ウェーブを進める
	if (m_nNowWave < mylib_const::MAX_WAVE_ENEMY)
	{
		m_nNowWave++;
	}

	if (m_nNumWave[m_nNowStage] == NULL)
	{// NULLだったら
		int n = 0;
		return;
	}

	if (m_nNumWave[m_nNowStage] <= m_nNowWave)
	{// ウェーブの終端に来たら

		// 現在のステージ加算
		m_nNowStage++;

		// 現在のウェーブリセット
		m_nNowWave = -1;

		// ウェーブ中に変更する
		m_bEndWave = false;

		// ステージ変更中にする
		m_bEndStage = false;

		if (m_nNumStage <= m_nNowStage)
		{// ステージの終端に来たら

			// モード設定
			CManager::GetFade()->SetFade(CScene::MODE_RESULT);
			return;
		}
		else
		{// まだステージがある

			// ステージ切り替え生成
			CStageSwitch::Create();
		}
	}

	// 敵配置
	CGame::GetEnemyManager()->SetSpawnWave(m_nNowStage, m_nNowWave);

	// 自然波配置
	CGame::GetObject3DMesh()->SetWaveNaturalwave(m_nNowStage, m_nNowWave);

	// ウェーブ中に変更する
	m_bEndWave = false;
}

//==========================================================================
// ウェーブ終了のフラグON
//==========================================================================
void CWaveManager::SetEnableWaveEnd(void)
{
	m_bEndWave = true;
}

//==========================================================================
// ウェーブ終了のフラグOFF
//==========================================================================
void CWaveManager::SetDisableWaveEnd(void)
{
	m_bEndWave = false;
}

//==========================================================================
// ステージ終了のフラグON
//==========================================================================
void CWaveManager::SetEnableStageEnd(void)
{
	m_bEndStage = true;
}

//==========================================================================
// ステージ終了のフラグOFF
//==========================================================================
void CWaveManager::SetDisableStageEnd(void)
{
	m_bEndStage = false;
}

//==========================================================================
// 終了のフラグ取得
//==========================================================================
bool CWaveManager::IsWaveFrag(void)
{
	return m_bEndWave;
}

//==========================================================================
// ステージ終了のフラグ取得
//==========================================================================
bool CWaveManager::IsStageFrag(void)
{
	return m_bEndStage;
}

//==========================================================================
// テキスト読み込み処理
//==========================================================================
HRESULT CWaveManager::ReadText(const std::string pTextFile)
{
	FILE *pFile = NULL;	// ファイルポインタを宣言

	// ファイルを開く
	pFile = fopen(pTextFile.c_str(), "r");

	if (pFile == NULL)
	{//ファイルが開けた場合
		return E_FAIL;
	}

	char aComment[MAX_COMMENT];	// コメント
	int nCntSpawnEnemy = 0;			// 何体目か
	int nCntSpawnWave = 0;			// 何体目か

	while (1)
	{// END_SCRIPTが来るまで繰り返す

		// 文字列の読み込み
		fscanf(pFile, "%s", &aComment[0]);

		// 各ステージの設定
		if (strcmp(aComment, "STAGESET") == 0)
		{// ステージ情報の読み込みを開始

			while (strcmp(aComment, "END_STAGESET") != 0)
			{// END_STAGESETが来るまで繰り返し

				// 文字列の読み込み
				fscanf(pFile, "%s", &aComment[0]);

				// 各パターンの設定
				if (strcmp(aComment, "WAVESET") == 0)
				{// 配置情報の読み込みを開始

					// スポーン人数リセット
					nCntSpawnEnemy = 0;
					nCntSpawnWave = 0;

					// 情報保存用
					std::vector<SetInfo> EnemyLoadInfo = {};
					std::vector<SetInfo> WaveLoadInfo = {};

					while (strcmp(aComment, "END_WAVESET") != 0)
					{// END_WAVESETが来るまで繰り返し

						fscanf(pFile, "%s", &aComment[0]);	//確認する

						// 読み込み用
						SetInfo InitInfo = {};
						memset(&InitInfo, NULL, sizeof(InitInfo));
						InitInfo.nChara = -1;

						if (strcmp(aComment, "RESPAWNNUM") == 0)
						{// RESPAWNNUMが来たら敵リスポーン規定値読み込み

							int nNum = 0;

							fscanf(pFile, "%s", &aComment[0]);	// =の分
							fscanf(pFile, "%d", &nNum);			// リスポーン規定値
							m_nRespawnThreshold[m_nNumStage][m_nNumWave[m_nNumStage]] = nNum;
						}

						// 各パターンの設定
						if (strcmp(aComment, "ENEMYSET") == 0)
						{// 配置情報の読み込みを開始

							// 初期化
							InitInfo.nChara = -1;
							EnemyLoadInfo.push_back(InitInfo);

							while (strcmp(aComment, "END_ENEMYSET") != 0)
							{// END_ENEMYSETが来るまで繰り返し

								fscanf(pFile, "%s", &aComment[0]);	//確認する

								if (strcmp(aComment, "TYPE") == 0)
								{// TYPEが来たらキャラファイル番号読み込み

									fscanf(pFile, "%s", &aComment[0]);	// =の分
									fscanf(pFile, "%d", &InitInfo.nType);	// 種類
								}

								if (strcmp(aComment, "BASE") == 0)
								{// BASEが来たら拠点番号読み込み

									fscanf(pFile, "%s", &aComment[0]);	// =の分
									fscanf(pFile, "%d", &InitInfo.nBase);	// 拠点
								}

								if (strcmp(aComment, "SPAWNNUM") == 0)
								{// SPAWNNUMが来たら出現の数

									fscanf(pFile, "%s", &aComment[0]);	// =の分
									fscanf(pFile, "%d", &InitInfo.nSpawnNum);	// 出現する数
								}

								if (strcmp(aComment, "MOVETYPE") == 0)
								{// MOVETYPEが来たら行動の種類読み込み

									fscanf(pFile, "%s", &aComment[0]);	// =の分
									fscanf(pFile, "%d", &InitInfo.nMoveType);	// 行動の種類
								}

								if (strcmp(aComment, "SPAWNTYPE") == 0)
								{// SPAWNTYPEが来たら出現の種類読み込み

									fscanf(pFile, "%s", &aComment[0]);			// =の分
									fscanf(pFile, "%d", &InitInfo.nSpawnType);	// 出現の種類
								}

								if (strcmp(aComment, "CHARACTER") == 0)
								{// CHARACTERが来たら色の種類読み込み

									fscanf(pFile, "%s", &aComment[0]);		// =の分
									fscanf(pFile, "%d", &InitInfo.nChara);	// キャラの種類
								}
								
								if (strcmp(aComment, "POS") == 0)
								{// POSが来たら位置読み込み

									fscanf(pFile, "%s", &aComment[0]);		// =の分
									fscanf(pFile, "%f", &InitInfo.pos.x);	// X座標
									fscanf(pFile, "%f", &InitInfo.pos.y);	// Y座標
									fscanf(pFile, "%f", &InitInfo.pos.z);	// Z座標
								}

								if (strcmp(aComment, "ROT") == 0)
								{// ROTが来たら向き読み込み

									fscanf(pFile, "%s", &aComment[0]);		// =の分
									fscanf(pFile, "%f", &InitInfo.rot.x);	// X向き
									fscanf(pFile, "%f", &InitInfo.rot.y);	// Y向き
									fscanf(pFile, "%f", &InitInfo.rot.z);	// Z向き
								}

							}// END_ENEMYSETのかっこ

							// 読み込んだ情報を渡す
							//EnemyLoadInfo[nCntSpawnEnemy] = InitInfo;
							EnemyLoadInfo[nCntSpawnEnemy] = InitInfo;

							// 出現する数加算
							nCntSpawnEnemy++;
						}


						// 各パターンの設定
						if (strcmp(aComment, "NATURALWAVESET") == 0)
						{// 配置情報の読み込みを開始

							// 初期化
							//WaveLoadInfo.push_back(InitInfo);
							WaveLoadInfo.push_back(InitInfo);

							while (strcmp(aComment, "END_NATURALWAVESET") != 0)
							{// END_NATURALWAVESETが来るまで繰り返し

								fscanf(pFile, "%s", &aComment[0]);	//確認する

								if (strcmp(aComment, "TYPE") == 0)
								{// TYPEが来たらキャラファイル番号読み込み

									fscanf(pFile, "%s", &aComment[0]);	// =の分
									fscanf(pFile, "%d", &InitInfo.nType);	// 種類
								}

								if (strcmp(aComment, "BASE") == 0)
								{// BASEが来たら拠点番号読み込み

									fscanf(pFile, "%s", &aComment[0]);	// =の分
									fscanf(pFile, "%d", &InitInfo.nBase);	// 拠点
								}

								if (strcmp(aComment, "POS") == 0)
								{// POSが来たら位置読み込み

									fscanf(pFile, "%s", &aComment[0]);		// =の分
									fscanf(pFile, "%f", &InitInfo.pos.x);	// X座標
									fscanf(pFile, "%f", &InitInfo.pos.y);	// Y座標
									fscanf(pFile, "%f", &InitInfo.pos.z);	// Z座標
								}

								if (strcmp(aComment, "ROT") == 0)
								{// ROTが来たら向き読み込み

									fscanf(pFile, "%s", &aComment[0]);		// =の分
									fscanf(pFile, "%f", &InitInfo.rot.x);	// X向き
									fscanf(pFile, "%f", &InitInfo.rot.y);	// Y向き
									fscanf(pFile, "%f", &InitInfo.rot.z);	// Z向き
								}

							}// END_NATURALWAVESETのかっこ

							// 読み込んだ情報を渡す
							//WaveLoadInfo.push_back(InitInfo);
							WaveLoadInfo[nCntSpawnWave] = InitInfo;

							// 出現する数加算
							nCntSpawnWave++;
						}

					}// END_WAVESETのかっこ	

					// 読み込みの情報を渡す
					if (EnemyLoadInfo.size() != 0)
					{
						CGame::GetEnemyManager()->SetEnemyInfo(&EnemyLoadInfo[0], m_nNumStage, m_nNumWave[m_nNumStage], EnemyLoadInfo.size());
					}

					if (WaveLoadInfo.size() != 0)
					{
						CGame::GetObject3DMesh()->SetNaturalWaveInfo(&WaveLoadInfo[0], m_nNumStage, m_nNumWave[m_nNumStage], WaveLoadInfo.size());
					}

					// ウェーブの総数加算
					m_nNumWave[m_nNumStage]++;
				}
			}// END_STAGESETのかっこ

			// ステージの総数加算
			m_nNumStage++;
		}

		if (strcmp(aComment, "END_SCRIPT") == 0)
		{// 終了文字でループを抜ける
			break;
		}
	}

	// ファイルを閉じる
	fclose(pFile);

	return S_OK;
}

//==========================================================================
// 現在のウェーブ取得
//==========================================================================
int CWaveManager::GetNowWave(void)
{
	return m_nNowWave;
}

//==========================================================================
// 現在のステージ取得
//==========================================================================
int CWaveManager::GetNowStage(void)
{
	return m_nNowStage;
}

//==========================================================================
// 総ステージ数取得
//==========================================================================
int CWaveManager::GetMaxStage(void)
{
	return m_nNumStage;
}

//==========================================================================
// 現在のステージのウェーブ数取得
//==========================================================================
int CWaveManager::GetMaxWave(void)
{
	return m_nNumWave[m_nNowStage];
}

//==========================================================================
// リスポーンの規定値取得
//==========================================================================
int CWaveManager::GetRespawnThreshold(void)
{
	return m_nRespawnThreshold[m_nNowStage][m_nNowWave];
}
