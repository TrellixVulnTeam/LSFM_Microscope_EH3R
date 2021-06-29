
#include "pch.h"
#include "GlobalFunctions.h"


vector<string> Stringdelimiter(string s1, string delimiter) {

	/* method to split a string with help from a delimiter*/

	size_t pos = 0;
	vector<string> list1;
	while ((pos = s1.find(delimiter)) != string::npos) {
		list1.push_back(s1.substr(0, pos));
		s1.erase(0, pos + delimiter.length());
	}

	return list1;

}


vector<LSFM_Motion_PI> CompareWithConfigFile(const char* pfadFile, vector<string> controlerList)
{
	/*//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///                      Method to check if the detected controllers match with those stored in the configuration file.                                                    ///
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	*/


	vector<string> list1;
	vector<LSFM_Motion_PI>listController;
	string line;
	ifstream input;
	string delimiter = "->";
	size_t pos = 0;
	input.open(pfadFile);
	if (input.is_open())
	{
		while (getline(input, line))
		{
			list1.push_back(line);
		}

		for (int i = 0; i < controlerList.size(); i++)
		{
			string s1 = controlerList[i];
			for (int j = 0; j < list1.size(); j++)
			{

				vector<string> list2;
				size_t pos = 0;
				string s3 = list1.at(j);
				while ((pos = s3.find(delimiter)) != string::npos) {
					list2.push_back(s3.substr(0, pos));
					s3.erase(0, pos + delimiter.length());
					list2.push_back(s3);
				}

				string s2 = list2[0];

				if (s1.find(s2) != std::string::npos) {
					char char_array[100];
					strcpy_s(char_array, 100, s1.c_str());
					int stage = atoi(list2[1].c_str());
					LSFM_Motion_PI motion(char_array, stage);
					listController.push_back(motion);
					break;
				}
			}

		}
		return listController;
	}

	else
	{
		throw std::runtime_error("The file to compare the Controller does not exist");
	}


	input.close();

}

vector<LSFM_Motion_PI> controllerDetection(char* description, int descriptionSize)
{
	/*//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///                       Method for detecting and identifying different controllers                                                                                           ///
	///                The purpose of this method is to detect the different controllers connected via USB and compare them with those stored in the configuration file.///
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	*/


	// Pfad of the configuration file
	const char* fileName = "C:\\LSFM_Software\\TreiberImplementierung\\LSFMLibrary\\ConfigFile.txt";
	


	// Vector to save all the Controller as object of a Class Motion_PI
	vector<LSFM_Motion_PI>listController;

	//method of the API to identify all hardware connected to the computer via USB. It is also possible to pass a filter to this method.
	//Here the filter is the word PI because all controllers are of type PI. 
	int iD = PI_EnumerateUSB(description, descriptionSize, "PI");

	if (iD >= 0)
	{
		int compare = strcmp(description, "");
		if (!compare)
		{
			printf("Any Controller is available\n");
			printf("Please Connect a Controller\n");
		}


		else
		{
			printf("List of available controllers:\n");
			printf("%s\n", description);

			string identificationList = description;

			string delimiter = "\n";


			vector<string> list1 = Stringdelimiter(identificationList, delimiter);


			//Matching with list of existing Controllers and  assignment to x/y/z/focus 

			try {
				listController = CompareWithConfigFile(fileName, list1);

			}
			catch (const std::exception& e) {
				std::cout << "Error " << e.what();
			}

		}
	}
	else
	{
		printf("Could not connect to controller.\n");

	}

	return listController;
};
