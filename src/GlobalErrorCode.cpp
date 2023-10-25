#include "../include/GlobalErrorCode.hpp"
//显示错误详细信息
void DisplayErrorText(DWORD err)
{
	LPSTR lpbuffer = NULL;
	FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, LANG_SYSTEM_DEFAULT, (LPSTR)&lpbuffer, 0, NULL);

	std::cout << lpbuffer << std::endl;
	LocalFree(lpbuffer);
}