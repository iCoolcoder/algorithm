// Bird.cpp : Implementation of CBird

#include "stdafx.h"
#include "Bird.h"


// CBird

HRESULT CBird::Activate()
{
	HRESULT hr = GetObjectContext(&m_spObjectContext);
	if (SUCCEEDED(hr))
		return S_OK;
	return hr;
} 

BOOL CBird::CanBePooled()
{
	return FALSE;
} 

void CBird::Deactivate()
{
	m_spObjectContext.Release();
} 


STDMETHODIMP CBird::Fly(LONG* lSpeed)
{
	// TODO: Add your implementation code here
	if (NULL == _watcher)
	{
		_watcher = new Watcher();
		_watcher->Start();
	}
	*lSpeed = 0xbee;
	return S_OK;
}

STDMETHODIMP CBird::Init()
{
	if (NULL == _watcher)
	{
		_watcher = new Watcher();
		_watcher->Start();
	}
	return S_OK;
}
