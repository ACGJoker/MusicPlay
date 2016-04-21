#include "stdafx.h"
#include "MusicPlay.h"

CMusicPlay::CMusicPlay()
: m_fStatus(STOP)
, m_strCurMusicPath(_T(""))
, m_wDeviceID(NULL)
, mciError(0)
, m_dwVolumn(100)
{
}

CMusicPlay::~CMusicPlay()
{
}

void CMusicPlay::setNotifyHwnd(HWND hNotifyWindow)
{
	m_hNotifyWindow = hNotifyWindow;
}

int CMusicPlay::OpenFile(CString strFile)
{
	if (m_wDeviceID)
	{
		CloseFile();
	}

	MCI_OPEN_PARMS mciOpen;
	mciOpen.lpstrDeviceType = _T("mpegvideo");
	mciOpen.lpstrElementName = strFile;
	mciError = mciSendCommand(NULL, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT, (DWORD)&mciOpen);
	if (0 != mciError)
	{
		m_wDeviceID = NULL;
		return mciError;
	}
	m_wDeviceID = mciOpen.wDeviceID;
	DWORD i = GetCurrentThreadId();
	return mciError;
}

int CMusicPlay::play(CString strFile)
{

	if (0 != OpenFile(strFile))
	{
		return mciError;
	}
	SetVolumn(m_dwVolumn);//设置音量
	MCI_PLAY_PARMS mciPlay;
	if (m_hNotifyWindow)
		mciPlay.dwCallback = (DWORD)m_hNotifyWindow;
	mciError = mciSendCommand(m_wDeviceID, MCI_PLAY, MCI_NOTIFY, (DWORD)&mciPlay);
	if (0 != mciError)
	{
		return mciError;
	}
	m_fStatus = PLAY;
	m_strCurMusicPath = strFile;
	return 0;
}

BOOL CMusicPlay::Pause()
{
	if (0 != mciSendCommand(m_wDeviceID, MCI_PAUSE, NULL, NULL))
	{
		return FALSE;
	}
	m_fStatus = PAUSE;
	return TRUE;
}

BOOL CMusicPlay::Resume()
{
	if (0 != mciSendCommand(m_wDeviceID, MCI_RESUME, NULL, NULL))
	{
		return FALSE;
	}
	m_fStatus = PLAY;
	return TRUE;
}

void CMusicPlay::Stop()
{
	mciSendCommand(m_wDeviceID, MCI_STOP, NULL, NULL);
	mciSendCommand(m_wDeviceID, MCI_SEEK, MCI_SEEK_TO_START, NULL);

	m_fStatus = STOP;
	return;
}

int CMusicPlay::GetStatus()
{
	return m_fStatus;
}

CString CMusicPlay::GetMusicPath()
{
	return m_strCurMusicPath;
}

DWORD CMusicPlay::GetTotalTime()
{
	MCI_STATUS_PARMS mciStatus;
	mciStatus.dwItem = MCI_STATUS_LENGTH;
	mciSendCommand(m_wDeviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)(LPVOID)&mciStatus);
	return mciStatus.dwReturn;
}

DWORD CMusicPlay::GetTimePostion()
{
	MCI_STATUS_PARMS mciStatus;
	mciStatus.dwItem = MCI_STATUS_POSITION;
	mciSendCommand(m_wDeviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)(LPVOID)&mciStatus);//关键,取得位置
	return mciStatus.dwReturn;
}

void CMusicPlay::CloseFile()
{
	mciSendCommand(m_wDeviceID, MCI_CLOSE, 0, NULL);
}

void CMusicPlay::PlayFrom(DWORD dwTime)
{
	MCI_PLAY_PARMS mciPlay;
	mciPlay.dwFrom = dwTime;
	mciError = mciSendCommand(m_wDeviceID, MCI_PLAY, MCI_FROM, (DWORD)(LPVOID)&mciPlay);
}

DWORD CMusicPlay::GetVolumn()
{
	MCI_STATUS_PARMS mciGetVolumn;
	mciGetVolumn.dwItem = MCI_DGV_STATUS_VOLUME;
	mciSendCommand(m_wDeviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)(LPVOID)&mciGetVolumn);
	return mciGetVolumn.dwReturn / 10;
}

BOOL CMusicPlay::SetVolumn(DWORD dwVol)
{
	MCI_DGV_SETAUDIO_PARMS mciSetVolumn;
	mciSetVolumn.dwItem = MCI_DGV_SETAUDIO_VOLUME;
	mciSetVolumn.dwValue = dwVol * 10;
	mciError = mciSendCommand(m_wDeviceID, MCI_SETAUDIO, MCI_DGV_SETAUDIO_VALUE | MCI_DGV_SETAUDIO_ITEM, (DWORD)(LPVOID)&mciSetVolumn);
	if (0 != mciError)
	{
		return FALSE; 
	}
	m_dwVolumn = dwVol;
	return TRUE;
}
