PERSONAL EXPENSE TRACKER - OOCP MINOR PROJECT

Requirements
- C++20 compiler (g++ recommended)
- Visual Studio Code or another C++ editor

Compile from the project root directory:
  g++ -std=c++20 -Wall -Wextra -pedantic Source/*.cpp -o Executable/PersonalExpenseTracker.exe

Run on Windows PowerShell from the project root:
  .\Executable\PersonalExpenseTracker.exe

Run on Linux/macOS from the project root:
  ./Executable/PersonalExpenseTracker.exe

Important: Run the executable with the project root as the working directory so that the Data/ folder is found. The program creates Data/ if it does not exist. It saves users.txt, categories.txt, expenses.txt and budgets.txt in that folder.

Data format: UTF-8-compatible plain text, one record per line, fields separated by |. The program replaces | and line breaks in user-entered text with spaces so records can be parsed again.

Menu options 1-9 follow the official blueprint. Options 10 and 11 are additional convenience options for budget and user-profile management. Option 7 generates a monthly report; Option 6 processes spending against configured budgets.

Persistence test:
1. Add an expense and exit with option 9.
2. Reopen the program from the project root.
3. Choose Display Records and verify the expense is still present.

The project demonstrates a dynamic Expense array managed with new[] and delete[], inheritance from abstract Record to Expense, and runtime polymorphism through a Record reference. Do not claim successful testing in the report until you have run the tests on your machine.
