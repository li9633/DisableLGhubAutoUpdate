#include "../include/LGServices.hpp"
#include "../include/GlobalErrorCode.hpp"

LGServices::LGServices()
{
	SCM_Handle = NULL;
	SVC_Handle = NULL;
	Close_SCMH_State = NULL;
	Close_SVCH_State = NULL;
	IsChangeSuccess = NULL;
	Global_Error_Code = NULL;
	Directory_Index = NULL;
}

LGServices::~LGServices()
{
	if (!(LGServices::Global_Error_Code))
	{
		Close_SCMH_State = CloseServiceHandle(LGServices::SCM_Handle);	//关闭SCM句柄

	}
	if (!(LGServices::Global_Error_Code))
	{
		Close_SVCH_State = CloseServiceHandle(LGServices::SVC_Handle);	//关闭服务句柄
	}

	if (LGServices::Directory_Index == (size_t)-1)
	{
		std::cout << "ErrorCode = " << -1 << std::endl;
		std::cout << "无法定位安装路径，请检查是否正确安装LGhub！" << std::endl;
		return;
	}


	if (!(Close_SCMH_State || Close_SVCH_State))
	{
		LGServices::Global_Error_Code = GetLastError();
		std::cout << "ErrorCode = " << LGServices::Global_Error_Code << std::endl;
	}

	DisplayErrorText(LGServices::Global_Error_Code);

}
