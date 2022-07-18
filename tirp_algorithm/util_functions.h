#pragma once
#include "Event.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iterator>
#include <vector>
#include <iomanip>
#include <thread>
#include <mutex>
#include <filesystem>
#include "Single_appliance.h"

using namespace std;

unordered_map<string, Event*>* first_iteration(int** list_of_series, int size_of_list_series, int size_of_series, int round_in);
int get_num_of_candidates(unordered_map<string, Event*>* dictionary, int level);
unordered_map<string, Event*>* remove_uncommon(unordered_map<string, Event*>* dictionary, int level_to_remove, float min_support, int denominator);
vector<string> get_possible_keys(string cur_key_begining, int* posebilities_array, int size_of_posebilities);
int* get_possibilities_array(unordered_map<string, Event*>* dictionary);
unordered_map<string, Event*>* tirp_algorithm(int** list_of_series, int size_of_list_series, int size_of_series, int round_in, float min_sup, int num_of_workers);
void iteration(unordered_map<string, Event*>* dictionary, int cur_level, int* probabilities_array, int size_of_posebilities, float min_sup, int support_denominator);
void iteration(unordered_map<string, Event*>* dictionary, int cur_level, int* probabilities_array, int size_of_posebilities, float min_sup, int support_denominator, int num_of_workers);
void worker_iteration(int task_num, unordered_map<string, Event*>* dictionary, vector<pair<Event*, Event*>> possible_combinations, int cur_level, float min_sup, int support_denominator, mutex* g_mutex);
//unordered_map<string, Event*>* get_dictionary_with_copies(unordered_map<string, Event*>* dictionary, float round_num);
bool check_if_in_dic(unordered_map<string, Event*>* dic, string key);
bool synchronized_check_if_in_dic(unordered_map<string, Event*>* dic, string key, mutex* mutex);
int get_row_length_in_csv(string path);
int get_number_of_rows_in_csv(string path);
int** read_activations_csv(string path, int rows_num, int size_of_row);
int** read_activations_csv(string path);
vector<Event*> get_events_from_level(unordered_map<string, Event*>* dictionary, int level);
int** concat_two_arrays(int** first_arr, int first_lines_size, int** second_arr, int second_lines_size, int line_length);
int get_max_level(unordered_map<string, Event*>* dictionary);
void free_dictionary(unordered_map<string, Event*>* dictionary);
bool check_event_suits(Event* event, int min_jump, int min_level);
bool check_event_suits(int* total_events, int cur_level, int min_jump, int min_level);
vector<string> get_file_names(string path, string appliance_name);
int** get_all_appliances_activations(string appliance_name, string path, int max_activation_size);
int** pad_zeroes(int** parray, int array_size, int row_length, int new_row_length);
int get_all_appliances_row_size(string appliance_name, string path);
void free_2dim_array(int** parray, int num_of_rows);
void worker_create_possible_pairs(int task_num, unordered_map<string, Event*>* dictionary, vector<string> first_keys, int* probabilities_array, int size_of_posebilities, int cur_level, float min_sup, int support_denominator, mutex* g_mutex);


double dtw(vector<int> first_arr, vector<int> second_arr);
bool is_patternts_similar(int* total_events, int size);