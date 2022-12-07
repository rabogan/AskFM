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
