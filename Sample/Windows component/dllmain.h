// dllmain.h : Declaration of module class.

class CcomponentModule : public CAtlDllModuleT< CcomponentModule >
{
public :
	DECLARE_LIBID(LIBID_componentLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_COMPONENT, "{0182F8FA-683F-4716-8614-61336FDBE158}")
};

extern class CcomponentModule _AtlModule;
