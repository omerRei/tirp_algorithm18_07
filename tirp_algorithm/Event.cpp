#include "Event.h"
using namespace std;



vector<pair<int, tuple<int, vector<int>*, int>*>*>* get_possible_appearnaces(vector<pair<int, tuple<int, vector<int>*, int>*>*>* first_appearances, vector<pair<int, tuple<int, vector<int>*, int>*>*>* second_appearances)
{
    vector<pair<int, tuple<int, vector<int>*, int>*>*>* new_vector = new vector<pair<int, tuple<int, vector<int>*, int>*>*>();
    for (int i = 0; i < first_appearances->size(); i++) {
        for (int j = 0; j < second_appearances->size(); j++) {
            if (first_appearances->at(i)->first < second_appearances->at(j)->first && second_appearances->at(j)->first - first_appearances->at(i)->first < Event::get_max_size_between_events()) {
                tuple<int, vector<int>*, int>* second_old_endings_tuple = second_appearances->at(j)->second;
                vector<int>* second_old_endings_vector = get<1>(*second_old_endings_tuple);
                int new_begining_pos = -1;
                int new_ending_pos = -1;
                for (int k = get<0>(*second_old_endings_tuple); k <= get<2>(*second_old_endings_tuple); k++) {
                    if (second_old_endings_vector->at(k) - first_appearances->at(i)->first < Event::get_total_size_of_event()) {
                        if (new_begining_pos == -1)
                            new_begining_pos = k;
                        new_ending_pos = k;
                        //new_endings_vector->push_back(old_endings_vector->at(k));
                    }
                }
                if (new_begining_pos != -1) {
                    tuple<int, vector<int>*, int>* new_endings_tuple = new tuple<int, vector<int>*, int>({ new_begining_pos,second_old_endings_vector,new_ending_pos });
                    pair<int, tuple<int, vector<int>*, int>*>* new_entire_pair = new pair<int, tuple<int, vector<int>*, int>*>({ first_appearances->at(i)->first,new_endings_tuple });
                    new_vector->push_back(new_entire_pair);
                    break;
                }
            }
        }
    }
    return new_vector;
}

vector<pair<int, tuple<int, vector<int>*, int>*>*>* get_possible_appearnaces_2nd_level(vector<pair<int, tuple<int, vector<int>*, int>*>*>* first_appearances, vector<pair<int, tuple<int, vector<int>*, int>*>*>* second_appearances)
{
    vector<pair<int, tuple<int, vector<int>*, int>*>*>* new_vector = new vector<pair<int, tuple<int, vector<int>*, int>*>*>();
    for (int i = 0; i < first_appearances->size(); i++) {
        vector<int>* new_endings_vector = new vector<int>();
        for (int j = 0; j < second_appearances->size(); j++) {
            if (first_appearances->at(i)->first < second_appearances->at(j)->first && second_appearances->at(j)->first - first_appearances->at(i)->first < Event::get_max_size_between_events()) {
                new_endings_vector->push_back(second_appearances->at(j)->first);
            }
        }
        if (new_endings_vector->size()) {
            tuple<int, vector<int>*, int>* new_endings_tuple = new tuple<int, vector<int>*, int>({ 0,new_endings_vector,new_endings_vector->size() - 1 });
            pair<int, tuple<int, vector<int>*, int>*>* new_entire_pair = new pair<int, tuple<int, vector<int>*, int>*>({ first_appearances->at(i)->first,new_endings_tuple });
            new_vector->push_back(new_entire_pair);
        }
        else
            delete new_endings_vector;
    }
    return new_vector;
}

int* get_new_total_events(int* first_events, int* second_events, int cur_level) {
    int* new_total_events = new int[cur_level + 1];
    for (int i = 0; i < cur_level; i++) {
        new_total_events[i] = first_events[i];
    }
    new_total_events[cur_level] = second_events[cur_level - 1];
    return new_total_events;
}

map<int, vector<pair<int, tuple<int, vector<int>*, int>*>*>*>* get_new_map(map<int, vector<pair<int, tuple<int, vector<int>*, int>*>*>*>* first_map, map<int, vector<pair<int, tuple<int, vector<int>*, int>*>*>*>* second_map, bool is_second_level) {
    map<int, vector<pair<int, tuple<int, vector<int>*, int>*>*>*>* new_map = new map<int, vector<pair<int, tuple<int, vector<int>*, int>*>*>*>();
    map<int, vector<pair<int, tuple<int, vector<int>*, int>*>*>*>::iterator it;
    for (it = first_map->begin(); it != first_map->end(); it++)
    {
        int first_key = it->first;
        if (second_map->count(first_key)) {
            auto it1_1 = first_map->find(first_key);
            auto it1_2 = second_map->find(first_key);
            vector<pair<int, tuple<int, vector<int>*, int>*>*>* first_list = it1_1->second;
            vector<pair<int, tuple<int, vector<int>*, int>*>*>* second_list = it1_2->second;
            vector<pair<int, tuple<int, vector<int>*, int>*>*>* new_list;
            if (is_second_level)
                new_list = get_possible_appearnaces_2nd_level(first_list, second_list);
            else
                new_list = get_possible_appearnaces(first_list, second_list);
            if (new_list->size())
                new_map->insert({ first_key, new_list });
            else {
                delete new_list;
            }
        }
    }
    return  new_map;
}

float Event::get_support(int denominator)
{
    float cur_sup = float(this->appearances->size()) / denominator;
    return cur_sup;
}
//map<int, vector<pair<int, tuple<int, vector<int>*, int>*>*>*>*
void Event::add_appearnce(int series_index, int appear_index) {
    if (this->appearances->count(series_index)) {
        vector<pair<int, tuple<int, vector<int>*, int>*>*>* list = this->appearances->find(series_index)->second;
        vector<int>* v = new vector<int>();
        v->push_back(appear_index);
        tuple<int, vector<int>*, int>* t_endings = new tuple<int, vector<int>*, int>({ 0, v,0 });
        pair<int, tuple<int, vector<int>*, int>*>* p = new pair<int, tuple<int, vector<int>*, int>*>({ appear_index, t_endings });//(appear_index, v);
        list->push_back(p);
    }
    else {
        vector<pair<int, tuple<int, vector<int>*, int>*>*>* list = new vector<pair<int, tuple<int, vector<int>*, int>*>*>();
        vector<int>* v = new vector<int>();
        v->push_back(appear_index);
        tuple<int, vector<int>*, int>* t_endings = new tuple<int, vector<int>*, int>(0, v, 0);
        pair<int, tuple<int, vector<int>*, int>*>* p = new pair<int, tuple<int, vector<int>*, int>*>({ appear_index, t_endings });
        list->push_back(p);
        this->appearances->insert({ series_index,list });
    }
}

Event* Event::addition(Event* other_event)
{
    int* first_total_events = this->total_events;
    int* second_total_events = other_event->total_events;
    int* new_total_events = get_new_total_events(first_total_events, second_total_events, this->level);
    Event* new_event = new Event();
    new_event->total_events = new_total_events;
    new_event->level = this->level + 1;
    map<int, vector<pair<int, tuple<int, vector<int>*, int>*>*>*>* new_map;
    if (this->level == 1)
        new_map = get_new_map(this->appearances, other_event->appearances, true);
    else
        new_map = get_new_map(this->appearances, other_event->appearances, false);
    new_event->appearances = new_map;
    return new_event;
}

string Event::get_key()
{
    string key = "";
    for (int i = 0; i < this->level; i++) {
        key += std::to_string(this->total_events[i]);
        key += string(",");
    }
    return key;
}

/*void Event::add_appearnaces_from_other(Event* other_event)
{
    map<int, vector<pair<int, vector<int>*>*>*>::iterator it;

        for (it = other_event->appearances->begin(); it != other_event->appearances->end(); it++)
        {
            int cur_series_index = it->first;
            vector<pair<int, vector<int>*>*>* cur_vector = it->second;
            vector<pair<int, vector<int>*>*>* new_appending_vector = new vector<pair<int, vector<int>*>*>();
            for (int i = 0; i < cur_vector->size(); i++) {
                vector<int>* new_endings = new vector<int>();
                for (int j = 0; j < cur_vector->at(i)->second->size(); j++) {
                    new_endings->push_back(cur_vector->at(i)->second->at(j));
                }
                pair<int, vector<int>*>* new_p = new pair<int, vector<int>*>();
                new_p->first = cur_vector->at(i)->first;
                new_p->second = new_endings;
                new_appending_vector->push_back(new_p);
            }
            if (this->appearances->count(cur_series_index)) {
                this->appearances->find(cur_series_index)->second->insert(this->appearances->find(cur_series_index)->second->end(), new_appending_vector->begin(), new_appending_vector->end());
            }
            else {
                this->appearances->insert({ cur_series_index, new_appending_vector });
            }
        }
}*/

Event::Event()
{
    this->appearances = new map<int, vector<pair<int, tuple<int, vector<int>*, int>*>*>*>();
}

/*Event::Event(Event* cur_event) {
    this->level = cur_event->level;
    this->total_events = new int(this->level);
    for (int i = 0; i < this->level; i++) {
        this->total_events[i] = cur_event->total_events[i];
    }
    this->appearances = new map<int, vector<pair<int, vector<int>*>*>*>();
    map<int, vector<pair<int, vector<int>*>*>*>::iterator it;
    for (it = cur_event->appearances->begin(); it != cur_event->appearances->end(); it++)
    {
        int cur_series_index = it->first;
        vector<pair<int, vector<int>*>*>* cur_vector = it->second;
        vector<pair<int, vector<int>*>*>* new_vector = new vector<pair<int, vector<int>*>*>();
        for (int i = 0; i < cur_vector->size(); i++) {
            vector<int>* new_endings_vector = new vector<int>();
            for (int j = 0; j < cur_vector->at(i)->second->size();j++) {
                new_endings_vector->push_back(cur_vector->at(i)->second->at(j));
            }

            pair<int, vector<int>*>* p = new pair<int, vector<int>*>(cur_vector->at(i)->first, new_endings_vector);
            new_vector->push_back(p);
        }
        this->appearances->insert({ cur_series_index,new_vector});
    }
    counter++;
}*/

/*Event::~Event() {
    delete [] this->total_events;
    if (this->level == 1) {
        map<int, vector<pair<int, vector<int>*>*>*>::iterator it;
        for (it = this->appearances->begin(); it != this->appearances->end(); it++)
        {
            vector<pair<int, vector<int>*>*>* cur_vector = it->second;
            for (int i = 0; i < cur_vector->size(); i++) {
                delete cur_vector->at(i)->second;
            }
        }
    }
    delete this->appearances;
}*/

Event::~Event() {
    delete[] this->total_events;
    map<int, vector<pair<int, tuple<int, vector<int>*, int>*>*>*>::iterator it;
    for (it = this->appearances->begin(); it != this->appearances->end(); it++)
    {
        vector < pair<int, tuple<int, vector<int>*, int>*>*>* cur_vector = it->second;
        for (int i = 0; i < cur_vector->size(); i++) {
            if (this->level == 1 || this->level == 2) {
                vector<int>* v = get<1>(*(cur_vector->at(i)->second));
                if (v->size() > 1) {
                    int x = 0;
                }
                v->clear();
                delete v;
            }

            delete cur_vector->at(i)->second;
            delete cur_vector->at(i);
        }
        cur_vector->clear();
        delete cur_vector;
    }
    this->appearances->clear();
    delete this->appearances;
}