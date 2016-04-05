#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string>
#include <unistd.h>
#include <algorithm>
#include <numeric>


using namespace std;

void cleanupboth(string &word);


struct sortindex{
    vector<string>::iterator start;
    sortindex(vector<string>::iterator begin){ start = begin; }
    bool operator()(const int first, const int second) const {
        string left = *(start+first);
        string right = *(start+second);
        int left2 = (int)left.find_last_of('|');
        int right2 = (int)right.find_last_of('|');
        left = left.substr(0, left2);
        right = right.substr(0, right2);
        if(left!=right)
        {
            return left<right;
        }
        else
        {
            return first<second;
        }
        
    }
};

struct sorttime{
    vector<int>::iterator start;
    sorttime(vector<int>::iterator begin){ start = begin; }
    bool operator()(const int left, const int &right) const {
        return *(start+left)<*(start+right);
    }
};


int main(int argc, char **argv) {
    int option_index = 0;
    
    static struct option long_options[] =
    {
        {"help", no_argument,0,'h'},
        {0, 0, 0, 0}
    };
    
    if(getopt_long(argc, argv, "h", long_options, &option_index)!= -1)
    {
        cout<<"help"<<'\n';
        return 0;
    }
    
    ostringstream oss;
    ostream& output = oss;
    
    istringstream ss;
    unsigned int entryID = 0;
    
    size_t split1 = 14;
    size_t split2;
    string logfile(*(argv+1));
    string line, catalog, keyword, oneword;
    
    ifstream infile;
    infile.open(logfile);
    
    
    vector<string> log;
    
    unordered_map<string, vector<int>> catalogs;
    unordered_map<string, vector<int>> keywords;
    
    while (getline(infile,line))
    {
        log.push_back(line);
        entryID++;
    }
    
    infile.close();
    output<<entryID<<" entries read"<<'\n';
    
    vector<string> timestamps(entryID);
    vector<int> logseq(entryID);
    vector<int> logindex(entryID);
    sorttime seq(logseq.begin());

    bool t = 0;
    bool c = 0;
    bool k = 0;
    bool firstinc;
    bool havesearch = 0;
    bool havesort = 0;
    char command;
    string search;
    int search_num;
    vector<int> excerpt;
    vector<int> searchresult;
    vector<int> searchresult_temp;
    vector<int> searchoneword;
    istringstream sl;
    string time1, time2;
    string lastcata;
    string linesort;
    while (1)
    {
        getline(cin,line);
        output<<"% ";
        sl.clear();
        sl.str(line);
        sl >> command;
        
        if (command!='#')
        {
            switch (command) {
                case 't':
                    searchresult.clear();
                    sl>>search;
                    time1 = search.substr(0, split1);
                    time2 = search.substr(split1+1);
                    if (time1.length()!=14 || time2.length()!=14) {
                        cerr<<"Error: Invalid command"<<'\n';
                    }
                    else{
                        if (t==0) {
                            if (havesort==0) {
                                havesort = 1;
                                iota(logindex.begin(), logindex.end(), 0);
                                sort(logindex.begin(), logindex.end(), sortindex(log.begin()));
                                
                                for (unsigned int i = 0; i<entryID; i++) {
                                    logseq[logindex[i]] = i;
                                }
                            }

                            t = 1;
                            havesearch = 1;
                            for (unsigned int i = 0; i<entryID; i++) {
                                linesort = log[logindex[i]];
                                timestamps[i] = linesort.substr(0,14);
                            }
                            
                        }
                        
                        searchresult.assign(logindex.begin()+(lower_bound(timestamps.begin(), timestamps.end(), time1)-timestamps.begin()), logindex.begin()+(lower_bound(timestamps.begin(), timestamps.end(), time2)-timestamps.begin()));
                        output<<searchresult.size()<<" entries found"<<'\n';
                    }
                    break;
                case 'c':
                    if (c==0) {
                        if (havesort==0) {
                            havesort = 1;
                            iota(logindex.begin(), logindex.end(), 0);
                            sort(logindex.begin(), logindex.end(), sortindex(log.begin()));
                            
                            for (unsigned int i = 0; i<entryID; i++) {
                                logseq[logindex[i]] = i;
                            }
                        }

                        c = 1;
                        havesearch = 1;
                        for (unsigned int i = 0; i<entryID; i++) {
                            linesort = log[logindex[i]];
                            transform(linesort.begin(), linesort.end(), linesort.begin(),::tolower);
                            split2 = linesort.find_last_of('|');
                            catalog = linesort.substr(split1+1, split2-split1-1);
                            catalogs[catalog].push_back(logindex[i]);
                        }
                    }
                    searchresult.clear();
                    search = line.substr(2);
                    transform(search.begin(), search.end(), search.begin(),::tolower);
                    output<< catalogs[search].size() << " entries found"<<'\n';
                    searchresult.assign(catalogs[search].begin(), catalogs[search].end());
                    break;
                case 'k':
                    if (k==0) {
                        if (havesort==0) {
                            havesort = 1;
                            iota(logindex.begin(), logindex.end(), 0);
                            sort(logindex.begin(), logindex.end(), sortindex(log.begin()));
                            
                            for (unsigned int i = 0; i<entryID; i++) {
                                logseq[logindex[i]] = i;
                            }
                        }

                        k = 1;
                        havesearch = 1;
                        for (unsigned int i = 0; i<entryID; i++) {
                            cleanupboth(keyword = log[logindex[i]].substr(split1+1));
                            ss.clear();
                            ss.str(keyword);
                            while (ss>>oneword) {
                                if (keywords[oneword].empty() || logindex[i]!= keywords[oneword].back()) {
                                    keywords[oneword].push_back(logindex[i]);
                                }
                            }
                        }
                    }
                    
                    firstinc = 1;
                    searchresult.clear();
                    cleanupboth(search = line.substr(2));
                    ss.clear();
                    ss.str(search);
                    while (ss>>oneword) {
                        searchoneword.assign(keywords[oneword].begin(), keywords[oneword].end());
                        if (firstinc == 1) {
                            searchresult.assign(searchoneword.begin(), searchoneword.end());
                            firstinc = 0;
                        }
                        else
                        {
                            searchresult_temp.assign(searchresult.begin(), searchresult.end());
                            searchresult.clear();
                            set_intersection(searchresult_temp.begin(), searchresult_temp.end(), searchoneword.begin(), searchoneword.end(), back_inserter(searchresult), seq);
                        }
                    }
                    output<< searchresult.size() << " entries found"<<'\n';
                    break;
                case 'a':
                    sl>>search_num;
                    if (search_num<0 || search_num>=(int)log.size()) {
                        cerr<<"Error: Invalid command"<<'\n';
                    }
                    else
                    {
                        
                        excerpt.push_back(search_num);
                        output<<"log entry " << search_num<<" appended"<<'\n';
                    }
                    
                    break;
                case 'r':
                    if (havesearch==0) {
                        cerr<<"Error: Invalid command"<<'\n';
                    }
                    else
                    {
                        excerpt.insert(excerpt.end(), searchresult.begin(), searchresult.end());
                        output<<searchresult.size()<<" log entries appended"<<'\n';
                    }
                    
                    break;
                case 'd':
                    sl>>search_num;
                    if (search_num<0 || search_num>=(int)excerpt.size()) {
                        cerr<<"Error: Invalid command"<<'\n';
                    }
                    else
                    {
                        excerpt.erase(excerpt.begin()+search_num);
                        output<<"excerpt list entry "<<search_num<<" deleted"<<'\n';
                    }
                    break;
                case 'b':
                    
                    sl>>search_num;
                    if (search_num<0 || search_num>=(int)excerpt.size()) {
                        cerr<<"Error: Invalid command"<<'\n';
                    }
                    else
                    {

                        excerpt.insert(excerpt.begin(), excerpt[search_num]);
                        excerpt.erase(excerpt.begin()+search_num+1);
                        output<<"excerpt list entry "<<search_num<<" moved"<<'\n';
                        
                    }
                    break;
                case 'e':
                    sl>>search_num;
                    if (search_num<0 || search_num>=(int)excerpt.size()) {
                        cerr<<"Error: Invalid command"<<'\n';
                    }
                    else
                    {

                        excerpt.push_back(excerpt[search_num]);
                        excerpt.erase(excerpt.begin()+search_num);
                        output<<"excerpt list entry "<<search_num<<" moved"<<'\n';
                    }
                    
                    break;
                case 's':
                    output<<"excerpt list sorted"<<'\n';
                    sort(excerpt.begin(), excerpt.end(), seq);
                    break;
                case 'l':
                    excerpt.clear();
                    output<<"excerpt list cleared"<<'\n';
                    break;
                case 'g':
                    if (havesearch==0) {
                        cerr<<"Error: Invalid command"<<'\n';
                    }
                    else
                    {
                        for (unsigned int i = 0 ; i<searchresult.size(); i++) {
                            output<<searchresult[i]<<"|"<<log[searchresult[i]]<<'\n';
                        }
                    }
                    break;
                case 'p':
                    for (unsigned int i = 0; i<excerpt.size(); i++) {
                        output<<i<<"|"<<excerpt[i]<<"|"<<log[excerpt[i]]<<'\n';
                    }
                    break;
                case 'q':
                    cout << oss.str();
                    return 0;
                    break;
                default:
                    cerr<<"Error: Invalid command"<<'\n';
                    break;
            }
            
        }
    }
    
    return 0;
}


void cleanupboth(string &word) {
    size_t i = 0;
    bool done = (i >= word.length());
    while (!done) {
        if (ispunct(word[i])) {
            word[i] = ' ';
        }
        else {
            word[i] = tolower(word[i]);
        }
        ++i;
        done = (i >= word.length());
    }
}




