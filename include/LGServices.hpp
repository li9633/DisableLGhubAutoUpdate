#pragma once
#include <string>
#include <Windows.h>
#include <iostream>
#include <winerror.h>
class LGServices
{
public:
	LGServices();

	~LGServices();

	//服务控制管理器句柄
	SC_HANDLE SCM_Handle;
	//LGHub服务句柄
	SC_HANDLE SVC_Handle;
	//关闭服务控制管理器句柄的状态
	bool Close_SCMH_State;
	//关闭LGhub服务句柄的状态
	bool Close_SVCH_State;
	//更改服务启动类型的状态
	bool IsChangeSuccess;
	//全局错误代码
	DWORD Global_Error_Code;
	//服务所在路径
	std::string LGhub_Directory;
	std::string LGhub_Updater_Position;
	std::string LGhub_agent_Position;

	size_t Directory_Index;
};
