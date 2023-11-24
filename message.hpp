
#pragma once
#include <bits/stdc++.h>
#include "user.hpp"
#include "client.hpp"

using namespace std;

struct Bitstream{
    int userId;
    char message[BUFFER_SIZE];
    int size;
};

class Message{
    private:
        int userId;
        string message;
        int size;
    public:
        Message();
        Message(int userId, string message);

        struct Bitstream createMessageBitstream();
        
};