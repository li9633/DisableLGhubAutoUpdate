#include "../include/LGServices.hpp"
#include "../include/LGhubJson.hpp"

using namespace nlohmann;
template<class K, class V, class dummy_compare, class A>
using my_workaround_fifo_map = fifo_map<K, V, fifo_map_compare<K>, A>;
using my_json = basic_json<my_workaround_fifo_map>;

void ChangeLGhubJson(LGServices* LGS)
{
	if (LGS->Directory_Index == (size_t)-1 || LGS->Global_Error_Code != ERROR_SUCCESS)
	{
		return;
	}
	
	my_json data;
	data =
	{
		{"checkPeriodically", false},
		{"intervalInSeconds" , 0 },
		{"nextCheckDueDate" , "20501231T235959"},
		{"downloadAutomatically" , false }

	};

	std::ofstream lgout("C:\\ProgramData\\LGHUB\\periodic_check.json");
	if (!lgout.is_open())
	{
		LGS->Global_Error_Code = ERROR_TOO_MANY_OPEN_FILES;
		std::cout << "无法打开 \'C:\\ProgramData\\LGHUB\\periodic_check.json \'\n请检查该文件是否存在!" << std::endl;
		return;
	}
	lgout << std::setw(1) << data << std::endl;
	std::cout << "LG更新配置文件(JSON)已更改为:->" << std::endl;
	std::cout << std::setw(1) << data << std::endl;
	std::cout << "LG更新配置文件(JSON)路径为:-> \'C:\\ProgramData\\LGHUB\\periodic_check.json \'" << std::endl;
	lgout.close();
}