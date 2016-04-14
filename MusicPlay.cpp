#include "stdafx.h"
#include "MusicPlay.h"


CMusicPlay::CMusicPlay()
: m_fStatus(STOP)
, m_strCurMusicPath(_T(""))
, m_wDeviceID(NULL)
, mciError(0)
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
	return mciError;
}

int CMusicPlay::play(CString strFile)
{
	if (m_wDeviceID)
	{
		CloseFile();
	}

	if (0 != OpenFile(strFile))
	{
		return mciError;
	}
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
	MCI_STATUS_PARMS mciStatusParms;
	mciStatusParms.dwItem = MCI_STATUS_LENGTH; 
	mciSendCommand(m_wDeviceID, MCI_STATUS, MCI_WAIT | MCI_STATUS_ITEM, (DWORD)(LPVOID)&mciStatusParms);
	return mciStatusParms.dwReturn;
}

DWORD CMusicPlay::GetTimePostion()
{
	MCI_STATUS_PARMS mciStatusParms;
	mciStatusParms.dwItem = MCI_STATUS_POSITION;
	mciSendCommand(m_wDeviceID, MCI_STATUS, MCI_NOTIFY | MCI_STATUS_ITEM, (DWORD)(LPVOID)&mciStatusParms);//关键,取得位置
	return mciStatusParms.dwReturn;
}

void CMusicPlay::CloseFile()
{
	mciSendCommand(m_wDeviceID, MCI_CLOSE, NULL, NULL);
}

void CMusicPlay::PlayFrom(DWORD dwTime)
{
	MCI_PLAY_PARMS PlayParms;
	PlayParms.dwFrom = dwTime;
	mciError = mciSendCommand(m_wDeviceID, MCI_PLAY, MCI_FROM, (DWORD)(LPVOID)&PlayParms);
	int i = 0;

}
