
#include <bits/stdc++.h>
#include "user.hpp"

using namespace std;

class Message{
    private:
        User user;
        string message;
        int size;
    public:
        Message();
        Message(int userId, string message);

        string createMessageBitstream();
        
}