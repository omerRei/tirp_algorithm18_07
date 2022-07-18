#include "Main.h"
#include <iostream>
#include <iterator>
#include <map>
#include "Event.h"
#include "util_functions.h"
#include <iterator>
#include <set>
#include <thread>
#include "util_results.h"
#include <chrono>
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

static vector<vector<int>> get_appliance_common_patterns(string dir_path, string app_name, double threshold, int level_to_get) {
	vector<vector<int>> ret_vecs;
	for (auto& dirEntry : std::filesystem::recursive_directory_iterator(dir_path)) { //"C:/Users/omer/PycharmProjects/pythonProject1/houses_data/generated_csvs"
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
	return ret_vecs;
}

int main() {

	/* need to check the size of the apliances vectorsand the threshold
	vector<int> a = { 1,2,6,4 };
	vector<int> b = { 2,6,3,4 };
	cout << dtw(a, b);*/

	vector<vector<int>> appliances_patterns = get_appliance_common_patterns("C:/Users/omer/PycharmProjects/pythonProject1/houses_data/generated_csvs", "washing machine", 0.15, 4);
	Single_appliance::set_patterns(appliances_patterns);
	Single_appliance::set_threshold(0.5);
	for (auto& dirEntry : std::filesystem::recursive_directory_iterator("C:/Users/Omer/PycharmProjects/pythonProject1/houses_data/tirps_dataset3_2")) {
		if (!dirEntry.is_regular_file()) {
			std::cout << "Directory: " << dirEntry.path().string() << std::endl;
			continue;
		}
		std::filesystem::path file = dirEntry.path();
		string file_name = file.filename().string();
		string house_num = file.parent_path().string().substr(file.parent_path().string().find("\\") + 1);
		string output_file_path = "C:/Users/Omer/PycharmProjects/pythonProject1/houses_data/dataset4_3/" + house_num + +"/" + file_name;
		string full_file_path = file.parent_path().string().substr(0, file.parent_path().string().find("\\")) + "/" + house_num + +"/" + file_name;
		ifstream ifile;
		ifile.open(output_file_path);
		if (ifile) {
			cout << output_file_path << "file exists" << endl;
		}
		else {
			main_one__month_one_faze(full_file_path, output_file_path, 0.15);
		}

	}

	//listFiles("C:/Users/omerrei/PycharmProjects/pythonProject7/houses_data/tirps_dataset3");
	/*const string files_path[14]
		= { "C:/Users/omerrei/PycharmProjects/pythonProject7/houses_data/tirps_datasets/4_19.csv",
		"C:/Users/omerrei/PycharmProjects/pythonProject7/houses_data/tirps_datasets/3_2078.csv",
		"C:/Users/omerrei/PycharmProjects/pythonProject7/houses_data/tirps_datasets/4_2078.csv",
		"C:/Users/omerrei/PycharmProjects/pythonProject7/houses_data/tirps_datasets/4_2069.csv",
		"C:/Users/omerrei/PycharmProjects/pythonProject7/houses_data/tirps_datasets/3_27.csv",
		"C:/Users/omerrei/PycharmProjects/pythonProject7/houses_data/tirps_datasets/3_2026.csv",
		"C:/Users/omerrei/PycharmProjects/pythonProject7/houses_data/tirps_datasets/2_4004.csv",
		"C:/Users/omerrei/PycharmProjects/pythonProject7/houses_data/tirps_datasets/3_2001.csv",
		"C:/Users/omerrei/PycharmProjects/pythonProject7/houses_data/tirps_datasets/3_1001.csv",
		"C:/Users/omerrei/PycharmProjects/pythonProject7/houses_data/tirps_datasets/2_2059.csv",
		"C:/Users/omerrei/PycharmProjects/pythonProject7/houses_data/tirps_datasets/3_2004.csv",
		"C:/Users/omerrei/PycharmProjects/pythonProject7/houses_data/tirps_datasets/3_33.csv",
		"C:/Users/omerrei/PycharmProjects/pythonProject7/houses_data/tirps_datasets/4_10.csv",
		"C:/Users/omerrei/PycharmProjects/pythonProject7/houses_data/tirps_datasets/4_2022.csv"
	};


	for (int i = 0; i < 14; i++) {
		std::chrono::steady_clock::time_point begin;
		std::chrono::steady_clock::time_point end;
		string path_to_csv_file = files_path[i];
		float threshold = 0.35;
		int lines_size = get_number_of_rows_in_csv(path_to_csv_file);
		int series_size = get_row_length_in_csv(path_to_csv_file);
		int** parray = read_activations_csv(path_to_csv_file);
		begin = std::chrono::steady_clock::now();
		std::size_t found = path_to_csv_file.find_last_of("/");
		string file_name = path_to_csv_file.substr(found + 1);
		std::cout << file_name << "started" << endl;
		std::cout << "total days: " << lines_size << endl;
		get_appliance_patterns(parray, lines_size, series_size, 0, threshold, false, 16, file_name);
		end = std::chrono::steady_clock::now();
		std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::seconds> (end - begin).count() << "[s]" << std::endl;
		free_2dim_array(parray, lines_size);
	}*/



	//std::chrono::steady_clock::time_point begin;
	//std::chrono::steady_clock::time_point end;
	//string path_to_csv_file = "C:/Users/omerrei/PycharmProjects/pythonProject7/houses_data/tirps_datasets/3_33.csv";
	//float threshold = 0.15;
	//int lines_size = get_number_of_rows_in_csv(path_to_csv_file);
	//int series_size = get_row_length_in_csv(path_to_csv_file);
	//int** parray = read_activations_csv(path_to_csv_file);
	//begin = std::chrono::steady_clock::now();
	//std::cout << "total days: " << lines_size << endl;
	//get_appliance_patterns(parray, lines_size, series_size, 0, threshold, true, 4, "xxxx.csv");
	//end = std::chrono::steady_clock::now();
	//std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::seconds> (end - begin).count() << "[s]" <<std::endl;
	//free_2dim_array(parray, lines_size);
	//int x = 0;


	/*std::ifstream file("C:/Users/omerr/PycharmProjects/nilmtkWithTirps/activations_sets_sax/sizes.txt");
	string appliance = "dish washer_10";
	int series_size = 310;
	int** activations = nullptr;
	int current_activations_length = 0;
	std::string line;
	while (std::getline(file, line))
	{
		if (line.find(appliance) != std::string::npos) {
			string file_path = "C:/Users/omerr/PycharmProjects/nilmtkWithTirps/activations_sets_sax/" + line.substr(0, line.find(":")) + ".csv";
			int rows_length = stoi(line.substr(line.find(":") + 1, line.size()));
			int** parray = read_activations_csv(file_path, rows_length, series_size);
			if (!activations)
				activations = parray;
			else {
				int** new_activations = concat_two_arrays(activations, current_activations_length, parray, rows_length, series_size);
				//need to free old arrrays
				activations = new_activations;
			}
			current_activations_length += rows_length;
		}
	}
	cout << "appliance " << appliance << endl;
	float thresholds[5] = { 0.025 , 0.05, 0.1, 0.20, 0.25 };
	for (int j = 0; j < 5; j++) {
		get_appliance_patterns(activations, current_activations_length, series_size, 0, thresholds[j], 2);
		cout << "#####################################################" << endl;
	}*/

	/*std::ifstream file("C:/Users/omerr/PycharmProjects/nilmtkWithTirps/activations_sets/sizes.txt");
	string appliance = "dish washer_200.0";
	int series_size = 310;
	int** activations = nullptr;
	std::string line;
	vector<vector<int>> complete_patterns;
	while (std::getline(file, line))
	{
		if (line.find(appliance) != std::string::npos) {
			cout << line << " begins :" << endl;
			string file_path = "C:/Users/omerr/PycharmProjects/nilmtkWithTirps/activations_sets/" + line.substr(0, line.find(":")) + ".csv";
			int rows_length = stoi(line.substr(line.find(":") + 1, line.size()));
			int** parray = read_activations_csv(file_path, rows_length, series_size);
			activations = parray;
			cout << "appliance " << appliance << endl;
			float thresholds[1] = {0.25};
			for (int j = 0; j < 1; j++) {
				vector<vector<int>> cur_patterns = get_appliance_patterns(activations, rows_length, series_size, 0, thresholds[j], 2, false);
				complete_patterns.insert(complete_patterns.end(), cur_patterns.begin(), cur_patterns.end());
				cout << "############### finished #############" << endl;
			}
		}
	}
	for (int i = 0; i < complete_patterns.size(); i++) {
		vector<int> cur_pattern = complete_patterns[i];
		for (int j = 0; j < cur_pattern.size(); j++) {
			cout << cur_pattern[j] << ",";
		}
		cout << endl;
	}
	cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
	vector<vector<int>> repeatings = get_repeating_vectors(complete_patterns, 3);
	for (int i = 0; i < repeatings.size(); i++) {
		vector<int> cur_pattern = repeatings[i];
		int min_power = 1800;
		int max = 0;
		for(int j = 0; j < cur_pattern.size(); j++){
			if (cur_pattern[j] > max)
				max = cur_pattern[j];
		}
		if ( max < min_power)
			continue;
		for (int j = 0; j < cur_pattern.size(); j++) {
			cout << cur_pattern[j] << ",";
		}
		cout << endl;
	}*/



}
