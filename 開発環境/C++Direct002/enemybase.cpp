//=============================================================================
// 
//  敵拠点処理 [enemybase.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "enemybase.h"
#include "calculation.h"
#include "manager.h"
#include "game.h"
#include "renderer.h"

//==========================================================================
// マクロ定義
//==========================================================================

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
std::vector<CEnemyBase::BaseData> CEnemyBase::m_aBaseData[mylib_const::MAX_STAGE] = {};	// 読み込みデータ
int CEnemyBase::m_nNumAll[mylib_const::MAX_STAGE] = {};		// 最大数
bool CEnemyBase::m_bLoad = false;	// 情報読み込み判定

//==========================================================================
// コンストラクタ
//==========================================================================
CEnemyBase::CEnemyBase()
{
	// 値のクリア
}

//==========================================================================
// デストラクタ
//==========================================================================
CEnemyBase::~CEnemyBase()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CEnemyBase *CEnemyBase::Create(const std::string pTextFile)
{
	// 生成用のオブジェクト
	CEnemyBase *pEnemyBase = NULL;

	if (pEnemyBase == NULL)
	{// NULLだったら

		// メモリの確保
		pEnemyBase = DEBUG_NEW CEnemyBase;

		if (pEnemyBase != NULL)
		{// メモリの確保が出来ていたら

			// 初期化処理
			HRESULT hr = pEnemyBase->Init(pTextFile);

			if (FAILED(hr))
			{// 失敗していたら
				return NULL;
			}
		}
		return pEnemyBase;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CEnemyBase::Init(const std::string pTextFile)
{
	// 初期化処理
	HRESULT hr = ReadText(pTextFile);

	if (FAILED(hr))
	{// 失敗していたら
		return NULL;
	}

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CEnemyBase::Uninit(void)
{
	
}

//==========================================================================
// 更新処理
//==========================================================================
void CEnemyBase::Update(void)
{
	
}

//==========================================================================
// テキスト読み込み処理
//==========================================================================
HRESULT CEnemyBase::ReadText(const std::string pTextFile)
{
	// ロードチェック
	if (m_bLoad == true)
	{// 既に読み込まれていたら
		return S_OK;
	}

	FILE *pFile = NULL;	// ファイルポインタを宣言

	// ファイルを開く
	pFile = fopen(pTextFile.c_str(), "r");

	if (pFile == NULL)
	{// ファイルが開けなかった場合
		return E_FAIL;
	}

	// 読み込み判定ON
	m_bLoad = true;

	char aComment[MAX_COMMENT];	// コメント
	BaseData InitBase = {};		// 初期化用
	int nCntStage = 0;			// ステージの数

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
				if (strcmp(aComment, "BASESET") == 0)
				{// 配置情報の読み込みを開始

					// 初期化＆追加
					m_aBaseData[nCntStage].push_back(InitBase);

					while (strcmp(aComment, "END_BASESET") != 0)
					{// END_BASESETが来るまで繰り返し

						// 文字列の読み込み
						fscanf(pFile, "%s", &aComment[0]);

						if (strcmp(aComment, "POS") == 0)
						{// POSが来たら位置読み込み

							fscanf(pFile, "%s", &aComment[0]);					// =の分
							fscanf(pFile, "%f", &m_aBaseData[nCntStage][m_nNumAll[nCntStage]].pos.x);	// X座標
							fscanf(pFile, "%f", &m_aBaseData[nCntStage][m_nNumAll[nCntStage]].pos.y);	// Y座標
							fscanf(pFile, "%f", &m_aBaseData[nCntStage][m_nNumAll[nCntStage]].pos.z);	// Z座標
						}

						if (strcmp(&aComment[0], "ROT") == 0)
						{// ROTが来たら向き読み込み

							fscanf(pFile, "%s", &aComment[0]);	// =の分
							fscanf(pFile, "%f", &m_aBaseData[nCntStage][m_nNumAll[nCntStage]].rot.x);	// Xの向き
							fscanf(pFile, "%f", &m_aBaseData[nCntStage][m_nNumAll[nCntStage]].rot.y);	// Yの向き
							fscanf(pFile, "%f", &m_aBaseData[nCntStage][m_nNumAll[nCntStage]].rot.z);	// Zの向き
						}

					}// END_BASESETのかっこ

					// 総数加算
					m_nNumAll[nCntStage]++;
				}
			}// END_STAGESETのかっこ
			nCntStage++;
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
// 敵拠点データ取得
//==========================================================================
CEnemyBase::BaseData *CEnemyBase::GetBaseData(int nCntStage)
{
	return &m_aBaseData[nCntStage][0];
}
