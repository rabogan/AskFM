# AskFM
This project is adapting my previous AskFM project (learned from when I didn't use pointers, OOP, or even error handling, and aims to improve it!
The code is adapted from Mostafa Saad Ibrahim's code.  However, this attempts to make SIGNIFICANT improvements to certain aspects, such as the ability to handle errors, user-friendliness, and conformity with OOP principles

See the original here:
https://docs.google.com/presentation/d/1Ap3777-JwOExXrHGjKdxCDg9rQFjMKV9/edit#slide=id.p1

December 6th UPDATE:
1)  Some variable names were improved
2)  The optionsMenu (previously called ShowReadMenu) utility function has an improved signature, a va_list object to hold menu options, and much better error handling - as well as validation of user input.  The new optionsMenu is GUARANTEED to return a valid input value
3)  The readInteger utility function (aka readInt) also has significant improvements, not limited to the declaration/definition separation.  It is now capable of handling invalid input, it is MUCH more user-friendly: it has CLEAR error messages, and allows a maximum number of attempts at input from the user before stopping (rather than repeating the same thing over and over)
4)  The AskMeSystem struct has been adapted into a CLASS in order to obey OOP principles, such as encapsulation and abstraction.  At the time of writing, the struct has been converted, and tentative .h and .cpp files have been written.  I want to change a few other things before making separate files for these things 

IMMEDIATE FUTURE PLANS:
1) Change the run() function within AskFM (specific)
2) Eliminate the using namespace std; (specific)
3) Change the STRUCTS into CLASSES, so that the code follows basic OOP principles
4) Improve error handling in all aspects of the code
