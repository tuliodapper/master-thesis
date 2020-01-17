#include "sim.h"
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

SimMobility::SimMobility(){}
void SimMobility::Initialize(int self, string loadPath){

    stringstream sfile;
    string str;
    ifstream file;
    char delim = ';';
    string::size_type sz;

    // cout << "SimMobility::Initialize" << endl;
    sfile << loadPath << "sim-mob.ini";
    file.open(sfile.str());
    // cout << "open" << endl;
    while(!file.eof()) // To get you all the lines.
    {
        getline(file, str); // Saves the line in STRING.
        //  cout << "line=" << str << endl;
        if (str.compare("") != 0){
            //  cout << "ok" << endl;
            strings_t str_split = split(str, delim);
            int nodeId = stoi(str_split.at(1), &sz);
            //  cout << "nodeId=" << nodeId << endl;
            if (self == nodeId) {
                mobility_info_t mobility;
                mobility.delay = stod(str_split.at(2));
                mobility.x = stod(str_split.at(3));
                mobility.y = stod(str_split.at(4));
                mobilities.push_back(mobility);
            }
        }
    }
    file.close();

    pos = 0;
}


bool SimMobility::isThereNextMobilityInfo(){
    return ((mobilities.size()!=0) && (pos <= mobilities.size()-1));
}

mobility_info_t SimMobility::retNextMobilityInfo(){
    if (!(isThereNextMobilityInfo()))
        pos = 0;
    pos = pos + 1;
    return mobilities.at(pos-1);
}

SimMessage::SimMessage(){}
void SimMessage::Initialize(int self, string loadPath){

    stringstream sfile;
    string str;
    ifstream file;
    char delim = ';';
    string::size_type sz;

    // cout << "SimMessage::Initialize" << endl;
    sfile << loadPath << "sim-msg.ini";
    file.open(sfile.str());
    // cout << "open" << endl;
    while(!file.eof()) // To get you all the lines.
    {
        getline(file, str); // Saves the line in STRING.
        // cout << "new line=" << str << endl;
        if (str.compare("") != 0){
            // cout << "ok" << endl;
            strings_t str_split = split(str, delim);
            int nodeId = stoi(str_split.at(1), &sz);
            // cout << "nodeId=" << nodeId << endl;
            if (self == nodeId) {
                message_info_t message;
                message.delay = stod(str_split.at(2));
                message.destination = stoi(str_split.at(3), &sz);
                message.byteLength = stoi(str_split.at(4), &sz);
                message.messageType = stoi(str_split.at(5), &sz);
                message.idVideo = stoi(str_split.at(6), &sz);
                // cout << "line=" << str << " => id=" << self << " dest=" << message.destination << endl;
                messages.push_back(message);
            }
        }
    }
    file.close();

    pos = 0;
}

bool SimMessage::isThereNextMessageInfo(){
    return ((messages.size()!=0) && (pos <= messages.size()-1));
}

message_info_t SimMessage::retNextMessageInfo(){
    if (!(isThereNextMessageInfo()))
        pos = 0;
    pos = pos + 1;
    return messages.at(pos-1);
}

strings_t split (const string &s, char delim) {
    vector<string> result;
    stringstream ss (s);
    string item;
    while (getline (ss, item, delim)) {
        result.push_back (item);
    }
    return result;
}
