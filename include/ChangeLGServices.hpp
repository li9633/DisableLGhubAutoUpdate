#pragma once
#include <iostream>
#include <Windows.h>
#include <winsvc.h>
#include <winerror.h>
#include <string>
#include <vector>
#include <stdlib.h>
#include "LGServices.hpp"
#include "LGhubJson.hpp"
#include "AddLGFWRule.hpp"
#define ERROR_CANNOT_FIND (size_t)-1


void Format_LGhub_Directory(LGServices* LGS);

size_t Get_LGhub_Directory_Index(std::string str);

void Change_LG_service(LGServices* LGS);

void ReBoot_LG_Services(LGServices* LGS);

void DisplayErrorText(DWORD err);

void Get_LGHub_Services_Directory(LGServices* LGS);

bool File_Is_Exist(const std::string& filedir);