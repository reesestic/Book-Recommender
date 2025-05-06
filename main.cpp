// Name: Reese Stichter
// Class: CPSC-1020
// Date: 4/23/2024
// Project 4
// Approximate time invested: 40 hours
// Description: Main file for Recommend
// This is the file where the user can interact and select
// a test file for data to output.
// It has all options from Wooster's video and allows
// user to print values until they type "quit".


#include "Recommend.h"
#include <iostream>
using namespace std;

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		cerr << "Invalid number of given arguments.";
		return 1; // exit because did not match expected input
	}

	Recommend data(argv[1]);

	//Recommend data("ratings_small.dat");
	
	// add input for program to use
	string input{ "" };
	string throwAway{ "" };

	// local variable to store recommender for specific programs

	// For functions that need recommender to exist, 
	// need to check if it exists and ask for input for it if it doesnt
	string recommender{ "" };
	while (input != "quit")
	{
		cin >> input;

		if (input == "averages") 
		{
			// print averages of books
			data.printAverages();
		}

		else if (input == "ratings")
		{
			// print reccomendation ratings for people
			data.printRecommendationRatings();
		}

		else if (input == "books") 
		{
			// print the vector of books
			data.printBooks();
		}
		else if (input == "debug")
		{
			// toggles debug to output data
			// put in .cpp to check values and
			// debug the program
			DEBUG = !DEBUG;
			cout << "Debug is now: " << DEBUG;
		}

		else if (input == "dotprod") 
		{
			// print out the dot product for the specific person
			// check if rec exists first

			if (recommender == "")
			{
				cout << "First need a recommender, please input one." << endl;
				cin >> recommender;
			}
			data.printDotProducts(recommender);
		}

		else if (input == "file") 
		{
			// outputs the data in the fancy format way
			data.printFancyRatings();
		}

		else if (input == "menu") 
		{
			// Print out all options of the program
			// formatted like in Woosters video
			cout << "BOOK RECOMMENDATION SYSTEM MENU" << endl;
			cout << "===============================" << endl;

			cout << "averages - display average ratings for each book" << endl;
			cout << "books - display books in alphabetic order" << endl;
			cout << "debug - toggles debugging outputs" << endl;
			cout << "file - fancy display of file data" << endl;
			cout << "menu - display this menu" << endl;
			cout << "books - display books in alphabetical order" << endl;
			cout << "names - display recommender names" << endl;
			cout << "ratings - print recommendation ratings" << endl;
			cout << "recommend <name> - display book recommendations based upon recommender <name>" << endl;
			cout << "sim - display current similarities" << endl;
			cout << "simavg - display averages for current similarities" << endl;
			cout << "quit - exits the program" << endl;
		}

		else if (input == "names") 
		{
			// print out the recommenders vector
			data.printNames();
		}

		else if (input == "ratings") 
		{
			// print the ratings out
			data.printRecommendationRatings();
		}

		else if (input == "recommend")
		{
			// We need to ask for a recommend
			//cin >> recommender
			getline(cin, throwAway);
			getline(cin, recommender);

			// If user left recs, use their data to 
			// make a recommendation
			// ONLY IF PERSON EXISTS THOUGH
			if (data.checkRecommender(recommender))
			{
				// print out the generated reccomendation
				// since the person exists
				data.printRecommendation(recommender);
			}
		}

		else if (input == "sim")
		{
			// print similar last based on given recommender
			if (recommender == "")
			{
				cout << "First need a recommender, please input one." << endl;
			}
			data.printSimilarList(recommender);
		}

		else if (input == "simavg")
		{
			//print out average values of
			// similarity based on recommender
			if (recommender == "")
			{
				cout << "First need a recommender, please input one." << endl;
			}
			data.printSimAvg(recommender);
		}

		else if (input == "quit")
		{
			// do nothing, quits on next loop
		}

		else
		{
			cout << "input not found. Please try again"; // << endl;
		}
		// all options get an endline
		cout << endl;
	}

	return 0;
}