#pragma once


#include "LSFM_Motion_PI.h"
#include <vector>
#include <fstream>
#include "LSFM_Laser.h"



using namespace std;

vector<string> Stringdelimiter(string s1, string delimiter);

vector<LSFM_Motion_PI> CompareWithConfigFile(const char* pfadFile, vector<string> controlerList);

vector<LSFM_Motion_PI> controllerDetection(char* description, int descriptionSize, const char* fileName);

vector<LSFM_Laser> LaserPortDetection(const char* filePath);
