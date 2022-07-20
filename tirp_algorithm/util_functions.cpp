#include "util_functions.h"
using namespace std;

//double Single_appliance::similarity_threshold = 0.8;

bool check_if_in_dic(unordered_map<string, Event*>* dic, string key) {
    if (dic->count(key))
        return true;
    return false;
}

bool synchronized_check_if_in_dic(unordered_map<string, Event*>* dic, string key, mutex* mutex) {
    mutex->lock();
    if (dic->count(key)) {
        mutex->unlock();
        return true;
    }
    mutex->unlock();
    return false;
}

/*unordered_map<string, Event*>* get_dictionary_with_copies(unordered_map<string, Event*>* old_dictionary, float round_num) {
    unordered_map<string, Event*>* new_dictionary = new unordered_map<string, Event*>();
    unordered_map<string, Event*>::iterator it;
    for (it = old_dictionary->begin(); it != old_dictionary->end(); it++)
    {
        Event* cur_event = it->second;
        Event* new_event = new Event(cur_event);
        new_dictionary->insert({ new_event->get_key(),new_event});
    }

    for (it = new_dictionary->begin(); it != new_dictionary->end(); it++)
    {
        Event* cur_event = it->second;
        int above_key = cur_event->total_events[0] + round_num;
        int below_key = cur_event->total_events[0] - round_num;
        int* other_keys = new int[2]{ above_key, below_key};
        for (int i = 0; i < 2; i++) {
            string other_key = std::to_string(other_keys[i]) + ",";
            if (check_if_in_dic(old_dictionary, other_key)) {
                Event* other_event =  old_dictionary->find(other_key)->second;
                cur_event->add_appearnaces_from_other(other_event);
             }
        }

    }
    free_dictionary(old_dictionary);
    return new_dictionary;
}*/


unordered_map<string, Event*>* first_iteration(int** list_of_series, int size_of_list_series, int size_of_series, int round_in) {
    unordered_map<string, Event*>* dictionary = new unordered_map<string, Event*>();
    for (int series_index = 0; series_index < size_of_list_series; series_index++) {
        int* cur_series = list_of_series[series_index];
        for (int single_event_index = 0; single_event_index < size_of_series; single_event_index++) {
            int single_event_value = cur_series[single_event_index];
            if (single_event_value == 0)
                continue;
            string cur_key = to_string(single_event_value) + ",";
            if (!dictionary->count(cur_key)) {
                Event* new_event = new Event();
                new_event->level = 1;
                int* new_total_events = new int(1);
                new_total_events[0] = single_event_value;
                new_event->total_events = new_total_events;
                dictionary->insert({ cur_key , new_event });
            }
            auto it = dictionary->find(cur_key);
            it->second->add_appearnce(series_index, single_event_index);
        }
    }
    if (round_in) {}
    //dictionary = get_dictionary_with_copies(dictionary, round_in);
    return dictionary;
}

void iteration(unordered_map<string, Event*>* dictionary, int cur_level, int* probabilities_array, int size_of_posebilities, float min_sup, int support_denominator) {
    unordered_map<string, Event*>::iterator it;
    int print_counter = 0;
    int possibilities_num = get_num_of_candidates(dictionary, cur_level - 1);
    float pre_size = dictionary->size();
    int last_print_val = 0;
    for (it = dictionary->begin(); it != dictionary->end(); it++)
    {
        Event* p_first_event = it->second;
        if (p_first_event->level != cur_level - 1) {
            continue;
        }
        print_counter++;
        if (last_print_val != (float(print_counter) / float(possibilities_num)) * 100 && print_counter % 100 == 0) {
            cout << fixed << setprecision(2) << float(print_counter) / float(possibilities_num) << ",";
            last_print_val = (float(print_counter) / float(possibilities_num)) * 100;
        }
        for (int prob_index = 0; prob_index < size_of_posebilities; prob_index++) {
            string new_key_begining = "";
            if (p_first_event->level > 1)
                new_key_begining = p_first_event->get_key().substr(p_first_event->get_key().find(",") + 1);
            string key_to_check = new_key_begining + to_string(probabilities_array[prob_index]) + ",";
            if (check_if_in_dic(dictionary, key_to_check)) {
                Event* p_second_event = dictionary->find(key_to_check)->second;
                Event* new_event = p_first_event->addition(p_second_event);
                int* pre_total_events = p_first_event->total_events;
                new_event->level = cur_level;
                int* new_total_events = new int[cur_level];
                if (cur_level == 2) {
                    new_total_events[0] = pre_total_events[0];
                    new_total_events[1] = probabilities_array[prob_index];
                }
                else {
                    for (int copy_index = 0; copy_index < cur_level; copy_index++) {
                        if (copy_index == cur_level - 1) {
                            new_total_events[copy_index] = probabilities_array[prob_index];
                        }
                        else
                        {
                            new_total_events[copy_index] = pre_total_events[copy_index];
                        }
                    }
                }
                new_event->total_events = new_total_events;
                if (new_event->get_support(support_denominator) < min_sup) {
                    delete new_event;
                }
                else
                    dictionary->insert({ new_event->get_key(),new_event });
            }
        }
    }
    cout << endl;
}

void iteration(unordered_map<string, Event*>* dictionary, int cur_level, int* probabilities_array, int size_of_posebilities, float min_sup, int support_denominator, int num_of_workers)
{
    mutex* mymutex = new mutex();
    unordered_map<string, Event*>::iterator it;
    int print_counter = 0;
    int possibilities_num = get_num_of_candidates(dictionary, cur_level - 1);
    float pre_size = dictionary->size();
    int last_print_val = 0;
    vector<pair<Event*, Event*>> all_possible_combinations;
    for (it = dictionary->begin(); it != dictionary->end(); it++)//very slow!!
    {
        Event* p_first_event = it->second;
        if (p_first_event->level != cur_level - 1) {
            continue;
        }
        string new_key_begining = p_first_event->get_key().substr(p_first_event->get_key().find(",") + 1);
        vector<string> possible_keys = get_possible_keys(new_key_begining, probabilities_array, size_of_posebilities);
        for (string key : possible_keys) {
            if (check_if_in_dic(dictionary, key)) {
                Event* p_second_event = dictionary->find(key)->second;
                all_possible_combinations.push_back(pair<Event*, Event*>(p_first_event, p_second_event));
            }
        }
    }
    vector<vector<pair<Event*, Event*>>> all_vectors;
    int size_of_vector = int(all_possible_combinations.size() / num_of_workers);
    if (!size_of_vector)
        size_of_vector = 1;
    for (size_t i = 0; i < all_possible_combinations.size(); i += size_of_vector) {
        int last;
        if (all_possible_combinations.size() < i + size_of_vector)
            last = all_possible_combinations.size();
        else
            last = i + size_of_vector;
        all_vectors.emplace_back(all_possible_combinations.begin() + i, all_possible_combinations.begin() + last);
    }
    vector<thread> threads;
    for (int i = 0; i < all_vectors.size(); i++) {
        threads.push_back(thread(worker_iteration, i, dictionary, all_vectors[i], cur_level, min_sup, support_denominator, mymutex));
    }
    for (int i = 0; i < threads.size(); i++) {
        threads[i].join();
    }
    delete mymutex;
}

void iteration2(unordered_map<string, Event*>* dictionary, int cur_level, int* probabilities_array, int size_of_posebilities, float min_sup, int support_denominator, int num_of_workers)
{
    mutex* mymutex = new mutex();
    unordered_map<string, Event*>::iterator it;
    int possibilities_num = get_num_of_candidates(dictionary, cur_level - 1);
    vector<string> all_keys;
    for (it = dictionary->begin(); it != dictionary->end(); it++)//very slow!!
    {
        Event* p_first_event = it->second;
        if (p_first_event->level == cur_level - 1) {
            all_keys.push_back(it->first);
        }
    }
    vector<vector<string>> all_vectors;
    int size_of_vector = int(all_keys.size() / num_of_workers);
    if (!size_of_vector)
        size_of_vector = 1;
    for (size_t i = 0; i < all_keys.size(); i += size_of_vector) {
        int last;
        if (all_keys.size() < i + size_of_vector)
            last = all_keys.size();
        else
            last = i + size_of_vector;
        all_vectors.emplace_back(all_keys.begin() + i, all_keys.begin() + last);
    }
    vector<thread> threads;
    for (int i = 0; i < all_vectors.size(); i++) {
        threads.push_back(thread(worker_create_possible_pairs, i, dictionary, all_vectors[i], probabilities_array, size_of_posebilities, cur_level, min_sup, support_denominator, mymutex));
    }
    for (int i = 0; i < threads.size(); i++) {
        threads[i].join();
    }
    delete mymutex;
}

void worker_iteration(int task_num, unordered_map<string, Event*>* dictionary, vector<pair<Event*, Event*>> possible_combinations, int cur_level, float min_sup, int support_denominator, mutex* g_mutex)
{
    for (int i = 0; i < possible_combinations.size(); i++) {
        Event* p_first_event = possible_combinations[i].first;
        Event* p_second_event = possible_combinations[i].second;
        int* new_total_events = get_new_total_events(p_first_event->total_events, p_second_event->total_events, p_first_event->level);
        if (!check_event_suits(new_total_events, cur_level , 3, 2)) {
            delete new_total_events;
            continue;
        }
        else
            delete new_total_events;
        Event* new_event = p_first_event->addition(p_second_event);
        if (new_event->get_support(support_denominator) < min_sup) { 
            delete new_event;
        }
        else {
            g_mutex->lock();
            dictionary->insert({ new_event->get_key(), new_event });
            g_mutex->unlock();
        }
    }
}

void worker_create_possible_pairs(int task_num, unordered_map<string, Event*>* dictionary, vector<string> first_keys, int* probabilities_array, int size_of_posebilities, int cur_level, float min_sup, int support_denominator, mutex* g_mutex) {
    vector<pair<Event*, Event*>> all_possible_combinations;
    for (int i = 0; i < first_keys.size(); i++) {
        Event* first_event = dictionary->find(first_keys[i])->second;
        string new_key_begining = first_event->get_key().substr(first_event->get_key().find(",") + 1);
        vector<string> possible_keys = get_possible_keys(new_key_begining, probabilities_array, size_of_posebilities);
        for (string key : possible_keys) {
            if (synchronized_check_if_in_dic(dictionary, key, g_mutex)) {
                Event* second_event = dictionary->find(key)->second;
                all_possible_combinations.push_back(pair<Event*, Event*>(first_event, second_event));
            }
        }
    }
    worker_iteration(task_num, dictionary, all_possible_combinations, cur_level, min_sup, support_denominator, g_mutex);
}

unordered_map<string, Event*>* tirp_algorithm(int** list_of_series, int size_of_list_series, int size_of_series, int round_in, float min_sup, int num_of_workers) {
    int cur_level = 1;
    unordered_map<string, Event*>* dictionary = first_iteration(list_of_series, size_of_list_series, size_of_series, round_in);
    remove_uncommon(dictionary, cur_level, min_sup, size_of_list_series);
    int possibilities_num = get_num_of_candidates(dictionary, cur_level);
    int* single_possibilities = get_possibilities_array(dictionary);
    cout << cur_level << " end with:" << possibilities_num << endl;
    int candidates_num = get_num_of_candidates(dictionary, cur_level);
    while (candidates_num > 1 && cur_level < 7 && candidates_num < 400000) {
        cur_level += 1;
        if (num_of_workers > 1)
            iteration2(dictionary, cur_level, single_possibilities, possibilities_num, min_sup, size_of_list_series, num_of_workers);
        else
            iteration(dictionary, cur_level, single_possibilities, possibilities_num, min_sup, size_of_list_series);
        candidates_num = get_num_of_candidates(dictionary, cur_level);
        cout << cur_level << "end with:" << candidates_num << endl;
    }
    return dictionary;
}

int* get_possibilities_array(unordered_map<string, Event*>* dictionary) {
    int array_size = get_num_of_candidates(dictionary, 1);
    int* posiibilities_array = new int[array_size];
    int possibility_index = 0;
    unordered_map<string, Event*>::iterator it;
    for (it = dictionary->begin(); it != dictionary->end(); it++)
    {
        if (it->second->level == 1) {
            posiibilities_array[possibility_index] = it->second->total_events[0];
            possibility_index++;
        }
    }
    return posiibilities_array;
}

unordered_map<string, Event*>* remove_uncommon(unordered_map<string, Event*>* dictionary, int level_to_remove, float min_support, int denominator) {
    string key_to_delete = "";
    unordered_map<string, Event*>::iterator it;
    for (it = dictionary->begin(); it != dictionary->end(); it++)
    {
        if (dictionary->count(key_to_delete))
            dictionary->erase(key_to_delete);
        key_to_delete = "";
        Event* cur_event = it->second;
        if (cur_event->level == level_to_remove) {
            if (cur_event->get_support(denominator) < min_support) {
                key_to_delete = cur_event->get_key();
                delete cur_event;
                //dictionary->erase(cur_event->get_key()); //possible error - delete inside iterator?
            }
        }
    }
    if (dictionary->count(key_to_delete))
        dictionary->erase(key_to_delete);
    return dictionary;
}

int get_num_of_candidates(unordered_map<string, Event*>* dictionary, int level) {
    int counter = 0;
    unordered_map<string, Event*>::iterator it;
    for (it = dictionary->begin(); it != dictionary->end(); it++)
    {
        if (it->second->level == level) {
            counter++;
        }
    }
    return counter;
}



int** read_activations_csv(string path, int rows_num, int size_of_row) {
    string line;                    /* string to hold each line */
    int** array = new int* [rows_num];
    ifstream f(path);
    if (!f.is_open()) {     /* validate file open for reading */
        cout << "error while opening file";
        return nullptr;
    }
    int array_row = 0;
    while (getline(f, line)) {         /* read each line */
        string val;                     /* string to hold value */
        int* row = new int[size_of_row];                /* vector for row of values */
        stringstream s(line);          /* stringstream to parse csv */
        int index = 0;
        while (getline(s, val, ',')) {   /* for each value */
            row[index] = stoi(val);  /* convert to int, add to row */
            if (val.length() > 5)
                int xxx = 0;
            index++;
        }
        array[array_row] = row;         /* add row to array */
        array_row++;
    }
    f.close();
    return array;
}
int get_number_of_rows_in_csv(string path) {
    ifstream f(path);
    string line;
    int rows_counter = 0;
    if (!f.is_open()) {     /* validate file open for reading */
        cout << "error while opening file";
        return 0;
    }
    while (getline(f, line)) {
        rows_counter++;
    }
    f.close();
    return rows_counter;
}

int get_row_length_in_csv(string path) {
    ifstream f(path);
    string line;
    int rows_counter = 0;
    int row_length = 0;
    if (!f.is_open()) {     /* validate file open for reading */
        cout << "error while opening file";
        return 0;
    }
    while (getline(f, line)) {
        string val;                     /* string to hold value */
        stringstream s(line);          /* stringstream to parse csv */
        int index = 0;
        while (getline(s, val, ',')) {   /* for each value */
            row_length++;
        }
        return row_length;
    }
    f.close();
}

int** read_activations_csv(string path) {
    int rows_counter = get_number_of_rows_in_csv(path);
    int row_length = get_row_length_in_csv(path);
    return read_activations_csv(path, rows_counter, row_length);
}

vector<Event*> get_events_from_level(unordered_map<string, Event*>* dictionary, int level) {
    vector<Event*> vec;
    unordered_map<string, Event*>::iterator it;
    for (it = dictionary->begin(); it != dictionary->end(); it++)
    {
        if (it->second->level == level) {
            vec.push_back(it->second);
        }
    }
    return vec;
}

int** concat_two_arrays(int** first_arr, int first_lines_size, int** second_arr, int second_lines_size, int line_length) {
    int** new_arr = new int* [first_lines_size + second_lines_size];
    for (int i = 0; i < first_lines_size; i++) {
        new_arr[i] = new int[line_length];
        for (int j = 0; j < line_length; j++) {
            new_arr[i][j] = first_arr[i][j];
        }
    }
    for (int i = 0; i < second_lines_size; i++) {
        new_arr[i + first_lines_size] = new int[line_length];
        for (int j = 0; j < line_length; j++) {
            new_arr[i + first_lines_size][j] = second_arr[i][j];
        }
    }
    return new_arr;
}

int get_max_level(unordered_map<string, Event*>* dictionary) {
    int max = 0;
    unordered_map<string, Event*>::iterator it;
    for (it = dictionary->begin(); it != dictionary->end(); it++)
    {
        Event* cur_event = it->second;
        if (cur_event->level > max) {
            max = cur_event->level;
        }
    }
    return max;
}

vector<string> get_possible_keys(string cur_key_begining, int* posebilities_array, int size_of_posebilities) {
    vector<string> vec;
    for (int prob_index = 0; prob_index < size_of_posebilities; prob_index++) {
        string new_key_begining = "";
        string key_to_check = cur_key_begining + to_string(posebilities_array[prob_index]) + ",";
        vec.push_back(key_to_check);
    }
    return vec;
}

void free_dictionary(unordered_map<string, Event*>* dictionary) {
    unordered_map<string, Event*>::iterator it;
    int counter = 0;
    for (it = dictionary->begin(); it != dictionary->end(); it++)
    {
        counter++;
        Event* cur_event = it->second;
        delete cur_event;
    }
    dictionary->clear();
    delete dictionary;
}

bool check_event_suits(Event* event, int min_jump, int min_level) {
    if (event->level <= min_level)
        return true;
    for (int i = 0; i < event->level; i++) {
        if (event->total_events[i] > 0) {
            if (event->total_events[i] >= min_jump)
                return true;
        }
        else {
            if (-1 * event->total_events[i] >= min_jump)
                return true;
        }
    }
    return false;
}

bool check_event_suits(int* total_events, int cur_level, int min_jump, int min_level) {
    if (cur_level <= min_level)
        return true;
    bool temp_flag = false;
    for (int i = 0; i < cur_level; i++) {
        if (total_events[i] > 0) {
            if (total_events[i] >= min_jump) {
                temp_flag = true;
                break;
            }
        }
        else {
            if (-1 * total_events[i] >= min_jump) {
                temp_flag = true;
                break;
            }
                
        }
    }
    if (!temp_flag)
        return false;
    if (cur_level == 4) { 
        if (is_patternts_similar(total_events, cur_level))
            return true;
        else
            return false;
    }else
        return true;
}

bool is_patternts_similar(int* total_events, int size)
{
    if (!Single_appliance::get_patterns()->size())
        return true;
    vector<int> events_vec;
    for (int i = 0;i < size;i++) {
        events_vec.push_back(total_events[i]);
    }
    
    for (int i = 0; i < Single_appliance::get_patterns()->size();i++) {
        double dtw_ret = dtw(Single_appliance::get_patterns()->at(i), events_vec);
        double t = Single_appliance::get_threshold();
        if (dtw_ret <= t) {
            return true;
        }
    }
    return false;
}


vector<string> get_file_names(string path, string appliance_name)
{
    namespace fs = std::filesystem;
    std::filesystem::path p;
    vector<string> filenames;
    for (const auto& file : fs::directory_iterator(path)) {
        string file_name = file.path().string();
        if (file_name.find(appliance_name) != std::string::npos) {
            cout << file.path() << endl;
            filenames.push_back(file.path().string());
        }
    }
    return filenames;
}

int** pad_zeroes(int** parray, int array_size, int row_length, int new_row_length) {
    int** new_parray = new int* [array_size];
    for (int i = 0; i < array_size; i++) {
        int* new_row = new int[new_row_length];
        for (int j = 0; j < row_length; j++) {
            new_row[j] = parray[i][j];
        }
        for (int j = row_length; j < new_row_length; j++) {
            new_row[j] = 0;
        }
        new_parray[i] = new_row;
    }
    return new_parray;
}

int** get_all_appliances_activations(string appliance_name, string path, int max_activation_size) {
    int** total_activations = nullptr;
    int current_activations_length = 0;
    vector<string> file_names = get_file_names(path, appliance_name);
    for (string file_name : file_names) {
        int** cur_activations = read_activations_csv(file_name);
        int** padded_activations = pad_zeroes(cur_activations, get_number_of_rows_in_csv(file_name), get_row_length_in_csv(file_name), max_activation_size);
        if (!total_activations)
            total_activations = padded_activations;
        else {
            int** new_activations = concat_two_arrays(total_activations, current_activations_length, padded_activations, get_number_of_rows_in_csv(file_name), get_row_length_in_csv(file_name));
            //need to free old arrrays
            total_activations = new_activations;
        }
        current_activations_length += get_number_of_rows_in_csv(file_name);
    }
    return total_activations;
}

int get_all_appliances_row_size(string appliance_name, string path) {
    vector<string> file_names = get_file_names(path, appliance_name);
    int current_activations_length = 0;
    for (string file_name : file_names) {
        current_activations_length += get_number_of_rows_in_csv(file_name);
    }
    return current_activations_length;
}

void free_2dim_array(int** parray, int num_of_rows)
{
    for (int i = 0; i < num_of_rows; i++) {
        delete[] parray[i];
    }
    delete parray;
}

double dtw(vector<int> first_arr, vector<int> second_arr) {
    int inf = 100;
    int** matrix = new int* [first_arr.size()];
    for (int i = 0; i < first_arr.size(); ++i) {
        matrix[i] = new int[second_arr.size()];
        for (int j = 0; j < second_arr.size();j++) {
            matrix[i][j] = inf;
        }
    }
    for (int i = 0; i < first_arr.size(); ++i) {
        for (int j = 0; j < second_arr.size();j++) {
            int res = abs(first_arr[i] - second_arr[j]);
            if (i > 0) {
                if (j > 0)
                    res += std::min({ matrix[i - 1][j - 1], matrix[i][j - 1], matrix[i - 1][j] });
                else
                    res += matrix[i - 1][j];
            }
            else {
                if (j > 0)
                    res += matrix[i][j - 1];
            }
            matrix[i][j] = res;
        }
    }
    /*for (int i = 0; i < first_arr.size(); ++i)
    {
        for (int j = 0; j < second_arr.size(); ++j)
        {
            std::cout << matrix[i][j] << ' ';
        }
        std::cout << std::endl;
    }*/

    int i = first_arr.size() - 1;
    int j = second_arr.size() - 1;
    int distance = 0;
    while (i || j) {
        distance += matrix[i][j];
        int min = inf;
        int direction;
        if (i > 0) {
            if (j > 0) {
                min = matrix[i - 1][j - 1];
                direction = 1;
                if (matrix[i][j - 1] < min) {
                    min = matrix[i][j - 1];
                    direction = 2;
                }
                if (matrix[i - 1][j] < min) {
                    min = matrix[i - 1][j];
                    direction = 3;
                }
            }
            else
                direction = 3;
        }
        else
            direction = 2;
        if (direction == 1) {
            i--;
            j--;
        }
        else if (direction == 2)
            j--;
        else
            i--;
    }
    for (int i = 0; i < first_arr.size(); ++i)
        delete matrix[i];
    delete matrix;
    return (double)distance / (double)(7 * min({ first_arr.size(),second_arr.size() }));
}