// Bird.h : Declaration of the CBird

#pragma once
#include "component_i.h"
#include "resource.h"       // main symbols
#include <comsvcs.h>
#include <mtxattr.h>
#include "Watcher.h"



// CBird

class ATL_NO_VTABLE CBird :
	public CComObjectRootEx<CComMultiThreadModel>,
	public IObjectControl,
	public IObjectConstruct,
	public CComCoClass<CBird, &CLSID_Bird>,
	public IDispatchImpl<IBird, &IID_IBird, &LIBID_componentLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CBird()
	{
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_BIRD)

BEGIN_COM_MAP(CBird)
	COM_INTERFACE_ENTRY(IBird)
	COM_INTERFACE_ENTRY(IObjectControl)
	COM_INTERFACE_ENTRY(IObjectConstruct)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()



// IObjectControl
public:
	STDMETHOD(Activate)();
	STDMETHOD_(BOOL, CanBePooled)();
	STDMETHOD_(void, Deactivate)();

	CComPtr<IObjectContext> m_spObjectContext;

	STDMETHOD(Construct)(IDispatch * pCtorObj)
	{
		CComPtr<IObjectConstructString> spObjectConstructString;
		HRESULT hr = pCtorObj->QueryInterface(&spObjectConstructString);
		if (SUCCEEDED(hr))
		{
			CComBSTR bstrConstruct;
			hr = spObjectConstructString->get_ConstructString(&bstrConstruct);
			if (SUCCEEDED(hr))
			{
				// TODO : Use the string to construct the object.
			}
		}
		return hr;
	}

// IBird
public:
	STDMETHOD(Fly)(LONG* lSpeed);
	STDMETHOD(Init)();
private:
	Watcher *_watcher;
};

OBJECT_ENTRY_AUTO(__uuidof(Bird), CBird)
