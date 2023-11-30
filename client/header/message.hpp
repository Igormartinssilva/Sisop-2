
#pragma once
#include <bits/stdc++.h>
#include "user.hpp"
#include "client.hpp"
#include "../../asserts/constraints.hpp"
#include "../../common/header/data.hpp"


using namespace std;

class Message{
    private:
        int userId;
        string message;
        int size;
    public:
        Message();
        Message(int userId, string message);

        struct twt::Package createMessageBitstream();
        
};