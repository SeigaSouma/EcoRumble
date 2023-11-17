//=============================================================================
// 
//  起伏処理 [elevation_outside.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "elevation_outside.h"
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
#include "ranking.h"
#include "sound.h"

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

//==========================================================================
// コンストラクタ
//==========================================================================
CElevationOutside::CElevationOutside(int nPriority) : CElevation(nPriority)
{
	// 値のクリア
	m_nTexIdx = 0;	// テクスチャのインデックス番号
	m_postype = POSTYPE_UP;	// 位置の種類
}

//==========================================================================
// デストラクタ
//==========================================================================
CElevationOutside::~CElevationOutside()
{

}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CElevationOutside::Init(void)
{
	return E_FAIL;
}


//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CElevationOutside::Init(const char *pText)
{
	HRESULT hr;

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// テクスチャの割り当て
	m_nTexIdx = CManager::GetTexture()->Regist(TEXTURE);

	// テクスチャの割り当て
	BindTexture(m_nTexIdx);

	// 種類設定
	SetType(TYPE_ELEVATION_OUTSIDE);

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

	// 位置の種類
	if (strcmp(pText, "data\\TEXT\\elevation_outside_UP.txt") == 0)
	{
		m_postype = POSTYPE_UP;
	}
	else if (strcmp(pText, "data\\TEXT\\elevation_outside_Right.txt") == 0)
	{
		m_postype = POSTYPE_RIGHT;
	}
	else if (strcmp(pText, "data\\TEXT\\elevation_outside_Left.txt") == 0)
	{
		m_postype = POSTYPE_LEFT;
	}
	else if (strcmp(pText, "data\\TEXT\\elevation_outside_Down.txt") == 0)
	{
		m_postype = POSTYPE_DOWN;
	}

	// オブジェクト3Dメッシュの初期化処理
	hr = CObject3DMesh::Init(CObject3DMesh::TYPE_FIELD);

	if (FAILED(hr))
	{// 失敗していたら
		return E_FAIL;
	}

	int nStage = 0;
	switch (CManager::GetMode())
	{
	case CScene::MODE_RESULT:
		nStage = 3;
		break;

	case CScene::MODE_RANKING:
		nStage = CRanking::GetRandStage();
		break;

	default:
		nStage = 0;
		break;
	}

	for (int nCntHeight = 0; nCntHeight < m_aInfo.nHeightBlock + 1; nCntHeight++)
	{// 縦の分割分繰り返す

		for (int nCntWidth = 0; nCntWidth < m_aInfo.nWidthBlock + 1; nCntWidth++)
		{// 横の分割分繰り返す
			m_VtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y = m_aInfo.pStageVtxPos[nStage][nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y;
		}
	}
	

	// 頂点数分メモリ確保
	int nVtxNum = GetNumVertex();
	m_sFadeInfo.pRot = DEBUG_NEW float[nVtxNum];
	memset(&m_sFadeInfo.pRot[0], 0, sizeof(float) * nVtxNum);

	// 頂点情報設定
	//SetVtx();

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CElevationOutside::Uninit(void)
{

	// 終了処理
	CElevation::Uninit();
}

//==========================================================================
// 解放処理
//==========================================================================
void CElevationOutside::Release(void)
{
	// 終了処理
	CElevation::Release();
}

//==========================================================================
// 更新処理
//==========================================================================
void CElevationOutside::Update(void)
{
	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();

	if (m_postype == POSTYPE_LEFT)
	{
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
	}

	if (CManager::GetScene()->GetWaveManager()->IsStageFrag() == false)
	{// ステージ切り替え中だけ
		// 頂点情報設定
		SetVtx();
	}
}

//==========================================================================
// 描画処理
//==========================================================================
void CElevationOutside::Draw(void)
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
float CElevationOutside::GetHeight(D3DXVECTOR3& pos, D3DXVECTOR3& move, bool &bLand, CObject::TYPE type)
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

			if (nLeft >= GetNumVertex() || nRight >= GetNumVertex())
			{
				continue;
			}

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
// 頂点上げ下げ
//==========================================================================
void CElevationOutside::UPVtxField(D3DXVECTOR3 pos)
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

	// フィールドの位置
	D3DXVECTOR3 posfield = GetPosition();

	// 頂点情報の設定
	for (int nCntHeight = 0; nCntHeight < m_aInfo.nHeightBlock + 1; nCntHeight++)
	{//縦の頂点数分繰り返す

		for (int nCntWidth = 0; nCntWidth < m_aInfo.nWidthBlock + 1; nCntWidth++)
		{// 横の頂点数分繰り返す

			if (pInputKeyboard->GetTrigger(DIK_DELETE))
			{// deleteでリセット
				m_aInfo.pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y = 0.0f;
				m_VtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y = 0.0f;
			}

			float fNowLength =
				sqrtf((pos.x - (pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].x + posfield.x)) * (pos.x - (pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].x + posfield.x))
					+ (pos.z - (pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].z + posfield.z)) * (pos.z - (pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].z + posfield.z)));

			float Wariai = fNowLength / m_fBrushRange;

			if (Wariai > 1.0f)
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
				m_aInfo.pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y = 0.0f;
			}

			if ((m_aInfo.nWidthBlock + 1) * (m_aInfo.nHeightBlock + 1) - 1 == nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1)))
			{
				int n = 0;
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
// 頂点情報設定処理
//==========================================================================
void CElevationOutside::SetVtx(void)
{
	// 頂点情報更新
	CElevation::SetVtx();
}

//==========================================================================
// ロード処理
//==========================================================================
HRESULT CElevationOutside::Load(const char *pText)
{
	char aComment[mylib_const::MAX_STRING] = {};	//コメント用

	FILE *pFile = NULL;	// ファイルポインタを宣言

	// ファイルを開く
	pFile = fopen(pText, "r");

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
void CElevationOutside::Save(void)
{
	FILE *pFile = NULL;	// ファイルポインタを宣言

	// ファイルを開く
	switch (m_postype)
	{
	case CElevation::POSTYPE_UP:
		pFile = fopen("data\\TEXT\\elevation_outside_save_UP.txt", "w");
		break;
	case CElevation::POSTYPE_DOWN:
		pFile = fopen("data\\TEXT\\elevation_outside_save_DOWN.txt", "w");
		break;
	case CElevation::POSTYPE_RIGHT:
		pFile = fopen("data\\TEXT\\elevation_outside_save_RIGHT.txt", "w");
		break;
	case CElevation::POSTYPE_LEFT:
		pFile = fopen("data\\TEXT\\elevation_outside_save_LEFT.txt", "w");
		break;
	case CElevation::POSTYPE_MAX:
		break;
	default:
		break;
	}

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
		"\tVTXPOS\n\t", m_aInfo.pos.x, m_aInfo.pos.y, m_aInfo.pos.z,
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
		fprintf(pFile, "\n\t");
	}
	fprintf(pFile, "\n\tEND_VTXPOS\n");

	fprintf(pFile, "\nEND_FIELDSET\n");
	fprintf(pFile, "\nEND_SCRIPT		# この行は絶対消さないこと！");

	//ファイルを閉じる
	fclose(pFile);
}

//==========================================================================
// 起伏の情報取得
//==========================================================================
CElevationOutside *CElevationOutside::GetElevation(void)
{
	return this;
}
