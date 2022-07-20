#pragma once
#include "Event.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iterator>
#include <vector>
#include <fstream>
#include "util_functions.h"
#include <filesystem>
#include <future>
#include <unordered_set>
using namespace std;

string get_event_distances(Event* cur_event, unordered_map<string, Event*>* dictionary);

pair<int, int> find_min_max_gap(map<int, vector<pair<int, tuple<int, vector<int>*, int>*>*>*>* first_event_appearances, map<int, vector<pair<int, tuple<int, vector<int>*, int>*>*>*>* second_event_appearances);

vector<vector<int>> get_appliance_patterns(int** serieses, int num_of_series, int series_size, float round_num, float threshold, bool to_print, int num_of_workers, string output_file_name);

vector<vector<int>> get_specific_appliance_patterns(int** serieses, int num_of_series, int series_size, float round_num, float threshold, bool to_print, int num_of_workers, int level_to_get);

vector<vector<int>> get_repeating_vectors(vector<vector<int>> vectors, int min_repetitions);

bool exist_in_list_of_vectors(vector<vector<int>> list, vector<int> cur_vector);

bool IsSubset(vector<int> first, vector<int> second);

string create_key(vector<int> vec);

unordered_set<string> create_hashmap(vector<vector<int>> list);

vector<vector<int>> get_vectors_without_subsets(vector<vector<int>> list);

vector<vector<int>> get_vectors_without_subsets2(vector<vector<int>> list, int num_of_workers);

vector<vector<int>> get_vectors_without_subsets4(vector<vector<int>> list, int num_of_workers);

void worker_subset_handler(vector<vector<int>> big_list, vector<vector<int>> small_list, vector<vector<int>>* new_list, mutex* mymutex);

vector<vector<int>> worker_subset_handler2(vector<vector<int>> big_list, vector<vector<int>> small_list);

vector<vector<int>> worker_subset_handler3(vector<vector<int>> big_list, unordered_set<string> hashmap, vector<int> possebilities);

vector<Event*> generate_sub_events(Event* event, unordered_map<string, Event*>* dictionary);

//vector<vector<int>> generate_final_outputs(vector<vector<int>> pre_outputs, Event* cur_event);
vector<vector<int>> generate_output_from_event(string key, unordered_map<string, Event*>* dictionary);

vector<vector<int>> sync_get_all_possible_outputs(unordered_map<string, Event*>* dictionary, vector<vector<int>> cur_outputs, int min_treshold, int min_level, int num_of_workers);

vector<vector<int>> worker_generate_output_from_events(vector<Event*> events, unordered_map<string, Event*>* dictionary);

vector<vector<int>> generate_final_outputs(int day, vector<vector<int>> pre_outputs, Event* cur_event);

void user_interaction(unordered_map<string, Event*>* dictionary);

void write_activation_to_csv(vector<vector<int>> activations, string path);


vector<vector<int>> get_all_possible_outputs(unordered_map<string, Event*>* dictionary, vector<vector<int>> cur_outputs, int min_treshold, int min_level);

void listFiles(std::string path);




