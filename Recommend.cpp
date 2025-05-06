// The file implementing all the functions needed
// to produce all action desired from the Book recommender
// Includes comments, debug statements only ran when debug = true,
// and all the functions and the constructor
// to properly populate the private member variables
// and carry out all arithmetic computations successfully.


#include "Recommend.h"

bool DEBUG = false;

/*==========================================================================
  bool compareRatings(const pair<string, double>&, const pair<string, couble>&)) - a helper function to compare by rating and then
  book title - used by sort
  ===========================================================================*/

bool compareRatings(const pair<string, double>& rate1, const pair<string, double>& rate2)
{
	// I decided to remove the DEBUGs in this function, as they took up the
	// majority of the debug output, and I think it cluttered the existing
	// information that is more important for debugging.
	bool firstRatingIsGreater{ true };

	// statement checking if the double value of the pairs are not the same
	if (rate1.second != rate2.second) {

		// returns true if first rating is higher
		// otherwise returns false if second rating is higher
		if (rate1.second > rate2.second)
		{
			/*if (DEBUG)
			{
				cout << "Rate 1 is greater than rate 2" << endl;
			}*/
			return firstRatingIsGreater;
		}
		else
		{
			/*if (DEBUG)
			{
				cout << "Rate 1 is not greater than rate 2" << endl;
			}*/
			return !firstRatingIsGreater;
		}
	}

	// compares ASCII values, returns true if rate1 is first alphabetically
	// else returns false if rate2 is second alphabetically
	else if (rate1.first < rate2.first)
	{
		/*if (DEBUG)
		{
			cout << "Rate 1 comes before rate 2 alphabetically" << endl;
		}*/
		return firstRatingIsGreater;
	}
	else
	{
		/*if (DEBUG)
		{
			cout << "Rate 1 doesnt come before rate 2 alphabetically" << endl;
		}*/
		return !firstRatingIsGreater;
	}
}


/*==========================================================================
  Recommend(string)
  - constructor:
  - open file from file name passed in
  - read through file (2X), removing possible CR (Windows files)
  - create a set of books and produces a vector of books sorted in title order
  - build a recommender ratings map that shows the ratings for each recommendation
note: if a recommender does not rate a given title then the rating should be 0
call this ratings
- compute the average rating for each book
- call the book averages

DEBUGGING NOTE: use LOTS of "if (DEBUG) cout ...." statements in this code
===========================================================================*/
// Pass file name as parameter
Recommend::Recommend(string str) 
{
	//open file from file name passed in
	ifstream inFile;
	inFile.open(str);

	// sets to watch out for potential duplicates
	set<RECOMMENDER> uniqueRecommenders;
	set<BOOK_TITLE> uniqueBooks;

	// variables to hold in lines read in from getlines
	RECOMMENDER currentRecommender{ "" };
	BOOK_TITLE currentBook{ "" };
	string rating{ "" };

	// 1st read through, storing variables and calling removeCR to remove newlines on windows
	while (getline(inFile, currentRecommender) && getline(inFile, currentBook) && getline(inFile, rating)) {

		// use emplace to put BOOK_TITLE currBookTitle into title set
		// this is so no duplicates occur
		uniqueBooks.insert(removeCR(currentBook));

		// use emplace to put RECOMMENDER currRecommender into recNames set
		// this is so no duplicates occur
		uniqueRecommenders.insert(removeCR(currentRecommender));
	}

	// make books vector able to hold a vector as long as uniqueBooks
	books.resize(uniqueBooks.size());

	// copy values of uniqueBooks set into books vector
	copy(uniqueBooks.begin(), uniqueBooks.end(), books.begin());

	// sort the books alphabetically
	sort(books.begin(), books.end());

	// make books vector able to hold a vector as long as uniqueRecommenders
	recommenders.resize(uniqueRecommenders.size());

	// copy values of uniqueRecommenders set into recommenders vector
	copy(uniqueRecommenders.begin(), uniqueRecommenders.end(), recommenders.begin());

	// sort the recommenders alphabetically
	sort(recommenders.begin(), recommenders.end());

	// THIS IS A VECTOR OF SORTED UNIQUE BOOKS IN TITLE ORDER
	RECOMMENDER_LIST recommenders(uniqueRecommenders.begin(), uniqueRecommenders.end());

	// close and reopen the text file passed to the constructor in ifstream object inFile
	inFile.close();
	inFile.open(str);

	/*build a recommender ratings map that shows the ratings for each recommendation
		note : if a recommender does not rate a given title then the rating should be 0
		call this ratings*/

	// this loop sets the map of ratings to hold all unique Recommenders as keys
	// the values of the keys are all 0.0 but now can index all possible values
	// so we can use indexes in the while loop for ratings.second[i]
	for (const string& recommender : uniqueRecommenders) {
		ratings[recommender] = vector<double>(books.size(), 0.0);
	}

	if (DEBUG)
	{
		cout << " DEBUG: Ratings map size initialized to: " << ratings.size() << endl;
	}
	
	while (getline(inFile, currentRecommender) && getline(inFile, currentBook) && getline(inFile, rating))
	{
		// for all recommenders (does full iteration with each rec)
		for (const string& currentRecommender : recommenders)
		{
			// for each rec, loops over ever book
			for (long int i = 0; i < books.size(); i++)
			{
				// if getline(inFile, currentBook) is a match in list
				// checking current book
				if (DEBUG)
				{
					cout << "DEBUG: ";
					cout << books[i] << " is the current book." << endl;
				}
				if (removeCR(currentBook) == books[i])
				{
					// ratings map stores
					// currentRecommender in name key for vector
					// finds name key, accesses index i for vector<double>
					// and stores the getline(inFile, rating) as double 
					// at ratings[currentRecommender][i]
					if (DEBUG)
					{
						cout << " DEBUG: The current rating held by the file";
						cout << " and being stored in";
						cout << " ratings[person][index] is " << rating;
						cout << endl;
					}
					ratings[removeCR(currentRecommender)][i] = stod(rating); 
				}
			}
		}
	}

	inFile.close();

	// - compute the average rating for each book (done in function)
	// - call the book averages*/
	computeBookAverages();
}

/*==========================================================================
  computeRecommendation(RECOMMENDER)
  - find the top 3 similar recommenders
  - find the highest rated books from the top 3 recommenders
  ===========================================================================*/
void Recommend::computeRecommendation(RECOMMENDER rec)
{
	// -find the top 3 similar recommenders
	computeSimilarities(rec);

	// -find the highest rated books from the top 3 recommenders
	// to do this, need to compute the averages from the similarities
	// stored in similarList object
	computeSimAvg(similarList);
}

/*==========================================================================
  computeSimAvg(BOOK_AVG_LIST)
  - takes top 3 most similar names to recommender
  - averages the books that are non 0
  - returns best books in order
  ===========================================================================*/

void Recommend::computeSimAvg(BOOK_AVG_LIST recs)
{
	// input is of type vector<pair<stirng, double>>

	// set numOfBooks values to 0 for safety
	vector<int> numOfBooks(books.size());
	for (long int i = 0; i < books.size(); i++)
	{
		numOfBooks.at(i) = 0;
	}
	if (DEBUG)
	{
		cout << endl << "DEBUG: numOfBooks vector initialized to: ";
		for (const auto& i : numOfBooks)
		{
			cout << i << " ";
		}
		cout << endl;
	}

	// Initalize to 0
	vector<double> recSum(books.size());
	for (int i = 0; i < books.size(); i++)
	{
		recSum.at(i) = 0;
	}
	if (DEBUG)
	{
		cout << endl << "DEBUG: recSum vector initialized to: ";
		for (const auto& i : recSum)
		{
			cout << i << " ";
		}
		cout << endl;
	}

	// clear vector for safety since it used multiple times in this function 
	// if called twice and needs to be cleared
	simAvg.clear();

	// takes 3 books by directions
	for (int i = 0; i < 3; i++) {

		// find sum and numOfBooks vector values
		// loop iterates while j is less than ratings[key] size
		// where key is the current index of the vector pair
		if (DEBUG)
		{
			cout << "DEBUG: Outer loops iterates properly." << endl;
			cout << "Recs size has " << recs.size() << " elements" << endl;
		}

		// recs is type vector<pair<BOOK_TITLE, double>>
		// recs[i].firt = the strings i index
		if (DEBUG)
		{
			for (int i = 0; i < recs.size(); i++)
			{
				cout << "DEBUG: Element " << i << " of recs: " << recs[i].first << endl;
			}
			cout << "You should expect ";
			cout << ratings[recs[i].first].size() << " ratings for this person." << endl;

		}

		// all we've done so far
	// recSum, size of books vector
	// numBooks, size of books vector
		// put all this into a loop that happens three times

		for (long int j = 0; j < ratings[recs[i].first].size(); j++) {
			// If recommender left recommendation, increase
			// the number of books in the reviews
			// (exclude 0 b/c that the placeholde value for no review)

			// if ratings at parameter's specified index key and j != 0
			// (if a rating was left)
			if (DEBUG)
			{
				cout << "DEBUG: ";
				cout << "Rating " << j + 1 << " of " << recs[i].first << endl;
			}

			if (ratings[recs[i].first][j] != 0)
			{
				// increments counter for numOfBooks vector
				numOfBooks[j]++;
				if (DEBUG)
				{
					cout << "DEBUG: ";
					cout << "Rating wasn't zero, adding to numOfBooks." << endl;
					cout << "Num of books at this index is now: " << numOfBooks[j] << endl;
				}

				// adds to sum the value of the rating
				recSum[j] += ratings[recs[i].first][j];
				if (DEBUG)
				{
					cout << "DEBUG: ";
					cout << "Sum of ratings for this recommender: " << recSum[j] << endl;
				}
			}
			else
			{
				if (DEBUG)
				{
					cout << "DEBUG: Rating at this index was zero." << endl;
				}
			}
			if (DEBUG)
			{
				cout << "DEBUG: ";
				cout << "Loop was successful for iteration: " << j + 1 << endl;
			}
		}
	}

	// -  code that averages the books that are non 0
	// loops through again to find each average
	for (long int i = 0; i < books.size(); i++)
	{
		double average{ 0.0 };
		// if a recommendation was left, find its average
		// without this if, leads to inaccurate math and 
		// dividing by zero which is an error
		if (numOfBooks[i] != 0)
		{
			// store average by dividing corresponding indices
			if (DEBUG)
			{
				cout << "DEBUG: average being stored is: ";
				cout << recSum[i] / numOfBooks[i];
				cout << endl;
			}
			average = (recSum[i] / numOfBooks[i]);
		}

		// places at back of simAvg the books and average calculated in
		// the specific index of books
		// makes a pair at index of simAvg of book title and average rating of book
		// use emplace_back to avoid using push_back(make_pair))

		if (DEBUG)
		{
			cout << "This is the simAvg being stored: ";
			cout << "Pair 1st value is book: " << books[i];
			cout << ", second value is :" << average;
			cout << endl;
		}
		simAvg.emplace_back(books[i], average);
	}

	// - returns best books in order using sort
	sort(simAvg.begin(), simAvg.end(), compareRatings);
}

/*==========================================================================
  computeBookAverages()
  - compute and store (using bookAverages) the average rating for each book
  from all non 0 ratings
  - sort by highest rating
  ===========================================================================*/
void Recommend::computeBookAverages()
{
	
	// function calculating average by finding
	// averages and number of reviews for each book and
	// build  a vector of pairs containing book title : average (String : double)
	
	for (long int i = 0; i < books.size(); i++)
	{
		double average{ 0.0 };
		int numOfBooks{ 0 };

		for (const pair<string, vector<double>>& recommender : ratings)
		{
			// Add book review ONLY IF not zero
			// because it is placeholder value for vector
			// size consistency
			if (recommender.second[i] != 0) 
			{
				if (DEBUG)
				{
					cout << "DEBUG: ";
					cout << recommender.first << " rated the book " << books[i] << " a " << recommender.second[i];
					cout << endl;

				}
				average += recommender.second[i];
				numOfBooks++;
				//cout << endl;
			}
		}
		// if books were reviewed, add using the pair with avg/numOfBooks division
		if (numOfBooks != 0)
		{
			bookAverages.push_back(make_pair(books[i], (average / numOfBooks)));
		}
		else
		{
			// do not divide by 0, just push back 0 as the map
			// if no rating is given
			bookAverages.push_back(make_pair(books[i], (0.0)));
		}

	}

	if (DEBUG)
	{
		for (const auto& book : bookAverages)
		{
			cout << "DEBUG: ";
			cout << "Book: " << book.first << ", value: " << book.second << endl;
			cout << endl;
		}
	}
	// sort by highest rating
	sort(bookAverages.begin(), bookAverages.end(), compareRatings);
}


/*==========================================================================
  computeSimilarities(RECOMMENDER)
  - finds the most similar recommendations to the recommender using dot product
  - sort by highest rating
  ==========================================================================*/

void Recommend::computeSimilarities(RECOMMENDER rec)
{
	// Rec is the user you are finding similarity with
	// Basically, multiply everyone with "rec"
	// 
	// clear vector due to multiple use of it
	similarList.clear();

	// In all recommenders, iterate over with each person

	//TESTING
	if (DEBUG)
	{
		cout << "DEBUG: ";
		cout << "First print out ratings: " << endl;
		for (const pair<string, vector<double>>& recAndRatings : ratings)
		{
			int count = 0;
			cout << recAndRatings.first << " : ";
			// for each recRating pair, accesses rating in inner loop to build str

			for (const double& currRating : recAndRatings.second)
			{
				cout << currRating << " ";
				count++;
			}
			cout << "The current rater made a total of " << count << " ratings." << endl;
		}
		cout << endl << "we are checking for " << rec << " who's vector size is " << ratings[rec].size();
	}

	for (const string& person : recommenders) {
		double dot{ 0.0 };
		// Compute dot product
		//cout << "The first rater's vector size is: " << ratings[person].size();
		if (person != rec) {
			// dot product equals the persons values times parameters values
			for (long int i = 0; i < ratings[person].size(); i++)
				dot += ratings[person][i] * ratings[rec][i];

			// add that index of "recommenders" and
			// its dot product as a pair to similarList
			similarList.push_back(make_pair(person, dot));
		}
	}

	// Sort list to make it actually highest dot products first
	sort(similarList.begin(), similarList.end(), compareRatings);
}

/*==========================================================================
  bool checkRecommender(RECOMMENDER) - given a recommender's name, return
  true if recommender has made ratings, otherwise return false
  ==========================================================================*/
bool Recommend::checkRecommender(RECOMMENDER rec)
{
	// we are checking if "rec" parameter is a recommender
	// that is, if they appear in "RECOMMENDER recommenders"
	bool recHasMadeRatings{ false };

	for (const string& person : recommenders) {
		// If one matches, return true
		if (DEBUG)
		{
			cout << "DEBUG: ";
			cout << "Current person being checked: " << person << endl;
			cout << "Being compared against: " << rec << endl;
		}
		if (person == rec)
		{
			recHasMadeRatings = true;
		}

	}
	// this returns false as default
	// returns true if recHasMadeRatings was true
	// which happens if any index of person was equal to parameter
	return recHasMadeRatings;
}

/*==========================================================================
  double getBookAverage(BOOK_TITLE) - return average rating from bookAverages
  for a given book
  ==========================================================================*/
double Recommend::getBookAverage(BOOK_TITLE book) 
{

	double bookAverage{ 0.0 };

	// vector<pair<string, double>
	for (const pair<BOOK_TITLE, double>& currentAverage : bookAverages) {
		// If the .first value (pair's member value which in this case is string)
		// is equal to the string variable "book" passed to the function
		// it is checking equality of two strings using .first and parameter

		if (DEBUG)
		{
			cout << "DEBUG: ";
			cout << "Current average being printed: " << currentAverage.first;
			cout << endl;
			cout << "Needs to match: " << book << endl;
		}

		if (currentAverage.first == book)

			// if current iteration's title equals parameter,
			// store in BookAverage vector for that element that average
			bookAverage = currentAverage.second;
	}
	// returns the found book average from loop
	return bookAverage;
}

/*==========================================================================
  int getBookCount() - return the number of books
  ==========================================================================*/
int Recommend::getBookCount()
{
	int bookCount{ 0 };

	// makes the size_t "size" operator return into int
	// so it stores in bookCount, then returns
	bookCount = books.size();

	if (DEBUG)
	{
		cout << "DEBUG: ";
		cout << "We are returning " << bookCount;
		cout << " as the bookCount." << endl;
	}

	return bookCount;
}

/*==========================================================================
  int getRecommenderCount() - return the number of recommenders in ratings
  ==========================================================================*/
int Recommend::getRecommenderCount()
{
	int recommenderCount{ 0 };

	// makes the size_t "size" operator return into int
	// so it stores in recommenderCount, then returns
	recommenderCount = recommenders.size();


	if (DEBUG)
	{
		cout << "DEBUG: ";
		cout << "We are returning " << recommenderCount;
		cout << " as the recommenderCount." << endl;
	}
	return recommenderCount;
}

/*==========================================================================
  int getBookIndex(BOOK_TITLE) - return the index of a given book title in
  the books vector
  ==========================================================================*/
int Recommend::getBookIndex(BOOK_TITLE desiredBook)
{

	// checks while index < size of books
	for (long int bookIndex = 0; bookIndex < books.size(); bookIndex++)
	{
		if (books[bookIndex] == desiredBook)
		{
			// returns index if the index matches the inputted index
			if (DEBUG)
			{
				cout << "DEBUG: ";
				cout << "We are returning " << bookIndex;
				cout << " as the bookIndex." << endl;
			}
			return bookIndex;
		}
	}
	// if fails, returns first index
	return 0;
}
/*==========================================================================
  double getRecommenderBookRating(RECOMMENDER, BOOK_TITLE)
  - find the position of the title in books vector
  - find the recommender in ratings vector
  - return the recommender's rating for that title
  ==========================================================================*/

double Recommend::getRecommenderBookRating(RECOMMENDER rec, BOOK_TITLE desiredBook)
{
	// book rec holds the index of the book entered as an argument
	// so bookRec is the index of "desiredBook"
	int book = getBookIndex(desiredBook);

	// returns ratings of specified recommender and finds 
	// the specified rating by using the specific book index

	if (DEBUG)
	{
		cout << "DEBUG: ";
		cout << "We are accessing " << rec << "'s ";
		cout << "reiew for " << desiredBook << endl;
	}
	return ratings[rec][book];

}

/*==========================================================================
  printDotProducts(RECOMMENDER) - display dot products for current recommender
  ==========================================================================*/

void Recommend::printDotProducts(RECOMMENDER desiredRecommender)
{
	cout << "DOT PRODUCTS FOR RECOMMENDER: " << desiredRecommender << endl;
	cout << "==============================" << strDivider('=', desiredRecommender.size()) << endl;
	// iterates over all recommenders, holds curr recommender as tempRec
	for (const string& tempRec : recommenders) {
		double dot{ 0.0 };

		// if current iteration is not desired person
		// we need this because do not want to take dot product of person
		// this would just square the values
		if (tempRec != desiredRecommender) {
			cout << tempRec << ": ";
			// iterates over all ratings of the inputted person
			for (int i = 0; i < ratings[tempRec].size(); i++) {

				// stores the dot product by multiplying 
				// all of the tempRec person's ratings by
				// all of the desired persons	ratings

				dot += ratings[desiredRecommender][i] * ratings[tempRec][i];

				// output math in a cout
				 cout << ratings[desiredRecommender][i] << " * " << ratings[tempRec][i] << " + ";
				 if (DEBUG)
				 {
					 cout << "\tthe value of dot being stored here is: ";
					 cout << ratings[desiredRecommender][i] * ratings[tempRec][i];
					 cout << endl;
				 }
			}

			// ends stream with an = and the cumulative value of dot
			cout << " = " << dot << endl;
		}
	}

}


/*==========================================================================
  string strAverages() - return a string of computeBookAverages
  formatted for display
  ==========================================================================*/
string Recommend::strAverages()
{

	// strstream object to build output to return using .str()
	stringstream strAvgs;

	// 	BOOK_AVG_LIST bookAverages;    vector of books along with their rating average
	// iterates over all bookAverages with the temp recRating pair of string, double

	for (const pair<string, double>& bookRatingPair : bookAverages) {

		// builds str stream object to output the pair 
		// as desired for Prof Wooster formatting
		// by printing first value of pair (string) 
		// then space then second value of pair (double)
		strAvgs << bookRatingPair.first << " " << fixed
			<< setprecision(2) << bookRatingPair.second << endl;
	}

	// returns stringstream to strAverages output by using .str();
	return strAvgs.str();
}

/*==========================================================================
  printAverages() - display the bookAverages using strAverages()
  ==========================================================================*/
void Recommend::printAverages()
{
	// printing out the way Prof Wooster specifies
	cout << "BOOK AVERAGES" << endl;
	cout << "=============" << endl;
	// output stringstream built
	// that outputs string followed by rating double
	cout << strAverages();

}

/*==========================================================================
  string strRecommendationRatings() - return a string of ratings as read in
  from the file
  ==========================================================================*/
string Recommend::strRecommendationRatings()
{
	stringstream recRatingsStr;

	// iterates over books using "book"
	for (const string& currBook : books) {
		// adds current book to stringstream
		recRatingsStr << currBook << " / ";
	}

	recRatingsStr << endl;

	// ratings holds map of string and vector<double>
	for (const pair<string, vector<double>>& nameFirstRatingVectorSecond : ratings)
	{
		recRatingsStr << nameFirstRatingVectorSecond.first << ": ";
		// for each recRating pair, accesses rating in inner loop to build str

		for (const double& currRating : nameFirstRatingVectorSecond.second)
			recRatingsStr << currRating << " ";

	// basically, for each book, prints recommender and for each recommender prints all ratings
	// prints all ratings by all recommenders for all books
		recRatingsStr << endl;
	}
	// Return stringstream using .str();
	return recRatingsStr.str();
}

/*==========================================================================
  printRecommendationRatings - display ratings
  ==========================================================================*/

void Recommend::printRecommendationRatings()
{
	
	cout << "RECOMMENDATION RATINGS" << endl;
	cout << "======================" << endl;
	//literally just call this function
	cout << strRecommendationRatings();
}

/*==========================================================================
  string strSimilarList() - return a string of similarList
  ==========================================================================*/

string Recommend::strSimilarList()
{
	
	stringstream similarListStr;

	// each list stores name and value for dot product of similarList
	// BOOK_AVG_LIST similarList:   used to compute the top 3 ratings based on similarity among recommenders
	// #define BOOK_AVG_LIST vector<pair<BOOK_TITLE, double>>
	// similarList.push_back(make_pair(person, dot));
	// similar list holds pairs of recommender name and dot product

	for (const pair<string, double>& nameAndDotProductPair : similarList)
	{
		// builds string with dot product and name, prints dot product first
		similarListStr << fixed << setprecision(2)
		<< nameAndDotProductPair.second << ": "
		<< nameAndDotProductPair.first
		<< endl;
	}

	return similarListStr.str();
}

/*==========================================================================
  printSimilarList(RECOMMENDER) - display similarList for given recommender
  ==========================================================================*/

void Recommend::printSimilarList(RECOMMENDER rec)
{// compute Similarities brekaing in larger datasets

	// find similarity score of rec first
	computeSimilarities(rec);

	// Format output to match canvas files
	cout << "SIMILARITIES LIST FOR: " << rec << endl;
	// use strDivider after writing enough divider characters
	cout << "=======================" << strDivider('=', rec.size()) << endl;

	// Output result of function
	cout << strSimilarList();
}


/*==========================================================================
  string strSimAvg() - return a string of simAvg
  ==========================================================================*/

string Recommend::strSimAvg()
{
	stringstream simAvgStr;

	// make a pair of name of book followed by average and iterate
	// over simAvg container
	// simAvg: vector<pair<string, double>>

	for (const pair<string, double>& nameAndAverage : simAvg) { 

		// setprecision(2) to match decimal req
		// set by Prof Wooster to pass Gradescope
		simAvgStr << fixed << setprecision(2) << nameAndAverage.second
			<< ": " << nameAndAverage.first;
		simAvgStr << endl;
	}

	return simAvgStr.str();
}


/*==========================================================================
  printSimAvg(RECOMMENDER) - display simAvg for given recommender
  ==========================================================================*/

void Recommend::printSimAvg(RECOMMENDER rec)
{
	// Compute the averages
	computeRecommendation(rec);

	// formatting to match Wooster video
	cout << "SIMILARITY AVERAGES FOR: " << rec << endl;
	cout << "=========================" << strDivider('=', rec.size()) << endl;


	// Output string from strSimAvg
	cout << strSimAvg();
}

/*==========================================================================
  string strFancyRatings() - return a fancy formatted string of ratings
  ==========================================================================*/

string Recommend::strFancyRatings()
{
	stringstream ss;
	ss << right << setw(10) << " ";
	for (const auto& b : books)
	{
		ss << setw(13) << b << " ";
	}
	ss << endl;
	for (auto const& nameFirstRatingVectorSecond : ratings)
	{
		ss << right << setw(8) // 10 13 8 13
			<< nameFirstRatingVectorSecond.first << ": ";
		for (double rating : nameFirstRatingVectorSecond.second)
			ss << setw(13) << right << rating << " ";

		ss << endl;
	}

return ss.str();
}

/*==========================================================================
  printFancyRatings() - display a fancy formatted string of ratings
  ==========================================================================*/

void Recommend::printFancyRatings()
{
	// Build like shown on canvas video
	cout << "RECOMMENDATION BOOK RATINGS\n";
	cout << "===========================\n";

	// just output it
	cout << strFancyRatings();
}

/*==========================================================================
  string strRecommendation() - return a string of current recommendations
  ==========================================================================*/

string Recommend::strRecommendation()
{
	// stringstream to return at end of function
	stringstream recStr;

	for (int i = 0; i < simAvg.size(); i++) {
		// make a pair of title and value average in the one index of simAvg
		// sim avg has same NUM_OF_RECS_WANTED, so range is okay on for loop
		auto specificAvg = simAvg[i];

		// DEBUGGING
		if (DEBUG)
		{
			cout << "DEBUG: ";
			cout << "Current simAvg being processed is: ";
			cout << specificAvg.first << endl;
		}

		// outputs that index of simAvg using specificAvg's pair member functions
		// and special spacing and decimal rounding
		// pair<string, double>, so first = name, second = average
		if (simAvg[i].second > 0)
		{
			recStr << specificAvg.first << " " << fixed << setprecision(2) << specificAvg.second << endl;
		}
	}
	recStr << endl;

	return recStr.str();
}

/*==========================================================================
  string strBooks() - return a string of books
  ==========================================================================*/

string Recommend::strBooks()
{
	stringstream bookStr;

	// for specific book in books vector
	for (const string& currBook : books)

		// output the book followed by / for formatting
		bookStr << currBook << " / ";

	return bookStr.str();
}


/*==========================================================================
  printBooks() - display books
  ==========================================================================*/

void Recommend::printBooks()
{
	// match format on canvas, call strBooks to print formatted titles
	cout << "BOOKS: " << strBooks() << endl;
}

/*==========================================================================
  string strNames() - return a string of recommender names
  ==========================================================================*/

string Recommend::strNames()
{
	stringstream nameStr;

	// takes each recommender name in recommenders
	// and formats it in a list of all recommender names
	for (const string& currRecommender : recommenders)
		nameStr << currRecommender << " / ";

	return nameStr.str();
}

/*==========================================================================
  printNames() - display recommender names
  ==========================================================================*/

void Recommend::printNames()
{
	// prints names like how canvas wants them
	// same as strNames
	cout << "RECOMMENDERS: " << strNames() << endl;
}

/*==========================================================================
  printRecommendation(RECOMMENDER) - display recommendations for current
  recommender (which is passed in)
  ==========================================================================*/

void Recommend::printRecommendation(RECOMMENDER rec)
{

	// get recommendation for parameter value "rec"
	computeRecommendation(rec);

	// format to match canvas files
	cout << "RECOMMENDATION WITH: " << rec << endl;
	cout << "=====================" << strDivider('=', rec.size()) << endl;
	// uses divider to match canvas directions too

	cout << strRecommendation();
}

/*==========================================================================
  string removeCR(string) - remove ending CR from string
  ==========================================================================*/

string Recommend::removeCR(string str)
{
	// Function to remove CR only if 
	// last char of string is a carriage return
	// (leave other characters includng \n)
	string newStr = str;
	if (str[str.length() - 1] == '\r')
	{	
		newStr = str.substr(0, str.length() - 1);
	}

	// return altered string
	return newStr;
}

/*==========================================================================
  string strDivider(char, int) - create a string of char of length int
  ==========================================================================*/
string Recommend::strDivider(char character, int num)
{
	// Create string divider with divider = character
	// prints this character (num) times
	return string(num, character);

}

// This was the longest week of my life dear god.
