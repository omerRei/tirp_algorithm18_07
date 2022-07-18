#pragma once

#include <map>
#include<unordered_map>
#include <vector>
#include <string>
using namespace std;

class Event
{
public:
	const static int MAX_SIZE_BETWEEN_EVENTS = 60;
	const static int TOTAL_SIZE_OF_EVENT = 180;
	int* total_events;
	int level;
	map<int, vector<pair<int, tuple<int, vector<int>*, int>*>*>*>* appearances;  // map<series,vector<begining,pair<ending begining,vector>
	void add_appearnce(int series_index, int appear_index);
	float get_support(int denominator);
	Event* addition(Event* other_event);
	std::string get_key();
	//void add_appearnaces_from_other(Event* event); // only for copy iteration
	Event();
	//Event(Event* cur_event);
	~Event();
	static int get_max_size_between_events() { return Event::MAX_SIZE_BETWEEN_EVENTS; };
	static int get_total_size_of_event() { return Event::TOTAL_SIZE_OF_EVENT; };
};

vector<pair<int, tuple<int, vector<int>*, int>*>*>* get_possible_appearnaces(vector<pair<int, tuple<int, vector<int>*, int>*>*>* first_appearances, vector<pair<int, tuple<int, vector<int>*, int>*>*>* second_appearances);
vector<pair<int, tuple<int, vector<int>*, int>*>*>* get_possible_appearnaces_2nd_level(vector<pair<int, tuple<int, vector<int>*, int>*>*>* first_appearances, vector<pair<int, tuple<int, vector<int>*, int>*>*>* second_appearances);
map<int, vector<pair<int, tuple<int, vector<int>*, int>*>*>*>* get_new_map(map<int, vector<pair<int, tuple<int, vector<int>*, int>*>*>*>* first_map, map<int, vector<pair<int, tuple<int, vector<int>*, int>*>*>*>* second_map, bool is_second_level);
int* get_new_total_events(int* first_events, int* second_events, int cur_level);