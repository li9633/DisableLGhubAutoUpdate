#define _CRT_SECURE_NO_WARNINGS 1
#include "../include/ChangeLGServices.hpp"
using namespace std;

size_t Get_LGhub_Directory_Index(string str)
{
	static const basic_string <char>::size_type npos = -1;
	basic_string <char>::size_type IndexChar;
	IndexChar = str.find_last_of("\\", str.size());
	if (IndexChar == npos)
	{
		cout << "无法定位字符串" << endl;
		return ERROR_CANNOT_FIND;
	}
	//cout << IndexChar << endl;
	return IndexChar;

}

//更改LGhub服务启动类型为手动
void Change_LG_service(LGServices* LGS)
{
	//获取句柄
	LGS->SCM_Handle = OpenSCManagerA(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!(LGS->SCM_Handle))
	{
		LGS->Global_Error_Code = GetLastError();
		return;
	}
	

	//打开服务
	LGS->SVC_Handle = OpenServiceA(LGS->SCM_Handle, "LGHUBUpdaterService", SC_MANAGER_ALL_ACCESS);
	if (!(LGS->SVC_Handle))
	{
		LGS->Global_Error_Code = GetLastError();
		return;
	}
	cout << "正在更改\'LGHUB Updater Service \' 启动类型为手动" << endl;
	//更改启动类型为 手动
	LGS->IsChangeSuccess = ChangeServiceConfigA(LGS->SVC_Handle, SERVICE_NO_CHANGE, SERVICE_DEMAND_START, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	if (!(LGS->IsChangeSuccess))
	{
		LGS->Global_Error_Code = GetLastError();
		return;
	}
	cout << "已成功更改" << endl;
	
}


//重新启动LGhub服务
void ReBoot_LG_Services(LGServices* LGS)
{
	if (LGS->Global_Error_Code)
	{
		return;
	}
	SERVICE_STATUS LGServicesStatus;

	if (!QueryServiceStatus(LGS->SVC_Handle, &LGServicesStatus))
	{
		LGS->Global_Error_Code = GetLastError();
		return;
	}
	
	switch (LGServicesStatus.dwCurrentState)
	{
	
	case SERVICE_STOPPED:
	{
		cout << "服务已停止运行" << endl;
		
		cout << "正在启动服务..." << endl;
		if (!StartServiceA(LGS->SVC_Handle, NULL, NULL))
		{
			cout << "服务启动失败" << endl;
			LGS->Global_Error_Code = GetLastError();
			return;
		}

		while (QueryServiceStatus(LGS->SVC_Handle, &LGServicesStatus))
		{
			Sleep(210);
			while (LGServicesStatus.dwCurrentState==SERVICE_RUNNING)
			{
				cout << "服务启动成功" << endl;
				return;
			}
			
		}
	}
		break;
	case SERVICE_START_PENDING:
	{
		cout << "服务正在启动" << endl;
		

	}
		break;
	case SERVICE_STOP_PENDING:
	{
		cout << "服务正在停止" << endl;
		

	}
		break;
	case SERVICE_RUNNING:
	{
		cout << "服务正在运行" << endl;
		

		cout << "正在停止服务..." << endl;
		if(!ControlService(LGS->SVC_Handle, SERVICE_CONTROL_STOP, &LGServicesStatus))
		{
			cout << "服务停止失败" << endl;
			LGS->Global_Error_Code = GetLastError();
			return;
		}
		cout << "服务停止成功" << endl;
		ReBoot_LG_Services(LGS);
	}
		break;
	case SERVICE_PAUSED:
	{
		cout << "服务已暂停" << endl;
		

		cout << "正在停止服务..." << endl;
		if (!ControlService(LGS->SVC_Handle, SERVICE_CONTROL_STOP, &LGServicesStatus))
		{
			cout << "服务停止失败" << endl;
			LGS->Global_Error_Code = GetLastError();
			return;
		}
		cout << "服务停止成功" << endl;
		ReBoot_LG_Services(LGS);
	}
		break;
	default:
		break;
	}
	
}

void Get_LGHub_Services_Directory(LGServices* LGS)
{
	if (LGS->Global_Error_Code)
	{
		return;
	}
	vector<BYTE> buffer;
	LPQUERY_SERVICE_CONFIGA LGhub_Config;
	DWORD Buffer_Size_Need = sizeof(LPQUERY_SERVICE_CONFIGA);

	do
	{
		buffer.resize(Buffer_Size_Need);
		LGhub_Config = (LPQUERY_SERVICE_CONFIGA)&buffer[0];
		if (QueryServiceConfigA(LGS->SVC_Handle, LGhub_Config, (DWORD)buffer.size(), &Buffer_Size_Need))
		{
			LGS->LGhub_Directory = LGhub_Config->lpBinaryPathName;
			LGS->Directory_Index = Get_LGhub_Directory_Index(LGS->LGhub_Directory);
			return;
		}

	} while (GetLastError()== ERROR_INSUFFICIENT_BUFFER);

	

}

bool File_Is_Exist(const string& filedir)
{
	if (FILE* f = fopen(filedir.c_str(), "r"))
	{
		fclose(f);
		return true;
	}
	return false;

}

void Format_LGhub_Directory(LGServices* LGS)
{
	if (LGS->Directory_Index == (size_t)-1)
	{
		return;
	}
	LGS->Directory_Index += 1;
	const string tempstr = LGS->LGhub_Directory;
	LGS->LGhub_Directory.clear();
	for (DWORD i = 1; i < LGS->Directory_Index; i++)
	{
		LGS->LGhub_Directory.append(1, tempstr[i]);
	}
	if (LGS->LGhub_Directory.length())
	{
		LGS->LGhub_agent_Position = LGS->LGhub_Directory;
		LGS->LGhub_Updater_Position = LGS->LGhub_Directory;

		LGS->LGhub_agent_Position.append("lghub_agent.exe");
		LGS->LGhub_Updater_Position.append("lghub_updater.exe");

	}
	cout << "服务所在目录:-> " << LGS->LGhub_Directory << endl;

	
	if (!File_Is_Exist(LGS->LGhub_agent_Position)|| !File_Is_Exist(LGS->LGhub_Updater_Position))
	{
		LGS->Global_Error_Code = ERROR_FILE_NOT_FOUND;
		return;
	}

	cout << "代理程序路径:-> " << LGS->LGhub_agent_Position << endl;
	cout << "更新程序路径:-> " << LGS->LGhub_Updater_Position << endl;

}