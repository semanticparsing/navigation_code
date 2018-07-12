#include "ConfigurationManager.h"
#include <iostream>

using namespace std;
//using namespace ConfigurationManager;

int main(){
    string filePath = "Test.ini";
    string key = "abc";
    string value = "";

    ConfigurationManager::ConfigurationManager manager;
    bool ret = manager.Init(filePath);
    if(ret){
        ConfigurationManager::ConfigurationManager::GetValue(key, value);
        cout<<"value"<<value<<endl;
    }
    cin >> key;
    return 0;
}
