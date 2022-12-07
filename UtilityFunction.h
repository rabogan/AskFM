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
int readInteger(int low, int high);

/*
The optionsMenu function uses a do-while loop to read and validate user input.
The loop continues until a valid input value is input.
If -1 is returned, then an error message is displayed, and the user prompted again
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
This ENSURES the function only returns a valid input value, which can then be used to determine which action to perform in the main program
Overall, this is more flexible, easier to use, and better at handling input values than the original version
*/
int optionsMenu(const int num_options, ...);
