#include <bits/stdc++.h>
#include <pthread.h>

using namespace std;

int main(int argc, const char** argv){
    string in_name = argv[1];
    string out_name = argv[2];
    string knl_size = argv[3];
    string num_threads = argv[4];
    cout << in_name << '\n';
    cout << out_name << '\n';
    cout << knl_size << '\n';
    cout << num_threads << '\n';
}