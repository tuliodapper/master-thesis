#ifndef SIM_H
#define SIM_H

#include <iostream>
#include <string>
#include <vector>

using namespace std;

typedef vector<string> strings_t;

typedef struct mobilityInfo {
    double x;
    double y;
    double delay;
} mobility_info_t;

typedef struct messageInfo {
    int destination;
    double delay;
    int byteLength;
    int messageType;
    int idVideo;
} message_info_t;

class SimMobility
{
private:
    int pos;
public:
    SimMobility();
    vector<mobility_info_t> mobilities;
    void Initialize(int self, string loadPath);
    bool isThereNextMobilityInfo();
    mobility_info_t retNextMobilityInfo();
};

class SimMessage
{
private:
    int pos;
public:
    SimMessage();
    vector<message_info_t> messages;
    void Initialize(int self, string loadPath);
    bool isThereNextMessageInfo();
    message_info_t retNextMessageInfo();
};

strings_t split (const string &s, char delim);

#endif // SIM_H
