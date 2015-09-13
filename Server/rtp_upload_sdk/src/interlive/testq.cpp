#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <tchar.h>
#include <minwindef.h>
#endif
#include <stdio.h>
#include "rtp_upload_sdk.h"


int main()
{
    printf("end\n");
    return 0;
}

//int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
//    _In_opt_ HINSTANCE hPrevInstance,
//    _In_ LPTSTR    lpCmdLine,
//    _In_ int       nCmdShow)
//{
//    rtp_upload_sdk_init("10.10.69.127", "7986", "8787");
//    return 0;
//}
