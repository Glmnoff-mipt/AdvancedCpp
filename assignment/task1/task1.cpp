#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

class Group;

class User 
{
private:
    int id_;
    std::string username_;
    std::string info_;
    
    std::weak_ptr<Group> group_; 

public:
    User(int id, std::string username, std::string info)
        : id_{id}, username_{std::move(username)}, info_{std::move(info)} {}

    int getId() const { return id_; }
    std::string getUsername() const { return username_; }
    std::string getInfo() const { return info_; }
    std::weak_ptr<Group> getGroup() const { return group_; }

    void setGroup(std::weak_ptr<Group> group) { group_ = group; }
};

class Group 
{
private:
    int id_;
    std::vector<std::weak_ptr<User>> users_;

public:
    explicit Group(int id) : id_{id} {}

    int getId() const { return id_; }
    const std::vector<std::weak_ptr<User>>& getUsers() const { return users_; }

    void addUser(std::weak_ptr<User> user) { users_.push_back(user); }
};


int main() 
{
    std::unordered_map<int, std::shared_ptr<User>> users;
    std::unordered_map<int, std::shared_ptr<Group>> groups;

    std::string command;
    std::cout << "Waiting for commands\n";

    while (std::cin >> command) 
    {
        if (command == "createUser") 
        {
            int id; std::string name, info;
            std::cin >> id >> name >> info;
            users[id] = std::make_shared<User>(id, name, info);
            std::cout << "ok\n";
        } 
        else if (command == "deleteUser") 
        {
            int id; std::cin >> id;
            users.erase(id); 
            std::cout << "ok\n";
        } 
        else if (command == "allUsers") 
        {
            for (const auto& [id, user] : users) 
            {
                std::cout << "user " << id << " | " << user->getUsername() << " | " << user->getInfo();
                
                if (auto spGroup = user->getGroup().lock()) { std::cout << " | group: " << spGroup->getId() << "\n"; } 
                else { std::cout << " | group: none\n"; }
            }
        } 
        else if (command == "getUser") 
        {
            int id; std::cin >> id;
            if (users.count(id)) 
            {
                auto user = users[id];
                std::cout << "user " << id << " | " << user->getUsername() << " | " << user->getInfo();
                
                if (auto spGroup = user->getGroup().lock()) { std::cout << " | group: " << spGroup->getId() << "\n"; } 
                else { std::cout << " | group: none\n"; }
            } 
            else { std::cout << "not found\n"; }
        } 
        else if (command == "createGroup") 
        {
            int id; std::cin >> id;
            groups[id] = std::make_shared<Group>(id);
            std::cout << "ok\n";
        } 
        else if (command == "deleteGroup") 
        {
            int id; std::cin >> id;
            groups.erase(id);
            std::cout << "ok\n";
        } 
        else if (command == "allGroups") 
        {
            for (const auto& [id, group] : groups) 
            {
                std::cout << "group " << id << " users:\n";
                for (const auto& weakUser : group->getUsers()) 
                {
                    if (auto spUser = weakUser.lock()) { std::cout << "  - " << spUser->getUsername() << "\n"; }
                }
            }
        } 
        else if (command == "getGroup") 
        {
            int id; std::cin >> id;
            if (groups.count(id)) 
            {
                std::cout << "group " << id << " users:\n";
                for (const auto& weakUser : groups[id]->getUsers()) 
                {
                    if (auto spUser = weakUser.lock()) { std::cout << "  - " << spUser->getUsername() << "\n"; }
                }
            } 
            else { std::cout << "not found\n"; }
        } 
        else if (command == "addToGroup") 
        {
            int userId, groupId;
            std::cin >> userId >> groupId;
            if (users.count(userId) && groups.count(groupId)) 
            {
                users[userId]->setGroup(groups[groupId]);
                groups[groupId]->addUser(users[userId]);
                std::cout << "ok\n";
            } 
            else { std::cout << "error\n"; }
        }
        else if (command == "exit") { break; }
    }
    return 0;
}