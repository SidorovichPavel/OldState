#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <iterator>
#include <functional>

#include "utility.hpp"

enum class LogState
{
	none = 0,
	logout = 0x0f0f0f0f,
	login = 0x70f0f0f0
};

class User
{
	std::string mName;
	std::string mPass;
	LogState mState;
public:
	User(std::string&& _Name, std::string _Pass, LogState _State = LogState::logout)
		:
		mName(std::move(_Name)),
		mPass(std::move(_Pass)),
		mState(_State)
	{}
	User()
		:
		mState(LogState::none)
	{}
	void set_log_state(LogState _LState)
	{
		mState = _LState;
	}
	std::string& get_name() noexcept
	{
		return mName;
	}
	std::string& get_pass() noexcept
	{
		return mPass;
	}
	LogState get_log_state() noexcept
	{
		return mState;
	}
};

void state_select(User& _User, std::function<void()> _OnLogin, std::function<void()> _OnLogout)
{
	switch (_User.get_log_state())
	{
	case LogState::login:
		_OnLogin();
		break;
	case LogState::logout:
		_OnLogout();
		break;
	}
}

int main()
{
	std::map<size_t, User> users;
	std::ostream& out = std::cout;
	const auto act_reg = std::hash<std::string>()("register");
	const auto act_login = std::hash<std::string>()("login");
	const auto act_logout = std::hash<std::string>()("logout");

	std::regex words_regex("[!-~]+");

	std::ifstream fin("res/input.txt");
	fin.imbue(std::locale(std::locale::classic(), new sp::ctype));

	int action_count;
	fin >> action_count;

	std::string action_desc;
	for (auto i = 0; i < action_count; i++)
	{
		fin >> action_desc;

		auto word_it = std::sregex_iterator(action_desc.begin(), action_desc.end(), words_regex);
		std::smatch match = *word_it;
		size_t action = std::hash<std::string>()(match.str());

		std::string name = (++word_it)->str();
		size_t id = std::hash<std::string>{}(name);
		auto user_in_base = users.find(id);

		if (action == act_reg)
		{
			User user(std::move(name), (++word_it)->str());

			if (user_in_base == users.end())
			{
				users[id] = std::move(user);
				out << "success: new user added" << std::endl;
			}
			else
				out << "fail: user already exists" << std::endl;

			continue;
		}

		if (user_in_base == users.end())
		{
			out << "fail: no such user" << std::endl;
			continue;
		}

		auto& user = user_in_base->second;

		if (action == act_login)
		{
			state_select(user,
				[&out]() {out << "fail : already logged in" << std::endl; },
				[&user, &word_it, &out]()
				{
					if (user.get_pass() == (++word_it)->str())
					{
						out << "success: user logged in" << std::endl;
						user.set_log_state(LogState::login);
					}
					else
						out << "fail: incorrect password" << std::endl;
				});
			continue;
		}

		if (action == act_logout)
		{
			state_select(user,
				[&out, &user]()
				{
					out << "success: user logged out" << std::endl;
					user.set_log_state(LogState::logout);
				},
				[&out]() {out << "fail: already logged out" << std::endl; });
			continue;
		}

	}

	return 0;
}