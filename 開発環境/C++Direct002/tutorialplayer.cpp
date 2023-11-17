//=============================================================================
// 
//  チュートリアルプレイヤー処理 [tutorialplayer.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "tutorial.h"
#include "tutorialplayer.h"
#include "tutorialstep.h"
#include "camera.h"
#include "manager.h"
#include "debugproc.h"
#include "renderer.h"
#include "input.h"
#include "motion.h"
#include "sound.h"
#include "particle.h"
#include "grassparticle.h"
#include "scene.h"
#include "wavemanager.h"
#include "calculation.h"

//==========================================================================
// マクロ定義
//==========================================================================

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================

//==========================================================================
// コンストラクタ
//==========================================================================
CTutorialPlayer::CTutorialPlayer(int nPriority) : CPlayer(nPriority)
{
	// 値のクリア
	
}

//==========================================================================
// デストラクタ
//==========================================================================
CTutorialPlayer::~CTutorialPlayer()
{

}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CTutorialPlayer::Init(void)
{
	// 親の初期化処理
	CPlayer::Init();

	return S_OK;
}

//==========================================================================
// モード別終了処理
//==========================================================================
void CTutorialPlayer::UninitByMode(void)
{

	// プレイヤー情報
	//CPlayer **ppPlayer = CTutorial::GetPlayer(0);
	CScene *pScene = CManager::GetScene();
	if (pScene != NULL)
	{
		CPlayer **ppPlayer = CManager::GetScene()->GetPlayer(0);

		// プレイヤーをNULL
		*ppPlayer = NULL;
	}
}

//==========================================================================
// 更新処理
//==========================================================================
void CTutorialPlayer::Update(void)
{
	// 死亡の判定
	if (IsDeath() == true)
	{// 死亡フラグが立っていたら
		return;
	}

	// 親の更新処理
	CPlayer::Update();

	// 死亡の判定
	if (IsDeath() == true)
	{// 死亡フラグが立っていたら
		return;
	}

	// ステップごとの更新
	UpdateByStep();
}

//==========================================================================
// ステップごとの更新処理
//==========================================================================
void CTutorialPlayer::UpdateByStep(void)
{
	// ステップの設定
	//CTutorial::GetStep()->SetStep(CTutorialStep::STEPFRAG_SPEEDUP);
}


//==========================================================================
// 操作処理
//==========================================================================
void CTutorialPlayer::Controll(void)
{
	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();

	// ゲームパッド情報取得
	CInputGamepad *pInputGamepad = CManager::GetInputGamepad();

	// カメラの情報取得
	CCamera *pCamera = CManager::GetCamera();

	// カメラの向き取得
	D3DXVECTOR3 Camerarot = pCamera->GetRotation();

	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// 移動量取得
	D3DXVECTOR3 move = GetMove();

	// 向き取得
	D3DXVECTOR3 rot = GetRotation();

	// 目標の向き取得
	float fRotDest = GetRotDest();

	// 現在の種類取得
	int nType = m_pMotion->GetType();

	// 移動量取得
	float fMove = GetVelocity();

	// 加速の割合取得
	m_fMoveRatio = pInputGamepad->GetStickPositionRatioL(0).y;

	if (m_fMoveRatio > 0.85f)
	{
		CTutorial::GetStep()->SetStep(CTutorialStep::STEP_SPEEDUP);
	}

	if (m_fMoveRatio < -0.85f)
	{
		CTutorial::GetStep()->SetStep(CTutorialStep::STEP_SPEEDDOWN);
	}

	static int nWalk = 0;
	if (nType == MOTION_WALK)
	{// 歩行中

		if (m_fMoveSlideDest >= SLIDEMOVE * 0.35f)
		{// 加速中
			m_nCntWalk = (m_nCntWalk + 1) % 2;
		}
		else if (m_fMoveSlideDest < 0.0f)
		{
			m_nCntWalk = (m_nCntWalk + 1) % 30;
		}
		else
		{
			m_nCntWalk = (m_nCntWalk + 1) % 25;
		}

		nWalk = (nWalk + 1) % 25;
		if (nWalk == 0)
		{
			// 歩行音再生
			CManager::GetSound()->PlaySound(CSound::LABEL_SE_WALK);
		}

		if (m_nCntWalk == 0)
		{
			my_particle::Create(pos, my_particle::TYPE_MOVESMOKE);
			CGrassParticle::Create(pos, rot, D3DXVECTOR2(3.5f, 3.0f), 10);
		}
	}
	else
	{
		m_nCntWalk = 0;
	}


	if (m_pMotion->IsGetMove(nType) == 1 && m_state != STATE_CONFUSION && CTutorial::GetStep()->IsEndStep() == false)
	{// 移動可能モーションの時 && 混乱じゃないとき

#ifndef AUTO_MOVE
		// 移動中にする
		m_bMove = true;
#endif

		//*************************************************
		// 向きの変更
		//*************************************************
		// スティックの向き取得
		float fStickRot = pInputGamepad->GetStickRot(0);

		if (fStickRot > D3DX_PI * 0.75f || fStickRot < -D3DX_PI * 0.75f)
		{// 真後ろは無し
			fStickRot = 0.0f;
		}

		fRotDest = D3DX_PI + fStickRot + Camerarot.y;

		// 目標の向き設定
		SetRotDest(fRotDest);

		if (pInputKeyboard->GetPress(DIK_W) == true || pInputGamepad->GetStickMoveL(0).y > 0 ||
			pInputKeyboard->GetPress(DIK_A) == true || pInputGamepad->GetStickMoveL(0).x < 0 ||
			pInputKeyboard->GetPress(DIK_S) == true || pInputGamepad->GetStickMoveL(0).y < 0 ||
			pInputKeyboard->GetPress(DIK_D) == true || pInputGamepad->GetStickMoveL(0).x > 0 ||
			pInputKeyboard->GetPress(DIK_O) == true || pInputGamepad->GetPress(CInputGamepad::BUTTON_Y, 0) == true ||
			pInputKeyboard->GetPress(DIK_RETURN) == true || pInputGamepad->GetPress(CInputGamepad::BUTTON_A, 0) == true)
		{//なにかキーが押された

			// 行動してる設定にする
			CTutorial::SetEnableMovingPlayer();
		}
		else
		{
			// 行動してない設定にする
			CTutorial::SetDesableMovingPlayer();
		}

		if (pInputKeyboard->GetPress(DIK_A) == true || pInputGamepad->GetStickMoveL(0).x < 0)
		{//←キーが押された,左移動

			if (pInputKeyboard->GetPress(DIK_W) == true || pInputGamepad->GetStickMoveL(0).y > 0)
			{//A+W,左上移動
				fRotDest = D3DX_PI * 0.75f + Camerarot.y;
			}
			else if (pInputKeyboard->GetPress(DIK_S) == true || pInputGamepad->GetStickMoveL(0).y < 0)
			{//A+S,左下移動
				fRotDest = D3DX_PI * 0.25f + Camerarot.y;
			}
			else
			{//A,左移動
				fRotDest = D3DX_PI * 0.5f + Camerarot.y;
			}
		}
		else if (pInputKeyboard->GetPress(DIK_D) == true || pInputGamepad->GetStickMoveL(0).x > 0)
		{//Dキーが押された,右移動

			if (pInputKeyboard->GetPress(DIK_W) == true || pInputGamepad->GetStickMoveL(0).y > 0)
			{//D+W,右上移動
				fRotDest = -D3DX_PI * 0.75f + Camerarot.y;
			}
			else if (pInputKeyboard->GetPress(DIK_S) == true || pInputGamepad->GetStickMoveL(0).y < 0)
			{//D+S,右下移動
				fRotDest = -D3DX_PI * 0.25f + Camerarot.y;
			}
			else
			{//D,右移動
				fRotDest = -D3DX_PI * 0.5f + Camerarot.y;
			}
		}

		// 加速度設定
		if (pInputKeyboard->GetPress(DIK_W) == true)
		{// 上移動

			if (pInputKeyboard->GetPress(DIK_A) == true || pInputKeyboard->GetPress(DIK_D) == true)
			{// A or D
				m_fMoveRatio = 0.5f;
				CTutorial::GetStep()->SetStep(CTutorialStep::STEP_SPEEDUP);
			}
			else
			{
				m_fMoveRatio = 1.0f;
				CTutorial::GetStep()->SetStep(CTutorialStep::STEP_SPEEDUP);
			}
		}
		else if (pInputKeyboard->GetPress(DIK_S) == true)
		{// 下移動

			if (pInputKeyboard->GetPress(DIK_A) == true || pInputKeyboard->GetPress(DIK_D) == true)
			{// A or D
				m_fMoveRatio = -0.5f;
				CTutorial::GetStep()->SetStep(CTutorialStep::STEP_SPEEDDOWN);
			}
			else
			{
				m_fMoveRatio = -1.0f;
				CTutorial::GetStep()->SetStep(CTutorialStep::STEP_SPEEDDOWN);
			}
		}
	}
	else if (m_state != STATE_CONFUSION)
	{// 移動可能モーションじゃない場合
#if 1
		if (pInputKeyboard->GetPress(DIK_A) == true || pInputGamepad->GetStickMoveL(0).x < 0)
		{//←キーが押された,左移動

			if (pInputKeyboard->GetPress(DIK_W) == true || pInputGamepad->GetStickMoveL(0).y > 0)
			{//A+W,左上移動
				fRotDest = D3DX_PI * 0.75f + Camerarot.y;
			}
			else if (pInputKeyboard->GetPress(DIK_S) == true || pInputGamepad->GetStickMoveL(0).y < 0)
			{//A+S,左下移動
				fRotDest = D3DX_PI * 0.25f + Camerarot.y;
			}
			else
			{//A,左移動
				fRotDest = D3DX_PI * 0.5f + Camerarot.y;
			}
		}
		else if (pInputKeyboard->GetPress(DIK_D) == true || pInputGamepad->GetStickMoveL(0).x > 0)
		{//Dキーが押された,右移動

			if (pInputKeyboard->GetPress(DIK_W) == true || pInputGamepad->GetStickMoveL(0).y > 0)
			{//D+W,右上移動
				fRotDest = -D3DX_PI * 0.75f + Camerarot.y;
			}
			else if (pInputKeyboard->GetPress(DIK_S) == true || pInputGamepad->GetStickMoveL(0).y < 0)
			{//D+S,右下移動
				fRotDest = -D3DX_PI * 0.25f + Camerarot.y;
			}
			else
			{//D,右移動
				fRotDest = -D3DX_PI * 0.5f + Camerarot.y;
			}
		}
#endif
	}

	// 移動の割合で移動種類変える
	if (m_fMoveRatio >= 0.5f)
	{
		// 高速移動
		m_walkType = WALKTYPE_SPEED;
	}
	else if (m_fMoveRatio <= -0.75f)
	{
		// 遅い移動
		m_walkType = WALKTYPE_SLOW;
	}
	else
	{
		// 通常移動
		m_walkType = WALKTYPE_NORMAL;
	}

	// 移動量設定
#ifndef AUTO_MOVE
	fMove = fMove + ((fMove * 0.5f) * m_fMoveRatio);
#else
	fMove = fMove + ((fMove * 0.5f) * 1.0f);
	fMove *= 1.5f;
#endif

	if (m_state == STATE_DMG || m_state == STATE_KNOCKBACK)
	{// 吹っ飛び状態のとき
		fRotDest = rot.y;
	}

	//角度の正規化
	RotNormalize(fRotDest);

	//現在と目標の差分を求める
	float fRotDiff = fRotDest - rot.y;

	//角度の正規化
	RotNormalize(fRotDiff);

	//角度の補正をする
	if (m_state != STATE_CONFUSION)
	{// 吹っ飛び状態のとき
		rot.y += fRotDiff * 0.014f;
	}

	//角度の正規化
	RotNormalize(rot.y);

	// スライド
	m_fMoveSlideDest = CScene::GetObject3DMesh()->Slide(pos);
	m_fMoveSlideDest *= SLIDEMOVE;

	m_fMoveSlide += (m_fMoveSlideDest - m_fMoveSlide) * 0.1f;

#ifndef AUTO_MOVE
	if (CTutorial::GetStep()->IsEndStep() == false)
	{// 終了時は一時停止

		if (m_state != STATE_KNOCKBACK && m_state != STATE_DMG && m_state != STATE_CONFUSION)
		{
			move.x += sinf(D3DX_PI + rot.y) * (fMove + m_fMoveSlide);
			move.z += cosf(D3DX_PI + rot.y) * (fMove + m_fMoveSlide);
		}
	}
#endif

	// 重力処理
	if (m_state != STATE_KNOCKBACK && m_state != STATE_DMG)
	{
		move.y -= mylib_const::GRAVITY;
	}

	// 位置更新
	pos += move;

	// 慣性補正
	if (m_state != STATE_KNOCKBACK && m_state != STATE_DMG)
	{
		move.x += (0.0f - move.x) * 0.25f;
		move.z += (0.0f - move.z) * 0.25f;
	}

	// 位置設定
	SetPosition(pos);

	// 移動量設定
	SetMove(move);

	// 向き設定
	SetRotation(rot);

	// 目標の向き設定
	SetRotDest(fRotDest);

	//**********************************
	// 当たり判定
	//**********************************
	Collision();

#if _DEBUG
	if (pInputKeyboard->GetPress(DIK_H) == true)
	{//←キーが押された,左移動

		// 攻撃タイプ変更
		CTutorial::GetStep()->SetStep(CTutorial::GetStep()->GetNowStep());
		//CTutorial::GetStep()->AddStep();
	}
#endif

	if (m_state != STATE_ATTACK && m_state != STATE_CONFUSION && m_state != STATE_DMG && m_state != STATE_KNOCKBACK)
	{
		if (CTutorial::GetStep()->GetNowStep() >= CTutorialStep::STEP_DIRECTWAVE&&
			(pInputKeyboard->GetTrigger(DIK_O) == true || pInputGamepad->GetTrigger(CInputGamepad::BUTTON_Y, 0) == true))
		{//←キーが押された,左移動

			// 攻撃タイプ変更
			m_Shotmode = ATK_DIRECT;
			m_bATK = true;		// 攻撃判定ON
		}

		if (CTutorial::GetStep()->GetNowStep() >= CTutorialStep::STEP_IMPACTWAVE &&
			(pInputKeyboard->GetTrigger(DIK_RETURN) == true || pInputGamepad->GetTrigger(CInputGamepad::BUTTON_A, 0) == true) &&
			m_bATK == false && nType != MOTION_DIRECTATK && nType != MOTION_SUPERATK)
		{// 攻撃

			// 攻撃タイプ変更
			m_Shotmode = ATK_DEBUG_POWER;
			m_bATK = true;		// 攻撃判定ON

			// 目標の距離設定
			pCamera->SetLenDest(pCamera->GetOriginDistance() + 200.0f);
		}
	}
}