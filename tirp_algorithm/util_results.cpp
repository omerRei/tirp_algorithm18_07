#include "util_results.h"
#include "util_functions.h"
/*string get_event_distances(Event* first_event, unordered_map<string, Event*>* dictionary) {
    string ret_string = "";
    for (int i = 1; i < first_event->level; i++) {
        string first_key = "";
        string second_key = "";
        for (int j = 0; j < i; j++) {
            first_key += to_string(first_event->total_events[j]) + ",";
        }
        for (int j = i; j < first_event->level; j++) {
            second_key += to_string(first_event->total_events[j]) + ",";
        }
        Event* first_event = dictionary->find(first_key)->second;
        Event* second_event = dictionary->find(second_key)->second;
        pair<int, int> min_max = find_min_max_gap(first_event->appearances, second_event->appearances);
        ret_string += to_string(first_event->total_events[i - 1]) + '(' + to_string(min_max.first) + ',' + to_string(min_max.second) + ')' + " ";
    }
    ret_string += to_string(first_event->total_events[first_event->level - 1]);
    return ret_string;
}*/

/*pair<int, int> find_min_max_gap(map<int, vector<pair<int, tuple<int, vector<int>*, int>*>*>*>* first_event_appearances, map<int, vector<pair<int, tuple<int, vector<int>*, int>*>*>*>* second_event_appearances) {
    map<int, vector<pair<int, tuple<int, vector<int>*, int>*>*>*>::iterator it;
    int min = 10000000000;
    int max = 0;
    for (it = first_event_appearances->begin(); it != first_event_appearances->end(); it++)
    {
        int cur_series = it->first;
        vector<pair<int, tuple<int, vector<int>*, int>*>*>* first_serieses_vector = it->second;
        if (!second_event_appearances->count(cur_series))
            continue;
        vector<pair<int, tuple<int, vector<int>*, int>*>*>* second_vector = second_event_appearances->find(cur_series)->second;
        for (int k = 0;k < second_vector->size(); k++) {
            vector<int>* second_ending_vector = get<1>(*(first_serieses_vector->at(k)->second));
            int second_ending_v_start = get<0>(*(first_serieses_vector->at(k)->second));
            int second_ending_v_end = get<2>(*(first_serieses_vector->at(k)->second));
            for (int i = 0; i < first_serieses_vector->size(); i++) {
                vector<int>* first_ending_vector = get<1>(*(first_serieses_vector->at(i)->second));
                int first_ending_v_start = get<0>(*(first_serieses_vector->at(k)->second));
                int first_ending_v_end = get<2>(*(first_serieses_vector->at(k)->second));
                for (int j = first_ending_v_start; j <= first_ending_v_end; j++) {
                    if (second_vector->at(k)->first > first_ending_vector->at(j)) {
                        if (second_vector->at(k)->first - first_serieses_vector->at(i)->second->at(j) > Event::get_max_size_between_events())
                            continue;
                        int gap = second_vector->at(k)->first - first_vector->at(i)->second->at(j);
                        if (gap < min)
                            min = gap;
                        if (gap > max)
                            max = gap;
                    }
                }
            }
        }
    }
    pair<int, int> minMax = pair<int, int>(min, max);
    return minMax;
}*/

vector<vector<int>> get_appliance_patterns(int** serieses, int num_of_series, int series_size, float round_num, float threshold, bool to_print, int num_of_workers, string output_file_name) {
    int levels_to_include = 6;
    int min_val_in_pattern = 5;
    int error_from_completion = 2;
    unordered_map<string, Event*>* dictionary = tirp_algorithm(serieses, num_of_series, series_size, round_num, threshold, num_of_workers);
    vector<vector<int>> events;
    int max_level = get_max_level(dictionary);
    for (int level = 3; level <= max_level; level++) {
        vector<Event*> temp_v = get_events_from_level(dictionary, level);
        for (int i = 0; i < temp_v.size(); i++) {
            vector<int> events_chain;
            bool negative_pattern = false;
            bool important_pattern = false;
            int sum = 0;
            for (int k = 0; k < temp_v[i]->level; k++) {
                if (k == 0 && temp_v[i]->total_events[k] < 0)
                    negative_pattern = true;
                events_chain.push_back(temp_v[i]->total_events[k]);
                sum += temp_v[i]->total_events[k];
                if (sum < 0)
                    negative_pattern = true;
                if (sum >= min_val_in_pattern)
                    important_pattern = true;
            }
            if (!negative_pattern && sum < error_from_completion && important_pattern) {
                events.push_back(events_chain);
                if (to_print) {
                    cout << temp_v[i]->get_key() << endl << " ~found in:" << temp_v[i]->appearances->size() << ", support:" << temp_v[i]->get_support(num_of_series) << endl;
                }
            }
        }
    }
    cout << "found before filtering:" << events.size() << endl;
    vector<vector<int>> filtered_events = get_vectors_without_subsets4(events, 8);
    cout << "found after filtering:" << filtered_events.size() << endl;
    if (max_level < levels_to_include)
        max_level = levels_to_include;
    vector<vector<int>> outs = sync_get_all_possible_outputs(dictionary, filtered_events, min_val_in_pattern, max_level - levels_to_include, 8);
    write_activation_to_csv(outs, output_file_name);
    cout << output_file_name << " finished succesfully" << endl;
    free_dictionary(dictionary);
    return filtered_events;
}

vector<vector<int>> get_specific_appliance_patterns(int** serieses, int num_of_series, int series_size, float round_num, float threshold, bool to_print, int num_of_workers, int level_to_get) {
    int min_val_in_pattern = 2;
    int error_from_completion = 4;
    unordered_map<string, Event*>* dictionary = tirp_algorithm(serieses, num_of_series, series_size, round_num, threshold, num_of_workers);
    vector<vector<int>> events;
    vector<Event*> temp_v = get_events_from_level(dictionary, level_to_get);
    for (int i = 0; i < temp_v.size(); i++) {
        vector<int> events_chain;
        bool important_pattern = false;
        int sum = 0;
        for (int k = 0; k < temp_v[i]->level; k++) {

            events_chain.push_back(temp_v[i]->total_events[k]);
            sum += temp_v[i]->total_events[k];
            if (sum >= min_val_in_pattern)
                important_pattern = true;
        }
        if (sum < error_from_completion && important_pattern) {
            events.push_back(events_chain);
            if (to_print) {
                cout << temp_v[i]->get_key() << endl << " ~found in:" << temp_v[i]->appearances->size() << ", support:" << temp_v[i]->get_support(num_of_series) << endl;
            }
        }

    }
    free_dictionary(dictionary);
    return events;
}



vector<vector<int>> get_repeating_vectors(vector<vector<int>> vectors, int min_repetitions) {
    vector<vector<int>> repeating_vectors;
    for (int i = 0; i < vectors.size(); i++) {
        int rep_counter = 0;
        for (int j = 0; j < vectors.size(); j++) {
            if (i == j || vectors[i].size() != vectors[j].size())
                continue;
            for (int k = 0; k < vectors[i].size(); k++) {
                if (vectors[i][k] != vectors[j][k])
                    break;
                if (k == vectors[i].size() - 1) {
                    rep_counter++;
                }
            }
        }
        if (rep_counter >= min_repetitions && !exist_in_list_of_vectors(repeating_vectors, vectors[i]))
            repeating_vectors.push_back(vectors[i]);
    }
    return repeating_vectors;
}

bool exist_in_list_of_vectors(vector<vector<int>> list, vector<int> cur_vector) {
    for (int i = 0; i < list.size(); i++) {
        if (list[i].size() != cur_vector.size())
            continue;
        for (int k = 0; k < cur_vector.size(); k++) {
            if (list[i][k] != cur_vector[k])
                break;
            if (k == list[i].size() - 1) {
                return true;
            }
        }
    }
    return false;
}

bool IsSubset(vector<int> first, vector<int> second)
{
    if (second.size() > first.size() || first == second)
        return false;
    int counter = 0;
    int j = 0;
    for (int i = 0; i < first.size(); i++) {
        if (second[j] == first[i]) {
            counter++;
            j++;
        }
        if (j == second.size())
            break;
    }
    if (counter == second.size())
        return true;
    return false;
}

vector<vector<int>> get_vectors_without_subsets(vector<vector<int>> list) {
    vector<vector<int>> new_list;
    for (int i = 0; i < list.size();i++) {
        bool is_subset = false;
        for (int j = 0; j < list.size(); j++) {
            if (i != j) {
                if (IsSubset(list[j], list[i])) {
                    is_subset = true;
                    break;
                }
            }
        }
        if (!is_subset)
            new_list.push_back(list[i]);
    }
    return new_list;
}

vector<vector<int>> get_vectors_without_subsets2(vector<vector<int>> list, int num_of_workers) {
    mutex* mymutex = new mutex();
    vector<vector<vector<int>>> all_vectors;
    int size_of_vector = int(list.size() / num_of_workers);
    if (!size_of_vector)
        size_of_vector = 1;
    for (size_t i = 0; i < list.size(); i += size_of_vector) {
        int last;
        if (list.size() < i + size_of_vector)
            last = list.size();
        else
            last = i + size_of_vector;
        all_vectors.emplace_back(list.begin() + i, list.begin() + last);
    }
    vector<thread> threads;
    vector<vector<int>> new_list;
    for (int i = 0; i < all_vectors.size(); i++) {
        threads.push_back(thread(worker_subset_handler, list, all_vectors[i], &new_list, mymutex));
    }
    for (int i = 0; i < threads.size(); i++) {
        threads[i].join();
    }
    return new_list;
}

unordered_set<string> create_hashmap(vector<vector<int>> list) {
    unordered_set<string> hashmap;
    for (vector<int> vec : list) {
        string key = create_key(vec);
        hashmap.insert(key);
    }
    return hashmap;
}

string create_key(vector<int> vec) {
    string ret = "";
    for (int x : vec) {
        ret += to_string(x) + ",";
    }
    return ret;
}

vector<vector<int>> get_vectors_without_subsets3(vector<vector<int>> list, int num_of_workers) {
    mutex* mymutex = new mutex();
    vector<vector<vector<int>>> all_vectors;
    int size_of_vector = int(list.size() / num_of_workers);
    if (!size_of_vector)
        size_of_vector = 1;
    for (size_t i = 0; i < list.size(); i += size_of_vector) {
        int last;
        if (list.size() < i + size_of_vector)
            last = list.size();
        else
            last = i + size_of_vector;
        all_vectors.emplace_back(list.begin() + i, list.begin() + last);
    }
    vector<thread> threads;
    vector<vector<int>> new_list;
    for (int i = 0; i < all_vectors.size(); i++) {
        threads.push_back(thread(worker_subset_handler, list, all_vectors[i], &new_list, mymutex));
    }
    for (int i = 0; i < threads.size(); i++) {
        threads[i].join();
    }
    return new_list;
}

vector<vector<int>> get_vectors_without_subsets4(vector<vector<int>> list, int num_of_workers) {

    unordered_set hashmap = create_hashmap(list);
    vector<vector<vector<int>>> all_vectors;
    int size_of_vector = int(list.size() / num_of_workers);
    if (!size_of_vector)
        size_of_vector = 1;
    for (size_t i = 0; i < list.size(); i += size_of_vector) {
        int last;
        if (list.size() < i + size_of_vector)
            last = list.size();
        else
            last = i + size_of_vector;
        all_vectors.emplace_back(list.begin() + i, list.begin() + last);
    }
    vector<vector<int>> new_list;
    vector < future < vector<vector<int>>>> futures;
    for (int i = 0; i < all_vectors.size(); i++) {
        vector<int> poseb{ 1,2,3,4,5,6,7,-1,-2,-3,-4,-5,-6,-7 };
        futures.emplace_back(async(worker_subset_handler3, all_vectors[i], hashmap, poseb));
        //threads.push_back(thread(worker_subset_handler, list, all_vectors[i], &new_list, mymutex));
    }

    for (int i = 0; i < futures.size(); i++) {
        vector<vector<int>> ret_vec = futures[i].get();
        new_list.insert(new_list.end(), ret_vec.begin(), ret_vec.end());
        //threads[i].join();
        int x = 0;
    }
    return new_list;
}

void worker_subset_handler(vector<vector<int>> big_list, vector<vector<int>> small_list, vector<vector<int>>* new_list, mutex* mymutex) {
    for (int i = 0; i < small_list.size();i++) {
        bool is_subset = false;
        for (int j = 0; j < big_list.size(); j++) {
            if (IsSubset(big_list[j], small_list[i])) {
                is_subset = true;
                break;
            }
        }
        if (!is_subset) {
            mymutex->lock();
            new_list->push_back(small_list[i]);
            mymutex->unlock();
        }
    }
}

vector<vector<int>> worker_subset_handler2(vector<vector<int>> big_list, vector<vector<int>> small_list) {
    vector<vector<int>> new_list;
    for (int i = 0; i < small_list.size();i++) {
        bool is_subset = false;
        for (int j = 0; j < big_list.size(); j++) {
            if (IsSubset(big_list[j], small_list[i])) {
                is_subset = true;
                break;
            }
        }
        if (!is_subset) {
            new_list.push_back(small_list[i]);
        }
    }
    return new_list;
}

vector<vector<int>> worker_subset_handler3(vector<vector<int>> big_list, unordered_set<string> hashmap, vector<int> possebilities) {
    vector<vector<int>> new_list;
    for (int i = 0; i < big_list.size();i++) {
        bool is_subset = false;
        vector<int> cur_vec = big_list[i];
        for (int pos : possebilities) {
            cur_vec.push_back(pos);
            string cur_key = create_key(cur_vec);
            cur_vec.pop_back();
            if (hashmap.find(cur_key) != hashmap.end()) {
                is_subset = true;
                break;
            }
        }
        if (!is_subset) {
            new_list.push_back(big_list[i]);
        }
        else {
            int xx = 0;
        }
    }
    return new_list;
}

/*vector<vector<int>> get_possible_parents(vector<vector<int>> cur_vec, vector<int> possebilities) {
    vector<vector<int>> new_vectors;
    for (int pos : possebilities) {
            vector<int> temp_vec = { pos };
            temp_vec.insert(temp_vec.end(), cur_vec.begin(), cur_vec.end());
            temp_vec.push_back(-pos);
            new_vectors.push_back(temp_vec);
            for (int pos2 : possebilities) {
                vector<int> temp_vec2 = { pos2 };
                temp_vec2.insert(temp_vec2.end(), temp_vec.begin(), temp_vec.end());
                temp_vec2.push_back(-pos2);
                new_vectors.push_back(temp_vec2);
            }
    }
}*/

vector<vector<int>> worker_subset_handler4(vector<vector<int>> big_list, unordered_set<string> hashmap, vector<int> possebilities) {
    vector<vector<int>> new_list;
    for (int i = 0; i < big_list.size();i++) {
        bool is_subset = false;
        vector<int> cur_vec = big_list[i];
        for (int pos : possebilities) {
            cur_vec.push_back(pos);
            string cur_key = create_key(cur_vec);
            cur_vec.pop_back();
            if (hashmap.find(cur_key) != hashmap.end()) {
                is_subset = true;
                break;
            }
        }
        if (!is_subset) {
            new_list.push_back(big_list[i]);
        }
        else {
            int xx = 0;
        }
    }
    return new_list;
}
/*vector<vector<int>> generate_output_from_event(string key, unordered_map<string, Event*>* dictionary)
{
    vector<vector<int>> output; // vector<pair<pair<int day, int start_minute>,vector<int>>
    if (!check_if_in_dic(dictionary, key))
        return output;
    Event* event = dictionary->find(key)->second;
    vector<Event*> sub_events = generate_sub_events(event, dictionary);
    map<int, vector<pair<int, vector<int>*>*>*>::iterator it;
    for (it = event->appearances->begin(); it != event->appearances->end(); it++)
    {
        vector<vector<int>> series_output;
        vector<pair<int, vector<int>*>*>* series_indexes = it->second;
        for (int i = 0; i < series_indexes->size(); i++) {
            series_output.push_back(vector<int>{series_indexes->at(i)->first});
        }
        for (int i = 0; i < sub_events.size(); i++) {
            Event* second_event = sub_events[i];
            series_indexes = second_event->appearances->find(it->first)->second;
            vector<vector<int>> new_series_output;
            for (int j = 0; j < series_output.size(); j++) {
                for (int k = 0; k < series_indexes->size(); k++) {
                    if (series_indexes->at(k)->first > series_output[j].back()) {
                        vector<int> new_vec = series_output[j];
                        new_vec.push_back(series_indexes->at(k)->first);
                        if(new_vec.back() - new_vec[0] < Event::get_total_size_of_event())
                            new_series_output.push_back(new_vec);
                    }
                }
            }
            series_output.clear();
            for (int j = 0; j < new_series_output.size(); j++){
                series_output.push_back(new_series_output[j]); // need to check if deep copy is made
            }
        }
        for (int i = 0; i < series_output.size(); i++) {
            output.push_back(series_output[i]);
        }
    }
    return generate_final_outputs(output, event);
}

vector<vector<int>> generate_final_outputs(vector<vector<int>> pre_outputs, Event* cur_event) {
    vector<vector<int>> final_outputs;
    for (int i = 0; i < pre_outputs.size(); i++) {
        vector<int> cur_vector = pre_outputs[i];
        vector<int> new_vector = vector<int>{ cur_event->total_events[0]};
        for (int j = 1; j < cur_vector.size(); j++) {
            for (int k = 0; k < cur_vector[j] - cur_vector[j-1] - 1; k++) {
                new_vector.push_back(0);
            }
            new_vector.push_back(cur_event->total_events[j]);
        }
        final_outputs.push_back(new_vector);
    }
    return final_outputs;
}*/

vector<Event*> generate_sub_events(Event* event, unordered_map<string, Event*>* dictionary) {
    vector<Event*> sub_events;
    for (int i = 1; i < event->level; i++) {
        string second_key = "";
        for (int j = i; j < event->level; j++) {
            second_key += to_string(event->total_events[j]) + ",";
        }
        Event* second_event = dictionary->find(second_key)->second;
        sub_events.push_back(second_event);
    }
    return sub_events;
}

void write_activation_to_csv(vector<vector<int>> activations, string path) {
    ofstream file;
    //string dir = "outs/";
    //file.open(dir + path);
    file.open(path);
    for (int i = 0; i < activations.size(); i++) {
        for (int j = 0; j < activations[i].size() - 1; j++) {
            file << activations[i][j] << ",";
        }
        file << activations[i].back() << endl;
    }
    file.close();
}

vector<vector<int>> get_all_possible_outputs(unordered_map<string, Event*>* dictionary, vector<vector<int>> cur_outputs, int min_treshold, int min_level) {
    vector<vector<int>> total_outputs;
    for (int i = 0; i < cur_outputs.size(); i++) {
        if (cur_outputs[i].size() < min_level)
            continue;
        bool flag = false;
        for (int j = 0; j < cur_outputs[i].size(); j++) {
            if (cur_outputs[i][j] >= min_treshold )
                flag = true;
        }
        if (flag) {
            string key = "";
            for (int j = 0; j < cur_outputs[i].size(); j++) {
                key += std::to_string(cur_outputs[i][j]) + ",";
            }
            Event* event = dictionary->find(key)->second;
            vector<vector<int>> outs = generate_output_from_event(event->get_key(), dictionary);
            total_outputs.insert(total_outputs.end(), outs.begin(), outs.end());
        }
    }
    return total_outputs;
}

vector<vector<int>> sync_get_all_possible_outputs(unordered_map<string, Event*>* dictionary, vector<vector<int>> cur_outputs, int min_treshold, int min_level, int num_of_workers) {
    vector<Event*> total_events;
    for (int i = 0; i < cur_outputs.size(); i++) {
        if (cur_outputs[i].size() < min_level)
            continue;
        bool flag = false;
        for (int j = 0; j < cur_outputs[i].size(); j++) {
            if (cur_outputs[i][j] >= min_treshold)
                flag = true;
        }
        if (flag) {
            string key = "";
            for (int j = 0; j < cur_outputs[i].size(); j++) {
                key += std::to_string(cur_outputs[i][j]) + ",";
            }
            Event* event = dictionary->find(key)->second;
            total_events.push_back(event);
        }
    }
    vector<vector<Event*>> all_vectors;
    int size_of_vector = int(total_events.size() / num_of_workers);
    if (!size_of_vector)
        size_of_vector = 1;
    for (size_t i = 0; i < total_events.size(); i += size_of_vector) {
        int last;
        if (total_events.size() < i + size_of_vector)
            last = total_events.size();
        else
            last = i + size_of_vector;
        all_vectors.emplace_back(total_events.begin() + i, total_events.begin() + last);
    }
    vector < future < vector<vector<int>>>> futures;
    for (int i = 0; i < all_vectors.size(); i++) {
        vector<int> poseb{ 1,2,3,4,5,6,7,-1,-2,-3,-4,-5,-6,-7 };
        futures.emplace_back(async(worker_generate_output_from_events, all_vectors[i], dictionary));
    }
    vector<vector<int>> total_outputs;
    for (int i = 0; i < futures.size(); i++) {
        vector<vector<int>> ret_vec = futures[i].get();
        total_outputs.insert(total_outputs.end(), ret_vec.begin(), ret_vec.end());
    }
    return total_outputs;
}



vector<vector<int>> worker_generate_output_from_events(vector<Event*> events, unordered_map<string, Event*>* dictionary) {
    vector<vector<int>> total_outputs;
    for (Event* event : events) {
        vector<vector<int>> outs = generate_output_from_event(event->get_key(), dictionary);
        total_outputs.insert(total_outputs.end(), outs.begin(), outs.end());
    }
    return total_outputs;
}



void user_interaction(unordered_map<string, Event*>* dictionary) {
    string ans;
    while (true) {
        cout << "please enter a key;" << endl;
        cin >> ans;
        if (ans == "-1")
            break;
        if (!dictionary->count(ans)) {
            cout << "wrong input, try again or tap -1 for exit" << endl;
            continue;
        }
        Event* event = dictionary->find(ans)->second;
        cout << "insert file name ending with .csv ;" << endl;
        cin >> ans;
        string path = "outs/" + ans;
        vector<vector<int>> outs = generate_output_from_event(event->get_key(), dictionary);
        write_activation_to_csv(outs, ans);
    }
}


vector<vector<int>> generate_output_from_event(string key, unordered_map<string, Event*>* dictionary)
{
    const int max_comb = 3;
    vector<vector<int>> final_output;
    if (!check_if_in_dic(dictionary, key))
        return final_output;
    Event* event = dictionary->find(key)->second;
    vector<Event*> sub_events = generate_sub_events(event, dictionary);
    map<int, vector<pair<int, tuple<int, vector<int>*, int>*>*>*>::iterator it;
    for (it = event->appearances->begin(); it != event->appearances->end(); it++)
    {
        int day = it->first;
        vector<vector<int>> series_output;
        vector<pair<int, tuple<int, vector<int>*, int>*>*>* series_indexes = it->second;
        for (int i = 0; i < series_indexes->size(); i++) {
            series_output.push_back(vector<int>{series_indexes->at(i)->first});
        }
        for (int i = 0; i < sub_events.size(); i++) {
            Event* second_event = sub_events[i];
            series_indexes = second_event->appearances->find(day)->second;
            vector<vector<int>> new_series_output;
            for (int j = 0; j < series_output.size(); j++) {
                //
                if (series_indexes->size() > max_comb ) {
                    int jump = series_indexes->size() / max_comb;
                    for (int k = 0; k < series_indexes->size(); k += jump) {
                        if (series_indexes->at(k)->first > series_output[j].back()) { 
                            vector<int> new_vec = series_output[j];
                            new_vec.push_back(series_indexes->at(k)->first);
                            if (new_vec.back() - new_vec[0] < Event::get_total_size_of_event())
                                new_series_output.push_back(new_vec);
                        }
                    }
                }
                else {
                    for (int k = 0; k < series_indexes->size(); k++) {
                        if (series_indexes->at(k)->first > series_output[j].back()) {
                            vector<int> new_vec = series_output[j];
                            new_vec.push_back(series_indexes->at(k)->first);
                            if (new_vec.back() - new_vec[0] < Event::get_total_size_of_event())
                                new_series_output.push_back(new_vec);
                        }
                    }
                }
            }
            series_output.clear();
            for (int j = 0; j < new_series_output.size(); j++) {
                series_output.push_back(new_series_output[j]);
            }
        }
        vector<vector<int>> final_series_output = generate_final_outputs(day, series_output, event);
        for (int i = 0; i < final_series_output.size(); i++) {
            final_output.push_back(final_series_output[i]);
        }
    }
    return final_output;
}

vector<vector<int>> generate_final_outputs(int day, vector<vector<int>> pre_outputs, Event* cur_event) {
    vector<vector<int>> final_outputs;
    for (int i = 0; i < pre_outputs.size(); i++) {
        vector<int> cur_vector = pre_outputs[i];
        vector<int> new_vector = vector<int>{ day };
        new_vector.push_back(cur_vector[0]);
        new_vector.push_back(cur_event->total_events[0]);
        for (int j = 1; j < cur_vector.size(); j++) {
            for (int k = 0; k < cur_vector[j] - cur_vector[j - 1] - 1; k++) {
                new_vector.push_back(0);
            }
            new_vector.push_back(cur_event->total_events[j]);
        }
        final_outputs.push_back(new_vector);
    }
    return final_outputs;
}

void listFiles(std::string path)
{
    for (auto& dirEntry : std::filesystem::recursive_directory_iterator(path)) {
        if (!dirEntry.is_regular_file()) {
            std::cout << "Directory: " << dirEntry.path().string() << std::endl;
            continue;
        }
        std::filesystem::path file = dirEntry.path();
        std::cout << "Filename: " << file.filename() << " extension: " << file.extension() << std::endl;
        string ppp = file.filename().string();
        string ttt = file.parent_path().string();
        int x = 0;
    }
}