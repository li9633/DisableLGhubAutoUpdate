#include "../include/AddLGFWRule.hpp"

void AddLGFWRule(LGServices* LGS)
{
	if (LGS->Directory_Index == (size_t)-1 || LGS->Global_Error_Code != ERROR_SUCCESS)
	{
		return;
	}
	
	HRESULT hr = CoInitializeEx(0, COINIT_APARTMENTTHREADED);// 初始化 COM
	INetFwPolicy2* pFwPolicy2 = NULL;
	hr = CoCreateInstance(__uuidof(NetFwPolicy2), NULL, CLSCTX_INPROC_SERVER, __uuidof(INetFwPolicy2), (void**)&pFwPolicy2);
	if (!SUCCEEDED(hr))
	{
		
		LGS->Global_Error_Code = GetLastError();
		std::cout <<"ErrorCode = " << LGS->Global_Error_Code << std::endl;
		return;
	}
	INetFwRules* pFwRules = NULL;
	//获取防火墙规则
	hr = pFwPolicy2->get_Rules(&pFwRules);
	if (!SUCCEEDED(hr))
	{

		LGS->Global_Error_Code = GetLastError();
		std::cout << "ErrorCode = " << LGS->Global_Error_Code << std::endl;
		return;
	}
	INetFwRule* pFwRule = NULL;
	BSTR ruleName = SysAllocString(L"DisableLGSerivcesTCP");

	//从规则集合中获取指定名称的规则
	hr = pFwRules->Item(ruleName, &pFwRule);
	if (SUCCEEDED(hr))
	{

		std::cout << "已拥有此防火墙规则" << std::endl;
		pFwRules->Remove(ruleName);//删除名称相同的防火墙
	}
	//创建一个新的防火墙规则
	hr = CoCreateInstance(__uuidof(NetFwRule), NULL, CLSCTX_INPROC_SERVER, __uuidof(INetFwRule), (void**)&pFwRule);
	if (!SUCCEEDED(hr))
	{

		LGS->Global_Error_Code = GetLastError();
		std::cout << "ErrorCode = " << LGS->Global_Error_Code << std::endl;
		return;
	}

	 //设置规则的属性
	pFwRule->put_Name(ruleName);
	pFwRule->put_Description(SysAllocString(L"Disable LG Serivces"));
	pFwRule->put_Action(NET_FW_ACTION_BLOCK); // 阻止连接
	pFwRule->put_Direction(NET_FW_RULE_DIR_IN); // 入站规则
	pFwRule->put_Enabled(VARIANT_TRUE); // 启用规则                  
	pFwRule->put_Protocol(NET_FW_IP_PROTOCOL_TCP); // TCP 协议
	pFwRule->put_LocalPorts(SysAllocString(L"ALL")); // 所有端口
	pFwRule->put_InterfaceTypes(SysAllocString(L"All")); // 适用于所有网络接口
	pFwRule->put_ApplicationName(_com_util::ConvertStringToBSTR(LGS->LGhub_agent_Position.c_str()));//程序路径

	 //将规则添加到规则集合中
	hr = pFwRules->Add(pFwRule);
	if (SUCCEEDED(hr)) 
	{
		std::cout << "防火墙添加完成" << std::endl;
	}
	else {
		std::cout << "已拥有此防火墙规则" << hr << std::endl;
	}

	pFwRule = NULL;
	ruleName = SysAllocString(L"DisableLGSerivcesUDP");

	//从规则集合中获取指定名称的规则
	hr = pFwRules->Item(ruleName, &pFwRule);
	if (SUCCEEDED(hr))
	{

		std::cout << "已拥有此防火墙规则" << std::endl;
		pFwRules->Remove(ruleName);//删除名为""DisableLGSerivcesTCP"的防火墙
		//CoUninitialize();//反初始化COM
	}
	//创建一个新的防火墙规则
	hr = CoCreateInstance(__uuidof(NetFwRule), NULL, CLSCTX_INPROC_SERVER, __uuidof(INetFwRule), (void**)&pFwRule);
	if (!SUCCEEDED(hr))
	{

		LGS->Global_Error_Code = GetLastError();
		std::cout << "ErrorCode = " << LGS->Global_Error_Code << std::endl;
		return;
	}

	// 设置规则的属性
	pFwRule->put_Name(ruleName);
	pFwRule->put_Description(SysAllocString(L"Disable LG Serivces"));
	pFwRule->put_Action(NET_FW_ACTION_BLOCK); // 阻止连接
	pFwRule->put_Direction(NET_FW_RULE_DIR_IN); // 入站规则
	pFwRule->put_Enabled(VARIANT_TRUE); // 启用规则                  
	pFwRule->put_Protocol(NET_FW_IP_PROTOCOL_UDP); // TCP 协议
	pFwRule->put_LocalPorts(SysAllocString(L"ALL")); // 所有端口
	pFwRule->put_InterfaceTypes(SysAllocString(L"All")); // 适用于所有网络接口
	pFwRule->put_ApplicationName(_com_util::ConvertStringToBSTR(LGS->LGhub_agent_Position.c_str()));//程序路径

	// 将规则添加到规则集合中
	hr = pFwRules->Add(pFwRule);
	if (SUCCEEDED(hr))
	{
		std::cout << "防火墙添加完成" << std::endl;
	}
	else {
		std::cout << "已拥有此防火墙规则" << hr << std::endl;
	}

	pFwRule->Release();
	SysFreeString(ruleName);
	pFwRules->Release();
	CoUninitialize();//反初始化COM

}