#include "include/ChangeLGServices.hpp"

void ProgramStart()
{
	LGServices* LGS = new LGServices;

	Change_LG_service(LGS);//更改服务类型为手动

	ReBoot_LG_Services(LGS);//重启动LG服务

	Get_LGHub_Services_Directory(LGS);//获取服务所在目录

	Format_LGhub_Directory(LGS);//通过服务所在路径格式化

	AddLGFWRule(LGS);//添加防火墙规则

	ChangeLGhubJson(LGS);//更改罗技配置文件


	delete(LGS);
	system("pause");

}

void AboutThis()
{
	std::cout << "->关于此项目<-" << std::endl;
	std::cout << "项目地址:-> https://github.com/li9633/DisableLGhubAutoUpdate" << std::endl;
	std::cout << "遇到问题请向本项目提交issue！" << std::endl;
	system("pause");
	system("cls");
}

int main(void)
{
	int Index = NULL;
	while (true)
	{
		std::cout << "-----------------------------------------" << std::endl;
		std::cout << "请输入特定选项后按Enter" << std::endl;
		std::cout << "输入\'1\'->LGhub禁止更新   \'2\'->关于本项目" << std::endl;
		std::cout << "-----------------------------------------" << std::endl;
		std::cout<<"请输入:->";
		std::cin >> Index;
		switch (Index)
		{

		case 1:
		{
			ProgramStart();
			return 0;
		}
		case 2:
		{
			AboutThis();
		}

		default:
			break;
		}
	}


	return 0;
}