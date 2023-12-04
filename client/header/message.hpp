
#pragma once
#include <bits/stdc++.h>
#include "user.hpp"
#include "client.hpp"
#include "../../asserts/constraints.hpp"
#include "../../common/header/data.hpp"


using namespace std;

class Message{
    private:
    public:
        int size;
        string message;
        int userId;
        Message();
        Message(int userId, string message);
};