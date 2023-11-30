#include "header/header.hpp"

using namespace std;

int main() {
  int userId = 15;
  string str;
  User user(userId);
  Session session = Session();

  cout << "Write down a message!" << endl;
  getline(cin, str);

  session.sendMessage(Message(user.getUserId(), str));
}
