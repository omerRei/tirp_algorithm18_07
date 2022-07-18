#pragma once
#include <vector>
using namespace std;



class Single_appliance
{
public:
	static vector<vector<int>>* appliance_patterns;
	static double similarity_threshold;
	static double get_threshold();
	static void set_threshold(double t);
	static void set_patterns(vector<vector<int>> patterns);
	static vector<vector<int>>* get_patterns();
	static void free_appliances_patterns();
};

