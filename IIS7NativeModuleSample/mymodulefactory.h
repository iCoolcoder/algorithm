#include "GlobalVariable.h"

#ifndef __MODULE_FACTORY_H__
#define __MODULE_FACTORY_H__

// Factory class for CMyHttpModule.
// This class is responsible for creating instances
// of CMyHttpModule for each request.
class CMyHttpModuleFactory : public IHttpModuleFactory
{
public:
    virtual
    HRESULT
    GetHttpModule(
        OUT CHttpModule            **ppModule, 
        IN IModuleAllocator        *
    )
    {
        HRESULT                    hr = S_OK;
        CMyHttpModule *            pModule = NULL;

		if ( !InitializeMe() )
		{
			hr = E_FAIL;
			goto Finished;
		}

	    if ( ppModule == NULL )
        {
            hr = HRESULT_FROM_WIN32( ERROR_INVALID_PARAMETER );
            goto Finished;
        }

        pModule = new CMyHttpModule();
        if ( pModule == NULL )
        {
            hr = HRESULT_FROM_WIN32( ERROR_NOT_ENOUGH_MEMORY );
            goto Finished;
        }

        *ppModule = pModule;
        pModule = NULL;
            
    Finished:

        if ( pModule != NULL )
        {
            delete pModule;
            pModule = NULL;
        }

        return hr;
    }

	bool InitializeMe()
	{
		if (GlobalVariable::initialized == false)
		{
			GlobalVariable::sessionMgr = new SessionManager();
			GlobalVariable::initialized = true;
		}

		return true;
	}

    virtual 
    void
    Terminate()
    {
        delete this;
    }
};

#endif
