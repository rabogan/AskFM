int readInteger(int low, int high) {
	const int maxAttempts = 3;
	std::cout << "\nEnter number in range " << low << " - " << high << ": ";
	int value;

	for (int i = 0; i < maxAttempts; i++) {

		std::cin >> value;

		//Check if the input value is a valid integer
		if (std::cin.fail())
		{
			//Clear error state and ignore remaining input
			std::cin.clear();
			//Used to clear the input stream and ignore any remaining input after an error is encountered
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  

			//Show error message and prompt the user for an integer!
			std::cout << "Error:  invalid input.  Please enter a valid integer in the specified range " << low << "-" << high << '\n'
				continue;  //Continue is used to avoid repetition
		}
		if (low <= value && value <= high)
			return value;

		//Show the error messages and prompts the user again for an integet in the correct range
		std::cout << "ERROR: invalid number.  Please enter a number in the specified range " << low << '-' << high << '\n';
	}
	//Maximum number of attempts reached
	std::cout << "ERROR: maximum number of attempts reached, input cancelled...\n";
	return -1;
}

int optionsMenu(const int num_options, ...) {
	//Created a va_list object to hold the menu options
	va_list options;
	//Initializes the va_list object to hold the menu options
	va_start(options, nums_options);
	//Show the menu options to the user
	std::cout << "\nMenu:\n";
	for (int i = 0; i < num_options; i++) {
		//Get the next menu option from the va_list object
		const char* option = va_arg(options, const char*);
		//Print the menu option to the console
		std::cout << "\t" << i + 1 << ": " << option << '\n';
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
			std::cout << "ERROR: Invalid input.  Please select a valid menu option.\n";
		}
	} while (choice == -1);

	//Return the valid input value
	return choice;
}
