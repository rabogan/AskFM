#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <assert.h>

//Major update: the reliance on using namespace std; has been completed removed from the system!

/////////////////////////////// UTILITY FUNCTIONS ///////////////////////////////
std::vector<std::string>readFileLines(std::string path) {
	std::vector<std::string>lines;
	std::fstream fileHandler(path.c_str());

	if (fileHandler.fail()) {
		std::cout << "\n\nERROR: Can't open the file\n\n";
		return lines;
	}
	std::string line;

	while (std::getline(fileHandler, line)) {
		if (line.size() == 0)
			continue;
		lines.push_back(line);
	}

	fileHandler.close();
	return lines;
}

void writeFileLines(std::string path, std::vector<std::string>lines, bool append = true) {
	auto status = std::ios::in | std::ios::out | std::ios::app;

	if (!append)
		status = std::ios::in | std::ios::out | std::ios::trunc;	// overwrite

	std::fstream fileHandler(path.c_str(), status);

	if (fileHandler.fail()) {
		std::cout << "\n\nERROR: Can't open the file\n\n";
		return;
	}
	for (auto line : lines)
		fileHandler << line << '\n';

	fileHandler.close();
}

std::vector<std::string> splitString(std::string s, std::string delimiter = ",") {
	std::vector<std::string>strings;

	int pos = 0;
	std::string substring;
	while ((pos = (int)s.find(delimiter)) != -1) {
		substring = s.substr(0, pos);
		strings.push_back(substring);
		s.erase(0, pos + delimiter.length());
	}
	strings.push_back(s);
	return strings;
}

int toInt(std::string str) {
	std::istringstream iss(str);
	int num;
	iss >> num;

	return num;
}
//////////////////////////////////////////////////////////////

struct Question {
	int questionID;
	// To support thread. Each question looks for a parent question
	// -1 means there's no parent question (i.e. it's the first question in the thread)
	int parentQuestionID;
	int fromUserID;
	int toUserID;
	int anonymousQuestion;	// 0 or 1
	std::string questionText;
	std::string answerText;			// empty = not answered

	Question() {
		questionID = parentQuestionID = fromUserID = toUserID = -1;
		anonymousQuestion = 1;
	}

	Question(std::string line) {
		std::vector<std::string> substringList = splitString(line);
		assert(substringList.size() == 7);

		questionID = toInt(substringList[0]);
		parentQuestionID = toInt(substringList[1]);
		fromUserID = toInt(substringList[2]);
		toUserID = toInt(substringList[3]);
		anonymousQuestion = toInt(substringList[4]);
		questionText = substringList[5];
		answerText = substringList[6];
	}

	std::string toString() {
		std::ostringstream oss;
		oss << questionID << "," << parentQuestionID << "," << fromUserID << "," << toUserID << "," << 
			anonymousQuestion << "," << questionText << "," << answerText;

		return oss.str();
	}

	void printQuestionsReceived() {
		std::string prefix = "";

		if (parentQuestionID != -1)
			prefix = "\tThread: ";

		std::cout << prefix << "Question #" << questionID;
		if (!anonymousQuestion)
			std::cout << " from user #" << fromUserID;
		std::cout << "\t  Question: " << questionText << '\n';

		if (answerText != "")
			std::cout << prefix << "\tAnswer: " << answerText << '\n';
		std::cout << '\n';
	}

	void printQuestionsSent() {
		std::cout << "Question #" << questionID;
		if (!anonymousQuestion)
			std::cout << " (Open Question)";

		std::cout << " to user #" << toUserID;
		std::cout << "\t Question: " << questionText;

		if (answerText != "")
			std::cout << "\tAnswer: " << answerText << '\n';
		else
			std::cout << "\tCurrently unanswered\n";
	}

	void printFeedQuestions() {
		if (parentQuestionID != -1)
			std::cout << "Thread Parent Question #" << parentQuestionID;

		std::cout << "Question #" << questionID;
		if (!anonymousQuestion)
			std::cout << " from user #" << fromUserID;

		std::cout << " To user #" << toUserID;

		std::cout << "\t Question: " << questionText << '\n';
		if (answerText != "")
			std::cout << "\tAnswer: " << answerText << '\n';
	}
};

struct User {
	int userID;		// internal system ID
	std::string username;
	std::string password;
	std::string name;
	std::string email;
	int allowAnonymousQuestions;	// 0 or 1

	std::vector<int> questionIDsFromMe;
	// From question id to list of questions IDS on this question (thread questions) - For this user
	std::map<int, std::vector<int>> questionIDQuestionThreadMap;

	User() {
		userID = allowAnonymousQuestions = -1;
	}

	User(std::string line) {
		std::vector<std::string> substringList = splitString(line);
		assert(substringList.size() == 6);

		userID = toInt(substringList[0]);
		username = substringList[1];
		password = substringList[2];
		name = substringList[3];
		email = substringList[4];
		allowAnonymousQuestions = toInt(substringList[5]);
	}

	std::string toString() {
		std::ostringstream oss;
		oss << userID << "," << username << "," << password << "," << name << "," << email << "," << allowAnonymousQuestions;

		return oss.str();
	}

	void printUserDetails() {
		std::cout << "User " << userID << ", " << username << " " << password << ", " << name << ", " << email << '\n';
	}
};

struct QuestionManager {
	// From question id to list of questions IDS on this question (thread questions) - All users
	std::map<int, std::vector<int>>questionIDQuestionThreadMap;

	// Map the question id to question object. Let's keep one place ONLY with the object
	std::map<int, Question> questionIDQuestionObjectMap;

	int lastID{};

	QuestionManager() {
		lastID = 0;
	}

	void loadDatabase() {
		lastID = 0;
		questionIDQuestionThreadMap.clear();
		questionIDQuestionObjectMap.clear();

		std::vector<std::string> lines = readFileLines("questions.txt");
		for (auto& line : lines) {
			Question question(line);
			lastID = std::max(lastID, question.questionID);

			questionIDQuestionObjectMap[question.questionID] = question;

			if (question.parentQuestionID == -1)
				questionIDQuestionThreadMap[question.questionID].push_back(question.questionID);
			else
				questionIDQuestionThreadMap[question.parentQuestionID].push_back(question.questionID);
		}
	}

	void fillUserQuestions(User& user) {
		user.questionIDsFromMe.clear();
		user.questionIDQuestionThreadMap.clear();

		for (auto& pair : questionIDQuestionThreadMap) {	// pair<int, std::vector<Question>>
			for (auto& questionID : pair.second) {		//  std::vector<Question>

				// Get the question from the map. & means same in memory, DON'T COPY
				Question& question = questionIDQuestionObjectMap[questionID];

				if (question.fromUserID == user.userID)
					user.questionIDsFromMe.push_back(question.questionID);

				if (question.toUserID == user.userID) {
					if (question.parentQuestionID == -1)
						user.questionIDQuestionThreadMap[question.questionID].push_back(question.questionID);
					else
						user.questionIDQuestionThreadMap[question.parentQuestionID].push_back(question.questionID);
				}
			}
		}
	}

	void printQuestionsReceived(User& user) {
		std::cout << '\n';

		if(user.questionIDQuestionThreadMap.size()==0)
			std::cout << "No Questions";

		for(auto&pair:user.questionIDQuestionThreadMap){		// pair<int, std::vector<Question>>
			for (auto& questionID : pair.second) {		//  std::vector<Question>
				// Get the question from the map. & means same in memory, DON'T COPY
				Question& question = questionIDQuestionObjectMap[questionID];
				question.printQuestionsReceived();
			}
		}
		std::cout << '\n';
	}

	void printQuestionsSent(User& user) {
		std::cout << '\n';
		if(user.questionIDsFromMe.size()==0)
			std::cout << "No Questions";

		for (auto& questionID : user.questionIDsFromMe) {		//  std::vector<Question>
			// Get the question from the map. & means same in memory, DON'T COPY
			Question& question = questionIDQuestionObjectMap[questionID];
			question.printQuestionsSent();
		}
		std::cout << '\n';
	}

	// Used in Answering a question for YOU.
	// It can be any of your questions (thread or not)
	int readQuestionID(User& user) {
		int questionID;
		std::cout << "Enter Question id or -1 to cancel: ";
		std::cin >> questionID;

		if (questionID == -1)
			return -1;

		if(!questionIDQuestionObjectMap.count(questionID)) {
			std::cout << "\nERROR: No question with this ID. Please try again\n\n";
			return readQuestionID(user);
		}
		Question& question = questionIDQuestionObjectMap[questionID];

		if(question.toUserID != user.userID){
			std::cout << "\nERROR: Invalid question ID. Please try again\n\n";
			return readQuestionID(user);
		}
		return questionID;
	}

	// Used to ask a question on a specific thread for whatever user
	int readQuestionIDThread(User& user) {
		int questionID;
		std::cout << "For thread question: Enter Question id or -1 for new question: ";
		std::cin >> questionID;

		if (questionID == -1)
			return -1;

		if(questionIDQuestionThreadMap.count(questionID)){
			std::cout << "No thread question with this ID. Please try again\n";
			return readQuestionIDThread(user);
		}
		return questionID;
	}

	void answerQuestion(User& user) {
		int questionID = readQuestionID(user);

		if (questionID == -1)
			return;

		Question& question = questionIDQuestionObjectMap[questionID];
		question.printQuestionsReceived();

		if (question.answerText != "")
			std::cout << "\nWarning: Already answered. Answer will be updated\n";

		std::cout << "Enter answer: ";	// if user entered comma, system fails :)
		std::getline(std::cin, question.answerText);	// read last enter
		std::getline(std::cin, question.answerText);
	}

	void deleteQuestion(User& user) {
		int questionID = readQuestionID(user);

		if (questionID == -1)
			return;

		std::vector<int>idsToRemove;	// to remove from questionIDQuestionObjectMap

		// Let's see if it's a thread or not. If it is a thread, remove all of it
		if(questionIDQuestionThreadMap.count(questionID)){ // thread
			idsToRemove = questionIDQuestionThreadMap[questionID];
			questionIDQuestionThreadMap.erase(questionID);
		}
		else {
			idsToRemove.push_back(questionID);
			// let's find in which thread to remove. Consistency is important when have multi-view
			for(auto&pair:questionIDQuestionThreadMap){
				std::vector<int>& vec = pair.second;
				for (int pos = 0; pos < (int)vec.size(); ++pos) {
					if (questionID == vec[pos]) {
						vec.erase(vec.begin() + pos);
						break;
					}
				}
			}
		}
		for (auto id : idsToRemove) {
			questionIDQuestionObjectMap.erase(id);
		}
	}

	void askQuestion(User& user, std::pair<int, int>toUserPair) {
		Question question;

		if (!toUserPair.second) {
			std::cout << "Note: Anonymous questions are not allowed for this user\n";
			question.anonymousQuestion = 0;
		}
		else {
			std::cout << "Is anonymous questions?: (0 or 1): ";
			std::cin >> question.anonymousQuestion;
		}

		question.parentQuestionID = readQuestionIDThread(user);

		std::cout << "Enter question text: ";	// if user entered comma, system fails :)
		std::getline(std::cin, question.questionText);
		std::getline(std::cin, question.questionText);

		question.fromUserID = user.userID;
		question.toUserID = toUserPair.first;

		// What happens in 2 parallel sessions who asked question?
		// They are given same id. This is wrong handling :)
		question.questionID = ++lastID;

		questionIDQuestionObjectMap[question.questionID] = question;

		if (question.parentQuestionID == -1)
			questionIDQuestionThreadMap[question.questionID].push_back(question.questionID);
		else
			questionIDQuestionThreadMap[question.parentQuestionID].push_back(question.questionID);
	}

	void listFeed() {
		for(auto&pair:questionIDQuestionObjectMap){
			Question& question = pair.second;

			if (question.answerText == "")
				continue;

			question.printFeedQuestions();
		}
	}

	void updateDatabase() {
		std::vector<std::string>lines;

		for (auto& pair : questionIDQuestionObjectMap)
			lines.push_back(pair.second.toString());

		writeFileLines("questions.txt", lines, false);
	}
};

struct UserManager {
	std::map<std::string, User> usernameUserObjectMap;
	User currentUser;
	int lastID{};

	UserManager() {
		lastID = 0;
	}

	void LoadDatabase() {
		lastID = 0;
		usernameUserObjectMap.clear();

		std::vector<std::string>lines = readFileLines("users.txt");
		for (auto& line : lines) {
			User user(line);
			usernameUserObjectMap[user.username] = user;
			lastID = std::max(lastID, user.userID);
		}
	}

	void AccessSystem() {
		const int possibilities = 2;
		int choice = optionsMenu(possibilities, { "Login", "Sign Up" });
		if (choice == 1)
			DoLogin();
		else
			DoSignUp();
	}

	void DoLogin() {
		LoadDatabase();	// in case user added from other parallel run

		while (true) {
			std::cout << "Enter user name & password: ";
			std::cin >> currentUser.username >> currentUser.password;

			if (!usernameUserObjectMap.count(currentUser.username)) {
				std::cout << "\nInvalid user name or password. Try again\n\n";
				continue;
			}
			User user_exist = usernameUserObjectMap[currentUser.username];

			if (currentUser.password != user_exist.password) {
				std::cout << "\nInvalid user name or password. Try again\n\n";
				continue;
			}
			currentUser = user_exist;
			break;
		}
	}

	void DoSignUp() {
		while (true) {
			std::cout << "Enter user name. (No spaces): ";
			std::cin >> currentUser.username;

			if (usernameUserObjectMap.count(currentUser.username))
				std::cout << "Already used. Try again\n";
			else
				break;
		}
		std::cout << "Enter password: ";
		std::cin >> currentUser.password;

		std::cout << "Enter name: ";
		std::cin >> currentUser.name;

		std::cout << "Enter email: ";
		std::cin >> currentUser.email;

		std::cout << "Allow anonymous questions? (0 or 1): ";
		std::cin >> currentUser.allowAnonymousQuestions;

		// What happens in 2 parallel sessions if they signed up?
		// They are given same id. This is wrong handling :)
		currentUser.userID = ++lastID;
		usernameUserObjectMap[currentUser.username] = currentUser;

		UpdateDatabase(currentUser);
	}

	void listUsernameIDs() {
		for(auto&pair:usernameUserObjectMap)
			std::cout << "ID #" << pair.second.userID << "\t\tName: " << pair.second.name << '\n';
	}

	std::pair<int, int> readUserID() {
		int userID;
		std::cout << "Enter User ID, or enter -1 to cancel: ";
		std::cin >> userID;

		if (userID == -1)
			return std::make_pair(-1, -1);

		for(auto&pair:usernameUserObjectMap){
			if (pair.second.userID == userID)
				return std::make_pair(userID, pair.second.allowAnonymousQuestions);
		}

		std::cout << "Invalid User ID. Try again\n";
		return readUserID();
	}

	void updateDatabase(User& user) {
		std::string line = user.toString();
		std::vector<std::string>lines(1, line);
		writeFileLines("users.txt", lines);
	}
};

/*
This part of the main.cpp function will end up being pretty all that is left
The helper functions at the top will eventually all end up in the UtilityFunction.h/cpp files
At this point, optionsMenu and readInteger are already in there
*/
int main() {
	AskMeSystem service;
	service.run();

	return 0;
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
