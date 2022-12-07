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
and functionality of the class type, and to extend or override those properties 
and functions as needed to implement its own unique behavior and functionality

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
