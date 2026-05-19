#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

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


class SystemManager 
{
private:
    std::unordered_map<int, std::shared_ptr<User>> users_;
    std::unordered_map<int, std::shared_ptr<Group>> groups_;

public:
    void createUser(int id, const std::string& name, const std::string& info) 
    {
        users_[id] = std::make_shared<User>(id, name, info);
        std::cout << "ok\n";
    }

    void deleteUser(int id) 
    {
        users_.erase(id); 
        std::cout << "ok\n";
    }

    void printAllUsers() const 
    {
        for (const auto& [id, user] : users_) 
        {
            std::cout << "user " << id << " | " << user->getUsername() << " | " << user->getInfo();
            if (auto spGroup = user->getGroup().lock()) { std::cout << " | group: " << spGroup->getId() << "\n"; } 
            else { std::cout << " | group: none\n"; }
        }
    }

    void getUser(int id) const 
    {
        if (users_.count(id)) 
        {
            auto user = users_.at(id);
            std::cout << "user " << id << " | " << user->getUsername() << " | " << user->getInfo();
            if (auto spGroup = user->getGroup().lock()) { std::cout << " | group: " << spGroup->getId() << "\n"; } 
            else { std::cout << " | group: none\n"; }
        } 
        else { std::cout << "not found\n"; }
    }

    void createGroup(int id) 
    {
        groups_[id] = std::make_shared<Group>(id);
        std::cout << "ok\n";
    }

    void deleteGroup(int id) 
    {
        groups_.erase(id);
        std::cout << "ok\n";
    }

    void printAllGroups() const 
    {
        for (const auto& [id, group] : groups_) 
        {
            std::cout << "group " << id << " users:\n";
            for (const auto& weakUser : group->getUsers()) 
            {
                if (auto spUser = weakUser.lock()) { std::cout << "  - " << spUser->getUsername() << "\n"; }
            }
        }
    }

    void getGroup(int id) const 
    {
        if (groups_.count(id)) 
        {
            std::cout << "group " << id << " users:\n";
            for (const auto& weakUser : groups_.at(id)->getUsers()) 
            {
                if (auto spUser = weakUser.lock()) { std::cout << "  - " << spUser->getUsername() << "\n"; }
            }
        } 
        else { std::cout << "not found\n"; }
    }

    void addUserToGroup(int userId, int groupId) 
    {
        if (users_.count(userId) && groups_.count(groupId)) 
        {
            users_[userId]->setGroup(groups_[groupId]);
            groups_[groupId]->addUser(users_[userId]);
            std::cout << "ok\n";
        } 
        else { std::cout << "error\n"; }
    }
};


class ConsoleApplication 
{
private:
    SystemManager manager_;
    std::unordered_map<std::string, std::function<void()>> commands_;
    bool is_running_ = true;

    void registerCommands() 
    {
        commands_["createUser"] = [this]() 
        {
            int id; std::string name, info;
            std::cin >> id >> name >> info;
            manager_.createUser(id, name, info);
        };
        commands_["deleteUser"] = [this]() 
        {
            int id; std::cin >> id;
            manager_.deleteUser(id);
        };
        commands_["allUsers"]   = [this]() { manager_.printAllUsers(); };
        commands_["getUser"]    = [this]() 
        {
            int id; std::cin >> id;
            manager_.getUser(id);
        };
        commands_["createGroup"] = [this]() 
        {
            int id; std::cin >> id;
            manager_.createGroup(id);
        };
        commands_["deleteGroup"] = [this]() 
        {
            int id; std::cin >> id;
            manager_.deleteGroup(id);
        };
        commands_["allGroups"]  = [this]() { manager_.printAllGroups(); };
        commands_["getGroup"]   = [this]() 
        {
            int id; std::cin >> id;
            manager_.getGroup(id);
        };
        commands_["addToGroup"] = [this]() 
        {
            int uid, gid; std::cin >> uid >> gid;
            manager_.addUserToGroup(uid, gid);
        };
        commands_["exit"]       = [this]() { is_running_ = false; };
    }

public:
    ConsoleApplication() { registerCommands(); }

    void run() 
    {
        std::string command;
        std::cout << "waiting for commands\n";

        while (is_running_ && std::cin >> command) 
        {
            if (commands_.count(command)) { commands_[command](); } 
            else { std::cout << "unknown command\n"; }
        }
    }
};


int main() 
{
    ConsoleApplication app;
    app.run();
    return 0;
}
