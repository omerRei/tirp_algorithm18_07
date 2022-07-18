#include "Single_appliance.h"

vector<vector<int>>* Single_appliance::appliance_patterns = new vector<vector<int>>();
double Single_appliance::similarity_threshold = 0.0;

double Single_appliance::get_threshold()
{
	return Single_appliance::similarity_threshold;
}

void Single_appliance::set_threshold(double t)
{
	Single_appliance::similarity_threshold = t;
}

void Single_appliance::set_patterns(vector<vector<int>> patterns) {
	//Single_appliance::appliance_patterns = new vector<vector<int>*>();
	for (vector<int> vec : patterns) {
		vector<int> new_vec = vector<int>(vec);
		Single_appliance::appliance_patterns->push_back(new_vec);
	}
}

vector<vector<int>>* Single_appliance::get_patterns() {
	return Single_appliance::appliance_patterns;
}

void Single_appliance::free_appliances_patterns() {
	
	Single_appliance::appliance_patterns->clear();
	delete Single_appliance::appliance_patterns;
}

