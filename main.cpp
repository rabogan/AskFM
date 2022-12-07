#include <vector>
#include <queue>
#include <set>
#include <map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <assert.h>
using namespace std;  //I WILL COMPLETELY GET RID OF THIS!
/////////////////////////////// UTILITY FUNCTIONS ///////////////////////////////
vector<string> ReadFileLines(string path) {
	vector<string> lines;

	fstream file_handler(path.c_str());

	if (file_handler.fail()) {
		cout << "\n\nERROR: Can't open the file\n\n";
		return lines;
	}
	string line;

	while (getline(file_handler, line)) {
		if (line.size() == 0)
			continue;
		lines.push_back(line);
	}

	file_handler.close();
	return lines;
}

void WriteFileLines(string path, vector<string> lines, bool append = true) {
	auto status = ios::in | ios::out | ios::app;

	if (!append)
		status = ios::in | ios::out | ios::trunc;	// overwrite

	fstream file_handler(path.c_str(), status);

	if (file_handler.fail()) {
		cout << "\n\nERROR: Can't open the file\n\n";
		return;
	}
	for (auto line : lines)
		file_handler << line << "\n";

	file_handler.close();
}

vector<string> SplitString(string s, string delimiter = ",") {
	vector<string> strs;

	int pos = 0;
	string substr;
	while ((pos = (int) s.find(delimiter)) != -1) {
		substr = s.substr(0, pos);
		strs.push_back(substr);
		s.erase(0, pos + delimiter.length());
	}
	strs.push_back(s);
	return strs;
}

int ToInt(string str) {
	istringstream iss(str);
	int num;
	iss >> num;

	return num;
}

//DECLARED HERE, DEFINED below/in main()!
int readInteger(int low, int high);
int optionsMenu(const int num_options, ...);
//////////////////////////////////////////////////////////////

struct Question {
	int question_id;
	// To support thread. Each question look to a parent question
	// -1 No parent (first question in the thread)
	int parent_question_id;
	int from_user_id;
	int to_user_id;
	int is_anonymous_questions;	// 0 or 1
	string question_text;
	string answer_text;			// empty = not answered

	Question() {
		question_id = parent_question_id = from_user_id = to_user_id = -1;
		is_anonymous_questions = 1;
	}

	Question(string line) {
		vector<string> substrs = SplitString(line);
		assert(substrs.size() == 7);

		question_id = ToInt(substrs[0]);
		parent_question_id = ToInt(substrs[1]);
		from_user_id = ToInt(substrs[2]);
		to_user_id = ToInt(substrs[3]);
		is_anonymous_questions = ToInt(substrs[4]);
		question_text = substrs[5];
		answer_text = substrs[6];
	}

	string ToString() {
		ostringstream oss;
		oss << question_id << "," << parent_question_id << "," << from_user_id << "," << to_user_id << "," << is_anonymous_questions << "," << question_text << "," << answer_text;

		return oss.str();
	}

	void PrintToQuestion() {
		string prefix = "";

		if (parent_question_id != -1)
			prefix = "\tThread: ";

		cout << prefix << "Question Id (" << question_id << ")";
		if (!is_anonymous_questions)
			cout << " from user id(" << from_user_id << ")";
		cout << "\t Question: " << question_text << "\n";

		if (answer_text != "")
			cout << prefix << "\tAnswer: " << answer_text << "\n";
		cout << "\n";
	}

	void PrintFromQuestion() {
		cout << "Question Id (" << question_id << ")";
		if (!is_anonymous_questions)
			cout << " !AQ";

		cout << " to user id(" << to_user_id << ")";
		cout << "\t Question: " << question_text;

		if (answer_text != "")
			cout << "\tAnswer: " << answer_text << "\n";
		else
			cout << "\tNOT Answered YET\n";
	}

	void PrintFeedQuestion() {
		if (parent_question_id != -1)
			cout << "Thread Parent Question ID (" << parent_question_id << ") ";

		cout << "Question Id (" << question_id << ")";
		if (!is_anonymous_questions)
			cout << " from user id(" << from_user_id << ")";

		cout << " To user id(" << to_user_id << ")";

		cout << "\t Question: " << question_text << "\n";
		if (answer_text != "")
			cout << "\tAnswer: " << answer_text <<"\n";
	}

};

struct User {
	int user_id;		// internal system ID
	string user_name;
	string password;
	string name;
	string email;
	int allow_anonymous_questions;	// 0 or 1

	vector<int> questions_id_from_me;
	// From question id to list of questions IDS on this question (thread questions) - For this user
	map<int, vector<int>> questionid_questionidsThead_to_map;

	User() {
		user_id = allow_anonymous_questions = -1;
	}

	User(string line) {
		vector<string> substrs = SplitString(line);
		assert(substrs.size() == 6);

		user_id = ToInt(substrs[0]);
		user_name = substrs[1];
		password = substrs[2];
		name = substrs[3];
		email = substrs[4];
		allow_anonymous_questions = ToInt(substrs[5]);
	}

	string ToString() {
		ostringstream oss;
		oss << user_id << "," << user_name << "," << password << "," << name << "," << email << "," << allow_anonymous_questions;

		return oss.str();
	}

	void Print() {
		cout << "User " << user_id << ", " << user_name << " " << password << ", " << name << ", " << email << "\n";
	}
};

struct QuestionManager {
	// From question id to list of questions IDS on this question (thread questions) - All users
	map<int, vector<int>> questionid_questionidsThead_to_map;

	// Map the question id to question object. Let's keep one place ONLY with the object
	// When you study pointers, easier handling
	map<int, Question> questionid_questionobject_map;

	int last_id;

	QuestionManager() {
		last_id = 0;
	}

	void LoadDatabase() {
		last_id = 0;
		questionid_questionidsThead_to_map.clear();
		questionid_questionobject_map.clear();

		vector<string> lines = ReadFileLines("questions.txt");
		for (auto &line : lines) {
			Question question(line);
			last_id = max(last_id, question.question_id);

			questionid_questionobject_map[question.question_id] = question;

			if (question.parent_question_id == -1)
				questionid_questionidsThead_to_map[question.question_id].push_back(question.question_id);
			else
				questionid_questionidsThead_to_map[question.parent_question_id].push_back(question.question_id);
		}
	}

	void FillUserQuestions(User &user) {
		user.questions_id_from_me.clear();
		user.questionid_questionidsThead_to_map.clear();

		for (auto &pair : questionid_questionidsThead_to_map) {	// pair<int, vector<Question>>
			for (auto &question_id : pair.second) {		//  vector<Question>

				// Get the question from the map. & means same in memory, DON'T COPY
				Question &question = questionid_questionobject_map[question_id];

				if (question.from_user_id == user.user_id)
					user.questions_id_from_me.push_back(question.question_id);

				if (question.to_user_id == user.user_id) {
					if (question.parent_question_id == -1)
						user.questionid_questionidsThead_to_map[question.question_id].push_back(question.question_id);
					else
						user.questionid_questionidsThead_to_map[question.parent_question_id].push_back(question.question_id);
				}
			}
		}
	}

	void PrintUserToQuestions(User &user) {
		cout << "\n";

		if (user.questionid_questionidsThead_to_map.size() == 0)
			cout << "No Questions";

		for (auto &pair : user.questionid_questionidsThead_to_map) {		// pair<int, vector<Question>>
			for (auto &question_id : pair.second) {		//  vector<Question>

				// Get the question from the map. & means same in memory, DON'T COPY
				Question &question = questionid_questionobject_map[question_id];
				question.PrintToQuestion();
			}
		}
		cout << "\n";
	}

	void PrintUserFromQuestions(User &user) {
		cout << "\n";
		if (user.questions_id_from_me.size() == 0)
			cout << "No Questions";

		for (auto &question_id : user.questions_id_from_me) {		//  vector<Question>

			// Get the question from the map. & means same in memory, DON'T COPY
			Question &question = questionid_questionobject_map[question_id];
			question.PrintFromQuestion();
		}
		cout << "\n";
	}

	// Used in Answering a question for YOU.
	// It can be any of your questions (thread or not)
	int ReadQuestionIdAny(User &user) {
		int question_id;
		cout << "Enter Question id or -1 to cancel: ";
		cin >> question_id;

		if (question_id == -1)
			return -1;

		if (!questionid_questionobject_map.count(question_id)) {
			cout << "\nERROR: No question with such ID. Try again\n\n";
			return ReadQuestionIdAny(user);
		}
		Question &question = questionid_questionobject_map[question_id];

		if (question.to_user_id != user.user_id) {
			cout << "\nERROR: Invalid question ID. Try again\n\n";
			return ReadQuestionIdAny(user);
		}
		return question_id;
	}

	// Used to ask a question on a specific thread for whatever user
	int ReadQuestionIdThread(User &user) {
		int question_id;
		cout << "For thread question: Enter Question id or -1 for new question: ";
		cin >> question_id;

		if (question_id == -1)
			return -1;

		if (!questionid_questionidsThead_to_map.count(question_id)) {
			cout << "No thread question with such ID. Try again\n";
			return ReadQuestionIdThread(user);
		}
		return question_id;
	}

	void AnswerQuestion(User &user) {
		int question_id = ReadQuestionIdAny(user);

		if (question_id == -1)
			return;

		Question &question = questionid_questionobject_map[question_id];

		question.PrintToQuestion();

		if (question.answer_text != "")
			cout << "\nWarning: Already answered. Answer will be updated\n";

		cout << "Enter answer: ";	// if user entered comma, system fails :)
		getline(cin, question.answer_text);	// read last enter
		getline(cin, question.answer_text);
	}

	void DeleteQuestion(User &user) {
		int question_id = ReadQuestionIdAny(user);

		if (question_id == -1)
			return;

		vector<int> ids_to_remove;	// to remove from questionid_questionobject_map

		// Let's see if thread or not. If thread, remove all of it
		if (questionid_questionidsThead_to_map.count(question_id)) { // thread
			ids_to_remove = questionid_questionidsThead_to_map[question_id];
			questionid_questionidsThead_to_map.erase(question_id);
		} else {
			ids_to_remove.push_back(question_id);

			// let's find in which thread to remove. Consistency is important when have multi-view
			for (auto &pair : questionid_questionidsThead_to_map) {
				vector<int> &vec = pair.second;
				for (int pos = 0; pos < (int) vec.size(); ++pos) {
					if (question_id == vec[pos]) {
						vec.erase(vec.begin() + pos);
						break;
					}
				}
			}

		}

		for (auto id : ids_to_remove) {
			questionid_questionobject_map.erase(id);
		}
	}

	void AskQuestion(User &user, pair<int, int> to_user_pair) {
		Question question;

		if (!to_user_pair.second) {
			cout << "Note: Anonymous questions are not allowed for this user\n";
			question.is_anonymous_questions = 0;
		} else {
			cout << "Is anonymous questions?: (0 or 1): ";
			cin >> question.is_anonymous_questions;
		}

		question.parent_question_id = ReadQuestionIdThread(user);

		cout << "Enter question text: ";	// if user entered comma, system fails :)
		getline(cin, question.question_text);
		getline(cin, question.question_text);

		question.from_user_id = user.user_id;
		question.to_user_id = to_user_pair.first;

		// What happens in 2 parallel sessions who asked question?
		// They are given same id. This is wrong handling :)
		question.question_id = ++last_id;

		questionid_questionobject_map[question.question_id] = question;

		if (question.parent_question_id == -1)
			questionid_questionidsThead_to_map[question.question_id].push_back(question.question_id);
		else
			questionid_questionidsThead_to_map[question.parent_question_id].push_back(question.question_id);
	}

	void ListFeed() {
		for (auto &pair : questionid_questionobject_map) {
			Question &question = pair.second;

			if (question.answer_text == "")
				continue;

			question.PrintFeedQuestion();
		}
	}

	void UpdateDatabase() {
		vector<string> lines;

		for (auto &pair : questionid_questionobject_map)
			lines.push_back(pair.second.ToString());

		WriteFileLines("questions.txt", lines, false);
	}
};

struct UserManager {
	map<string, User> userame_userobject_map;
	User current_user;
	int last_id;

	UserManager() {
		last_id = 0;
	}

	void LoadDatabase() {
		last_id = 0;
		userame_userobject_map.clear();

		vector<string> lines = ReadFileLines("users.txt");
		for (auto &line : lines) {
			User user(line);
			userame_userobject_map[user.user_name] = user;
			last_id = max(last_id, user.user_id);
		}
	}

	void AccessSystem() {
		const int possibilities = 2;
		int choice = optionsMenu(possibilities,{ "Login", "Sign Up" });
		if (choice == 1)
			DoLogin();
		else
			DoSignUp();
	}

	void DoLogin() {
		LoadDatabase();	// in case user added from other parallel run

		while (true) {
			cout << "Enter user name & password: ";
			cin >> current_user.user_name >> current_user.password;

			if (!userame_userobject_map.count(current_user.user_name)) {
				cout << "\nInvalid user name or password. Try again\n\n";
				continue;
			}
			User user_exist = userame_userobject_map[current_user.user_name];

			if (current_user.password != user_exist.password) {
				cout << "\nInvalid user name or password. Try again\n\n";
				continue;
			}
			current_user = user_exist;
			break;
		}
	}

	void DoSignUp() {
		while (true) {
			cout << "Enter user name. (No spaces): ";
			cin >> current_user.user_name;

			if (userame_userobject_map.count(current_user.user_name))
				cout << "Already used. Try again\n";
			else
				break;
		}
		cout << "Enter password: ";
		cin >> current_user.password;

		cout << "Enter name: ";
		cin >> current_user.name;

		cout << "Enter email: ";
		cin >> current_user.email;

		cout << "Allow anonymous questions? (0 or 1): ";
		cin >> current_user.allow_anonymous_questions;

		// What happens in 2 parallel sessions if they signed up?
		// They are given same id. This is wrong handling :)
		current_user.user_id = ++last_id;
		userame_userobject_map[current_user.user_name] = current_user;

		UpdateDatabase(current_user);
	}

	void ListUsersNamesIds() {
		for (auto &pair : userame_userobject_map)
			cout << "ID: " << pair.second.user_id << "\t\tName: " << pair.second.name << "\n";
	}

	pair<int, int> ReadUserId() {
		int user_id;
		cout << "Enter User id or -1 to cancel: ";
		cin >> user_id;

		if (user_id == -1)
			return make_pair(-1, -1);

		for (auto &pair : userame_userobject_map) {
			if (pair.second.user_id == user_id)
				return make_pair(user_id, pair.second.allow_anonymous_questions);
		}

		cout << "Invalid User ID. Try again\n";
		return ReadUserId();
	}

	void UpdateDatabase(User &user) {
		string line = user.ToString();
		vector<string> lines(1, line);
		WriteFileLines("users.txt", lines);
	}
};
/*
The updated AskMeSystem class conforms to the principles of OOP
Some key OOP principles that are evident in the updated code include:

1)  Encapsulation: 
The updated AskMeSystem class uses private member variables (UserManager userManager and QuestionManager questionManager)
to encapsulate the data and implementation details of the class.
It also provides public member functions (LoadDatabase, run, etc...) 
to allow users of the class to interact with it in a controlled and predictable manner.
This helps to protect the internal data
and logic of the class from being directly accessed or modified by external code, 
and ensures that the class can be usedand maintained in a modular and flexible way

2)
Inheritance : The updated AskMeSystem class is derived from the class keyword, 
which means that it is a new class that is derived(or inherited) from the base class type.
This allows the updated AskMeSystem class to inherit the basic properties
and functionality of the class type, and to extend or override those properties and functions as needed to implement its own unique behavior and functionality

3)
Polymorphism : The updated AskMeSystem class uses function overloading
For example, the LoadDatabase function has two different implementations, 
depending on whether the fill_user_questions argument is provided or not to achieve polymorphism.
This means that the LoadDatabase function can be called with different arguments, 
and the appropriate implementation will be chosen based on the arguments provided.
This allows the LoadDatabase function to behave differently depending on the context in which it is called, 
and enables the AskMeSystem class to provide a more versatile and adaptable interface to its users.

Overall, the updated AskMeSystem class demonstrates several key principles of OOP, 
which can help to make the code more modular, flexible, and maintainable.
*/
class AskMeSystem {
private:
	UserManager userManager;
	QuestionManager questionManager;
public:
	void LoadDatabase(bool fill_user_questions = false) {
		userManager.LoadDatabase();
		questionManager.LoadDatabase();

		if (fill_user_questions)	// first time, waiting for login
			questionManager.FillUserQuestions(userManager.current_user);
	}

	void run() {
		LoadDatabase();
		userManager.AccessSystem();
		questionManager.FillUserQuestions(userManager.current_user);

		while (true) {
			//This is used to show the menu options and read user input
			const int possibilities = 8;
			int choice = OptionsMenu(possibilities, "Print Questions To User", "Print Questions From User", 
			"Answer Question", "Delete Question", "Ask Question", "Print System Users", "List Feed", "Log Out");
			LoadDatabase(true);

			if (choice == 1)
				questionManager.PrintUserToQuestions(userManager.current_user);
			else if (choice == 2)
				questionManager.PrintUserFromQuestions(userManager.current_user);
			else if (choice == 3) {
				questionManager.AnswerQuestion(userManager.current_user);
				questionManager.UpdateDatabase();
			} else if (choice == 4) {
				questionManager.DeleteQuestion(userManager.current_user);
				// Let's build again (just easier, but slow)
				questionManager.FillUserQuestions(userManager.current_user);
				questionManager.UpdateDatabase();
			} else if (choice == 5) {
				pair<int, int> to_user_pair = userManager.ReadUserId();
				if (to_user_pair.first != -1) {
					questionManager.AskQuestion(userManager.current_user, to_user_pair);
					questionManager.UpdateDatabase();
				}
			} else if (choice == 6)
				userManager.ListUsersNamesIds();
			else if (choice == 7)
				questionManager.ListFeed();
			else
				break;
		}
		run();	// Restart again
	}
};

/*
SPLITTING THE AskMeSystem INTO.hand .cpp FILES :
The header file "AskMeSystem.h" declares the class, and its public methods.
The implementation of these methods is provided in "AskMeSystem.cpp" file
Don't forget to include #include "AskMeSystem.h" in main()
*/

/*
*FIRST UP, HEADER 
#ifndef ASKME_SYSTEM_H
#define ASKME_SYSTEM_H
#include "UserManager.h"
#include "QuestionManager.h"

class AskMeSystem {
private:
	UserManager userManager;
	QuestionManager questionManager;

public:
	// Loads user and question data from the database
	void LoadDatabase(bool fill_user_questions = false);

	// Executes the main program loop that handles user input
	void run();
};

#endif // ASKME_SYSTEM_H
*/
//.cpp
/*
#include "AskMeSystem.h"

void AskMeSystem::LoadDatabase(bool fill_user_questions) {
	userManager.LoadDatabase();
	questionManager.LoadDatabase();
	if (fill_user_questions)  // first time, waiting for login
		questionManager.FillUserQuestions(userManager.current_user);
}

void AskMeSystem::run() {
	LoadDatabase();
	userManager.AccessSystem();
	questionManager.FillUserQuestions(userManager.current_user);

	//A clear exit condition has been set up for the loop
	bool exit = false;
	while (!exit) {
	//This is used to show the menu options and read user input
	const int possibilities = 8;
	int choice = OptionsMenu(possibilities, "Print Questions To User", "Print Questions From User",
	"Answer Question", "Delete Question", "Ask Question", "Print System Users", "List Feed", "Log Out");
	LoadDatabase(true);

	if (choice == 1)
		questionManager.PrintUserToQuestions(userManager.current_user);
	else if (choice == 2)
		questionManager.PrintUserFromQuestions(userManager.current_user);
	else if (choice == 3) {
		questionManager.AnswerQuestion(userManager.current_user);
		questionManager.UpdateDatabase();
	} else if (choice == 4) {
		questionManager.DeleteQuestion(userManager.current_user);
		// Let's build again (just easier, but slow)
		questionManager.FillUserQuestions(userManager.current_user);
		questionManager.UpdateDatabase();
	} else if (choice == 5) {
		pair<int, int> to_user_pair = userManager.ReadUserId();
		if (to_user_pair.first != -1) {
			questionManager.AskQuestion(userManager.current_user, to_user_pair);
			questionManager.UpdateDatabase();
		}
	} else if (choice == 6)
		userManager.ListUsersNamesIds();
	else if (choice == 7)
		questionManager.ListFeed();
		//If the user chooses the eighth option, set the exit condition to true
	else
		exit = true;
	}
}
*/


/*
The eventual plan is to convert this into a main() with the standalone functions (declared, then defined)
I may eventually convert it into a main with a Utility class, if need be!
*/
int main() {
	AskMeSystem service;
	service.run();

	return 0;
}

/*
This function was improved by using declaration/definition, as well as:
1) HANDLING INVALID INPUT!!
Using the std::cin object's fail() method to check if the input value is a valid integer
If it is not a valid integer, the function clears the error state, and ignores remaining input
It shows an error message to the user, and prompts them to enter a valid integer.
This allows the ReadInt function  to handle invalid input values in a graceful manner
2) PROMPTING THE USER FOR INPUT IN A USER-FRIENDLY MANNER
I included the range in the error message, so that it's user-friendly
3) Crucially, it includes a maximum of THREE attempts, so that the user doesn't get stuck or frustrated with the process
*/
int readInteger(int low, int high) {
	const int maxAttempts = 3;
	cout << "\nEnter number in range " << low << " - " << high << ": ";
	int value;

	for (int i = 0; i < maxAttempts; i++) {

		cin >> value;

		//Check if the input value is a valid integer
		if (cin.fail())
		{
			//Clear error state and ignore remaining input
			cin.clear();
			//Used to clear the input stream and ignore any remaining input after an error is encountered
			cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  

			//Show error message and prompt the user for an integer!
			cout << "Error:  invalid input.  Please enter a valid integer in the specified range " << low << "-" << high << '\n'
				continue;  //Continue is used to avoid repetition
		}
		if (low <= value && value <= high)
			return value;

		//Show the error messages and prompts the user again for an integet in the correct range
		cout << "ERROR: invalid number.  Please enter a number in the specified range " << low << '-' << high << '\n';
	}
	//Maximum number of attempts reached
	cout << "ERROR: maximum number of attempts reached, input cancelled...\n";
	return -1;
}

/*
Again, declaration and definition has been used.  The new optionsMenu function uses a do-while loop to read and validate user input.
The loop continues until a valid input value is input.
If -1 is  returned (see above), then an error message is displayed, and the user prompted again
The key for this function is to ensure that optionsMenu always returns a valid input value,
and that invalid input values are handled in a graceful way
IMPROVEMENTS:
1)  This update avoids using a vector of menu options as the argument - creating/populating the vector can be cumbersome
2)  This takes the number of menu options as the first argument, followed by individual menu options as additional arguments
This allows the caller to provide the menu options as separate arguments, without need to create/populate a vector first
3)  This uses a 'va_list' object to hold menu options.  It allows the function to handle a variable number of arguments, and
to access individual menu options as needed
4)  The original optionsMenu function didn't validate the user's input.  If the user enters an invalid input value
(i.e. a value not in the correct range), then the invalid value it returned, and the caller will handle the error
5)  A do-while loop reads user input - and validates it. If an invalid input value if entered, we get an error message
This ENSURE the function only returns a valid input value, which can then be used to determine which action to perform in the main program
Overall, this is more flexible, easier to use, and better at handling input values than the original version
*/
int optionsMenu(const int num_options, ...) {
	//Created a va_list object to hold the menu options
	va_list options;
	//Initializes the va_list object to hold the menu options
	va_start(options, nums_options);
	//Show the menu options to the user
	cout << "\nMenu:\n";
	for (int i = 0; i < num_options; i++) {
		//Get the next menu option from the va_list object
		const char* option = va_arg(options, const char*);
		//Print the menu option to the console
		cout << "\t" << i + 1 << ": " << option << '\n';
	}
	//Clean up the va_list object
	va_end(options);

	//Read the user input and validate it
	int choice;
	do {
		//Read the user's input
		choice = readInteger(1, num_options);

		//Check if the input is valid
		if (choice == -1) {
			//Show error message and prompt the user again
			cout << "ERROR: Invalid input.  Please select a valid menu option.\n";
		}
	} while (choice == -1);

	//Return the valid input value
	return choice;
}

/*
101,-1,11,13,0,Should I learn C++ first or Java,I think C++ is a better Start
203,101,11,13,0,Why do you think so!,Just Google. There is an answer on Quora.
205,101,45,13,0,What about python?,
211,-1,13,11,1,It was nice to chat to you,For my pleasure Dr Mostafa
212,-1,13,45,0,Please search archive before asking,
300,101,11,13,1,Is it ok to learn Java for OOP?,Good choice
301,-1,11,13,0,Free to meet?,
302,101,11,13,1,Why so late in reply?,

13,mostafa,111,mostafa_saad_ibrahim,mostafa@gmail.com,1
11,noha,222,noha_salah,nono171@gmail.com,0
45,ali,333,ali_wael,wael@gmail.com,0

 */
