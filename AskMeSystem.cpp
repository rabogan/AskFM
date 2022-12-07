#include "AskMeSystem.h"
#include "UtilityFunction.h"

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
	//This is used to show the menu options and read user input.  The OptionsMenu will be a UtilityFunction, hence the inclusion of UtilityFunction.h
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
