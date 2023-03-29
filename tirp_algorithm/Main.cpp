#include <iostream>
#include <iterator>
#include <map>
#include "Event.h"
#include "util_functions.h"
#include <iterator>
#include <set>
#include <thread>
#include "util_results.h"
#include "Single_appliance.h"
#include <chrono>
#include <sstream>

using namespace std;

static void main_one__month_one_faze(string path_to_csv_file, string output_file_full_path, float threshold) {
	std::chrono::steady_clock::time_point begin;
	std::chrono::steady_clock::time_point end;
	int lines_size = get_number_of_rows_in_csv(path_to_csv_file);
	int series_size = get_row_length_in_csv(path_to_csv_file);
	int** parray = read_activations_csv(path_to_csv_file);
	begin = std::chrono::steady_clock::now();
	std::cout << output_file_full_path << " started" << endl;
	std::cout << "total days: " << lines_size << endl;
	get_appliance_patterns(parray, lines_size, series_size, 0, threshold, false, 8, output_file_full_path);
	end = std::chrono::steady_clock::now();
	std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::seconds> (end - begin).count() << "[s]" << std::endl;
	free_2dim_array(parray, lines_size);
}

static int count_ac(string path_to_csv_file, float threshold) {
	int lines_size = get_number_of_rows_in_csv(path_to_csv_file);
	int series_size = get_row_length_in_csv(path_to_csv_file);
	int** parray = read_activations_csv(path_to_csv_file);
	int cnt = ac_algorithm(parray, lines_size, series_size, 0, threshold, 4);
	return cnt;
}

static vector<vector<int>> get_appliance_common_patterns(string dir_path, vector<string> apps, double threshold, int level_to_get) {
	vector<vector<int>> ret_vecs;
	for (string app_name : apps) {
		for (auto& dirEntry : std::filesystem::recursive_directory_iterator(dir_path)) {
			if (!dirEntry.is_regular_file()) {
				std::cout << "Directory: " << dirEntry.path().string() << std::endl;
				continue;
			}
			std::filesystem::path file = dirEntry.path();
			string file_name = file.filename().string();

			if (file_name.find(app_name) != std::string::npos) {
				string path_to_csv_file = file.parent_path().string().substr(0, file.parent_path().string().find("\\")) + "/" + "/" + file_name;
				int lines_size = get_number_of_rows_in_csv(path_to_csv_file);
				int series_size = get_row_length_in_csv(path_to_csv_file);
				int** parray = read_activations_csv(path_to_csv_file);
				vector<vector<int>> temp_ret_vecs = get_specific_appliance_patterns(parray, lines_size, series_size, 0, threshold, false, 2, level_to_get);
				for (vector<int> first_vec : temp_ret_vecs) {
					bool found = false;
					for (vector<int> second_vec : ret_vecs) {
						if (first_vec == second_vec) {
							found = true;
							break;
						}
					}
					if (!found) {
						ret_vecs.push_back(first_vec);
					}
				}
				free_2dim_array(parray, lines_size);
			}
		}
	}
	return ret_vecs;
}

int main() {
	string csv = "";
	for (auto& dirEntry : std::filesystem::recursive_directory_iterator("C:/Users/Omer/PycharmProjects/pythonProject1/houses_data/full_year_symbolic_dataset_ac")) {
		if (!dirEntry.is_regular_file()) {
			std::cout << "Directory: " << dirEntry.path().string() << std::endl;
			continue;
		}
		std::filesystem::path file = dirEntry.path();
		string file_name = file.filename().string();
		string full_file_path = file.parent_path().string() + "/" + file_name;
		int cur_cnt = count_ac(full_file_path, 0.1);
		string id = file_name.substr(0, file_name.find("-"));
		string faze = file_name.substr(file_name.find("-")+6, file_name.find("."));
		faze = faze.at(0);
		csv += id + "," + faze + "," + to_string(cur_cnt) + "\n";	
	}
	ofstream file;
	file.open("ac_report2.csv");
	file << csv;
	file.close();

}
