#include "AskMeSystem.h"
#include "UtilityFunction.h"

void AskMeSystem::loadDatabase(bool fillUserQuestions) {
	userManager.loadDatabase();
	questionManager.loadDatabase();
	if (fillUserQuestions)  // first time, waiting for login
		questionManager.fillUserQuestions(userManager.currentUser);
}
void AskMeSystem::run() {
	loadDatabase();
	userManager.accessSystem();
	questionManager.fillUserQuestions(userManager.currentUser);
	//A clear exit condition has been set up for the loop
	bool exit = false;
	while (!exit) {
	//This is used to show the menu options and read user input.  The OptionsMenu will be a UtilityFunction, hence the inclusion of UtilityFunction.h
	const int possibilities = 8;
	int choice = optionsMenu(possibilities, "Print Questions To User", "Print Questions From User",
	"Answer Question", "Delete Question", "Ask Question", "Print System Users", "List Feed", "Log Out");
	loadDatabase(true);
	if (choice == 1)
		questionManager.printQuestionsReceived(userManager.currentUser);
	else if (choice == 2)
		questionManager.printQuestionsSent(userManager.currentUser);
	else if (choice == 3) {
		questionManager.answerQuestion(userManager.currentUser);
		questionManager.updateDatabase();
	} else if (choice == 4) {
		questionManager.deleteQuestion(userManager.currentUser);
		// Let's build it again (just easier, but slow)
		questionManager.fillUserQuestions(userManager.currentUser);
		questionManager.updateDatabase();
	} else if (choice == 5) {
		std::pair<int, int>toUserPair = userManager.readUserID();
		if (toUserPair.first != -1) {
			questionManager.askQuestion(userManager.currentUser, toUserPair);
			questionManager.updateDatabase();
		}
	} else if (choice == 6)
		userManager.listUsernameIDs();
	else if (choice == 7)
		questionManager.listFeed();
		//If the user chooses the eighth option, set the exit condition to true
	else
		exit = true;
	}
}
