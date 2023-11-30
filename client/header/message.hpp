
#pragma once
#include <bits/stdc++.h>
#include "user.hpp"
#include "client.hpp"
#include "../../asserts/constraints.hpp"

using namespace std;

struct Bitstream{
    char type[TYPE_SIZE];
    char message[BUFFER_SIZE];
    int userId;
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