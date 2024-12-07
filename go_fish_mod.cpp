#include <iostream>
#include <fstream>
#include <cstdlib>
using namespace std;

//------------------------------- CONSTANTS ------------------------------------

const char SUITES[] = {'C', 'D', 'H', 'S'};
const string RANKS[] = {"A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", 
                                                                      "K", "Q"};

const int NUM_RANKS = 13;
const int SIZE_OF_GUESSES = 5;

//-------------------------------- CLASSES -------------------------------------
struct Card 
   {
    //array of strings with the rank in the first index and the suit in the 
    //second
    string rankAndSuite[2];
   };

class Deck
   {
    private:
        //array to hold the cards in the deck
        Card cards[ 52 ];
        //integer to hold the current size of the deck
        int size = 52;

    public:
        Card drawRandomCard();   //method to draw a random card and update deck
        void populateDeck();   //method to populate the deck before match
        void displayDeck();   //debug method

        bool isEmpty()
           {
            return size == 0;
           }
        
        int getSize(){return size;}
   };

class Hand
   {
    private:
        Card cards[ 52 ];
        int size = 0;

    public:
        //gets 7 cards from the deck and replaces the cards in the cards array
        //with them
        void dealCards( Deck *deck );
        //bool isInHand();
        bool getAllRank( string targetRank, Card *cardsFound, 
                                                        int *numCardsReturned );
        Card removeCard( int removeIndex );   //method to remove a specific index
        void addCards( Card *newCards, int numCards );   //method to add several cards to the hand
        void displayHand();   //method to display the hand to the console
        void goFish( Deck *deck, bool verbose );
        int checkForSets( string *setsCollected, int *sizeOfSets );
        bool isInHand( string targetRank );

        int getSize()
           {
            return size;
           }

        string getRank( int index )
           {
            return cards[ index ].rankAndSuite[0];
           }
   };


//------------------------- FUNCTION PROTOTYPES --------------------------------
void runGame( Hand *playerHand, Hand *cpuHand, Deck *deck, string moveList);
int humanTurn( Deck *deck, Hand *playerHand, Hand *cpuHand, string *setsCollected, 
             int *sizeOfSets, string *playerGuesses, int *numberOfPlayerGuesses,
             string moveList, int *moveListIndex );
int cpuTurn(  Deck *deck, Hand *playerHand, Hand *cpuHand, string *setsCollected, 
             int *sizeOfSets, string *playerGuesses, int *sizeOfPlayerGuesses );
string choosePlay( Deck *deck, Hand *cpuHand, string *playerGuesses, 
                                   string lastGuess, int *sizeOfPlayerGuesses );
string verifyInput( string inString, Hand *playerHand );

//AFL mod functions  V V V
bool readNextValidInput( string moveList, int *moveListIndex, string *newMove,
                         Hand *playerHand );

//-------------------------- MAIN PROGRAM --------------------------------------
int main( int argc, char** argv)
   {
    //verifying propper commandline arguments
    if( argc != 2 ) 
       {
        //print error 
        cerr << "Usage: " << argv[0] << "<input file>" << endl;
        //return with exit status 1
        return 1;
       }

    //setting up input file
    ifstream inputFile(argv[1]);

    //verifying file opens
    if(!inputFile)
       {
        cerr << "Error: File could not be opened!" << endl;
        return 1;
       }

    //reading input from file, and close input file
    string moveList((istreambuf_iterator<char>(inputFile)), istreambuf_iterator<char>());
    inputFile.close();


    //create deck, as well as player and cpu hands
    Hand cpuHand, playerHand;
    Deck gameDeck;

    runGame(&playerHand, &cpuHand, &gameDeck, moveList);


    return 0;
   }
//--------------------- MEMBER FUNCTION DEFINITIONS ----------------------------
void Deck::displayDeck()
   {
    //loop over the number the cards in the deck      DEBUG FUNCTION! DELETE!
    for( int i = 0; i < size; i++ )
       {
        //print out the suit and the rank 
        cout << cards[ i ].rankAndSuite[ 0 ] << cards[ i ].rankAndSuite[ 1 ];
        cout << " ";

        if( i%12 == 0)
           {
            cout << endl;
           }  
       }
   }

Card Deck::drawRandomCard()
   {
    //generate a randomIndex between (and including) 0 and whatever the last
    // index of the current deck is
    int randomIndex = rand() % (size);

    //save the card to be returned (drawn)
    Card returnCard = cards[ randomIndex ];

    //adjusting the array after card removal
    for( int index = 0; index < size - randomIndex - 1; index ++ )
       {
        //setting the card we removed as the next one in the array
        //(Basically moving every card down by 1)
        cards[ randomIndex + index ] = cards[ randomIndex + index + 1];
       }
    
    //update the size of the deck
    size--;

    //return the removed card
    return returnCard;
   }

//funciton to fill the deck to the standard 52 cards with 4 suits and 13 ranks
void Deck::populateDeck()
   {
    //int to keep track of what index we are in
    int cardNum = 0;

    //loop over the possible suits
    for( int suit = 0; suit < 4; suit++ )
       {
        //loop over the possible ranks
        for( int rank = 0; rank < 13; rank++ )
           {
            //update the rank and suit for each card index (cardnum)
            cards[ cardNum ].rankAndSuite[0] = RANKS[ rank ];
            cards[ cardNum ].rankAndSuite[1] = SUITES[ suit ];
            cardNum++; 
           }
       }    
   }



void Hand::dealCards( Deck *deck )
   {
    //deal 7 cards from the deck into the current hand
    //this increases the hand size and decreases the deck size
    for( int index = 0; index < 7; index++ )
       {
        cards[ index ] = deck->drawRandomCard();    
        size++;
       }
   }

//this function checks to see if a rank is in a hand, if so it removes the cards
//and returns them along with the number of cards gained
bool Hand::getAllRank( string targetRank, Card *cardsFound, 
                                                                int *numCardsReturned )
   {
    //bool flag for return
    bool inHand = false;


    //loop over the cards on hand
    for( int index = 0; index < size; index++ )
       {
        //if the ranks match the target rank...
        if( cards[ index ].rankAndSuite[0] == targetRank) 
           {
            //...update the return flag
            inHand = true;
            //add card to the cards to be transfered
            cardsFound[ *numCardsReturned ] = cards[ index ];
            *numCardsReturned = *numCardsReturned + 1;
            //remove the card at this index
            removeCard( index );
            index--;
           }
       }
    return inHand;
   }



Card Hand::removeCard( int removeIndex )
   {
    Card returnCard = cards[ removeIndex ];

    for( int index = 0; index < size - removeIndex - 1; index++ )
       {
        cards[ removeIndex + index ] = cards[ removeIndex + index + 1];
       }
    //update size
    size--;

    return returnCard;
   }

void Hand::addCards( Card *newCards, int numCards )
   {
    //loop through the given new cards
    for( int index = 0; index < numCards; index++ )
       {
        //add each card to the next available space in the array
        cards[ size ] = newCards[ index ];
        size++;
       }
   }


void Hand::displayHand()
   {
    //loop over the cards on hand
    for( int index = 0; index < size; index++ )
       {
        //print the suit and rank of each
        cout << cards[ index ].rankAndSuite[0] << cards[ index ].rankAndSuite[1];
        cout << " ";
       }
   }


void Hand::goFish( Deck *deck, bool verbose )
   {
    //draw a random card from the deck
    cards[size] = deck->drawRandomCard();

    //print the card drawn
    if(verbose)
       {
        cout << "New Card: " << cards[size].rankAndSuite[0];
        cout << cards[size].rankAndSuite[1] << ".";
       }

    //update number of cards on hand
    size++; 
   }


//returns the number of sets collected (1 or 0) and adds the set to the set 
//array as well as updates the size of said array
int Hand::checkForSets( string *setsCollected, int *sizeOfSets )
   {
    //integer to store number of cards of specific rank
    int numCards = 0;
    int cardIndecies[4];

    //looping over every rank
    for( int rank = 0; rank < 13; rank++ )
       {
        //looping over every card on hand
        for( int index = 0; index < size; index++  )
           {
            //check if the card matches the current rank
            if( cards[ index ].rankAndSuite[0] == RANKS[ rank ] )
               {
                cardIndecies[numCards] = index;

                //if so increment the cards found for that rank
                numCards++;
               }
           }

        //check if a set has been collected
        if( numCards >= 4 )
           {
            //add the rank to the array of sets collected during the game
            setsCollected[ *sizeOfSets ] = RANKS[ rank ];
            *sizeOfSets = *sizeOfSets + 1;

            //remove set from hand
            for( int index = 3; index >= 0; index--)
               {
                removeCard( cardIndecies[index] );
               }
            
            return 1;
           }
        numCards = 0;
       } 
    return numCards;
   }


bool Hand::isInHand( string targetRank )
   {
    //loop over all cards on hand
    for( int index = 0; index < size; index++ )
       {
        //if card rank matches target rank return true
        if( cards[ index ].rankAndSuite[0] == targetRank )
           {
            return true;
           }
       }
    return false;
   }



//------------------------ FUNCTION DEFINITIONS --------------------------------


//Function: runGame( Hand, Hand, Deck)
//Process: 1) Set Up variables
//         2) populate the deck (add 52 cards in order) 
//         3) deal 5 cards to player hand
//         4) deal 5 cards to ai hand
//         5) display player hand
//         2) Loop while there are still cards in the deck
//              3) Human takes turn  
//              4) CPU takes turn
//              5) Points are added to player scores 
//         6) Scores are compared, larger score wins
void runGame( Hand *playerHand, Hand *cpuHand, Deck *deck, string moveList)
   {

    //integer for drawing extra cards in the case when deck is low and hand is empty
    int cards;

    //index for keeping track of the position within the moveList
    int moveListIndex = 0;

    //score vars
    int humanScore = 0; int retVal;
    int cpuScore = 0;
    

    string setsCollected[ 13 ];
    int sizeOfSets = 0;

    string playerGuesses[SIZE_OF_GUESSES];  //used for deciding what move the AI will choose
    int numberOfPlayerGuesses; //stores the total number of guesses made by the
                               //player during the match... HOWEVER:
                                    //the array will only have the most recent SIZE_OF_GUESSES
                                    //guesses. Mod will be used to keep the most
                                    //recent SIZE_OF_GUESSES guesses in the array

    //setup the deck with the 52 cards
    deck->populateDeck();

    //deal cards to the players
    playerHand->dealCards( deck );
    cpuHand->dealCards( deck );

    //title and intro prints
    cout << "Game Starting!\n" << "Your Starting Hand:\n";
    playerHand->displayHand();
    cout << endl;
    

    //while the deck isn't empty:
    while( !deck->isEmpty() )
       {
        retVal = humanTurn( deck, playerHand, cpuHand, setsCollected, 
                       &sizeOfSets, playerGuesses, &numberOfPlayerGuesses,
                       moveList, &moveListIndex );
       
        //human turn
        if(retVal > 0)
           {
            cout << "\nYou Collected a Set!!" << endl;
            cout << "Removing Rank: " << setsCollected[sizeOfSets - 1] << endl;

            humanScore++;
           }
        else if( retVal == -1 )
           {
            //error status, ran out of input moves
            cout << endl << "Out of Moves! Exiting Program..." << endl << endl;
            return;
           }

        cout << "\n= = = = = = = = = = = = = = = = = = = = = = = = = = = = =\n";

        //check if the deck is empty
        if(deck->isEmpty())
           {
            break;
           }

        //cpu turn
        if(cpuTurn( deck, playerHand, cpuHand, setsCollected, 
                       &sizeOfSets, playerGuesses, &numberOfPlayerGuesses ) > 0)
           {
            cout << "\nCPU Has Collected a Set!!" << endl;
            cout << "Removing Rank: " << setsCollected[sizeOfSets - 1] << endl;

            cpuScore++;
           }
        cout << "\n=========================================================\n";

        if(deck->isEmpty())
           {
            break;
           }

        if(playerHand->getSize() == 0)
           {
            if( deck->getSize() < 7)
               {
                cards = deck->getSize();
               }
            else 
               {
                cards = 7;
               }

            for(int iteration = 0; iteration < cards; iteration++)
               {
                playerHand->goFish( deck, true );
               }
           }
        else if(cpuHand->getSize() == 0 )
           {
            if( deck->getSize() < 7)
               {
                cards = deck->getSize();
               }
            else 
               {
                cards = 7;
               }

            for(int iteration = 0; iteration < cards; iteration++)
               {
                cpuHand->goFish( deck, false );
               }
           }
       }

    cout << "\n\n\n\n\n\n The Deck Is Empty!\n\n\n";
    
    if( humanScore > cpuScore)
       {
        cout << "  You Win!  :)\n";
        cout << "--------------\n";
        cout << "Your Score: " << humanScore << endl;
        cout << "CPU Score: " << cpuScore << endl << endl;
       }
    else
       {
        cout << "  You Lost!  :(\n";
        cout << "---------------\n";
        cout << "Your Score: " << humanScore << endl;
        cout << "CPU Score: " << cpuScore << endl << endl;
       }
   }


// Function: humanTurn( Deck, Hand, Hand, string*, int*, string*, int*)
// --------------------------------------------------------------------
// Description: This function allows the user to guess a rank and checks to see
//              if that rank is in the CPU's hand.
//                  If it is, cards are removed from the CPU's hand, and placed
//                  in the cardsGained array, along with the number of cards
//
//              if it isn't, 
// Process: 
//          1) Set up variables
//          2) Prompt user for Rank
//          3) Add player guess to the 3 element temp array for guesses 
//          4) Check if CPU has guessed rank
//              5) If so
//          1)
//          1)
//          1)


int humanTurn( Deck *deck, Hand *playerHand, Hand *cpuHand, string *setsCollected, 
             int *sizeOfSets, string *playerGuesses, int *numberOfPlayerGuesses,
             string moveList, int *moveListIndex )
   {
    //array of cards gained from the rank guess and integer for number of cards
    //gained
    Card cardsGained[4];
    int numCardsGained = 0;  

    //- - - - - - - - - - - - - - AFL MODS - - - - - - - - - - - - - -  - - - - 
    string moveInput;
    if( !readNextValidInput( moveList, moveListIndex, &moveInput, playerHand))
       {
        //return with error status
        return -1;
       }
    //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 



    // //string to store user input
    // string inString;

    // //prompt user for rank#
    // cout << "\nIt's your turn. Ask for a rank/number (A-10, J, K, Q): ";
    
    // //get user input
    // cin >> inString;                                                            // GETTING INPUT, CHANGE TO NEW FORMAT

    //verify user input
    //string verifiedInput = verifyInput( inString, playerHand );

    //record the user guess and update size
    playerGuesses[ *numberOfPlayerGuesses % SIZE_OF_GUESSES] = moveInput;
    if(*numberOfPlayerGuesses < 5)
       {
        *numberOfPlayerGuesses = *numberOfPlayerGuesses + 1;
       }


    
    //check to see if given rank is in the cpuHand
    if( cpuHand->getAllRank( moveInput, cardsGained, &numCardsGained ) )
       {
        //if so let user know what cards they gained
        cout << "\nOpponent has [";
        for( int index = 0; index < numCardsGained; index++ )
           {
            cout << " " << cardsGained[ index ].rankAndSuite[0];
            cout << cardsGained[ index ].rankAndSuite[1];
           }
        cout << " ] ";
        cout << "You collect all " << moveInput << "s.\n";

        //update player hand
        playerHand->addCards( cardsGained, numCardsGained );

        //display the new player hand
        cout << "Your hand: \n";
        playerHand->displayHand();
        cout << endl;
       }
    else
       {
        //let user know about go fish
        cout << "\nOpponent does not have " << moveInput << ". You go fish!\n";
        
        //draw the random card to the player hand
        playerHand->goFish( deck, true );


        //display the new Hand
        cout << "\nYour hand:\n";
        playerHand->displayHand();


       }
    
    //check for any sets, return the number of sets found and update set array
    return playerHand->checkForSets( setsCollected, sizeOfSets );
   }

//ran out of time
int cpuTurn(  Deck *deck, Hand *playerHand, Hand *cpuHand, string *setsCollected, 
             int *sizeOfSets, string *playerGuesses, int *sizeOfPlayerGuesses )
   {
    static string lastGuess = "";

    //array of cards gained from the rank guess and integer for number of cards
    //gained
    Card cardsGained[4];
    int numCardsGained = 0;

    //choose what card to ask for
    string cpuGuess = choosePlay( deck, cpuHand, playerGuesses, lastGuess, sizeOfPlayerGuesses );
    lastGuess = cpuGuess;

    cout << "\nOpponent Guesses: " << cpuGuess;
    //check to see if given rank is in the cpuHand
    if( playerHand->getAllRank( cpuGuess, cardsGained, &numCardsGained ) )
       {
        //if so let user know what cards they gained
        cout << "\nYou Have: [";
        for( int index = 0; index < numCardsGained; index++ )
           {
            cout << " " << cardsGained[ index ].rankAndSuite[0];
            cout << cardsGained[ index ].rankAndSuite[1];
           }
        cout << " ] ";
        cout << "They collect all " << cpuGuess << "s.\n";

        //update player hand
        cpuHand->addCards( cardsGained, numCardsGained );

        //display the new player hand
        cout << "Your hand: \n";
        playerHand->displayHand();
       }
    else
       {
        //let user know about go fish
        cout << "\nYou do not have " << cpuGuess << ". They go fish!\n";
        
        //draw the random card to the player hand
        cpuHand->goFish( deck, false );


        //display the new Hand
        cout << "\nYour hand:\n";
        playerHand->displayHand();
        cout << endl;

       }
    
    //check for any sets, return the number of sets found and update set array
    return cpuHand->checkForSets( setsCollected, sizeOfSets );
   }

string choosePlay( Deck *deck, Hand *cpuHand, string *playerGuesses, 
                                    string lastGuess, int *sizeOfPlayerGuesses )
   {

    //static int oldGuessIndex;
    //int newGuessIndex;

    string retStr = "";
    cout << "\nDEBUG: sizeOfPlayerGuesses = " << *sizeOfPlayerGuesses << "!\n";
    //check to see if one of the opponents most recent guesses is in the hand
    for( int index = 0; index < *sizeOfPlayerGuesses; index++ )
       {
        //check to see if guess is in hand
        if( *sizeOfPlayerGuesses > 0 && cpuHand->isInHand( playerGuesses[ index ] )
            && lastGuess != playerGuesses[index] )
           {
            cout << "\nDEBUG: GUESSING PREV PLAYER GUESS\n";
            retStr = playerGuesses[ index ];
           }
       }

    if( retStr == "")
       {
        //ask for a rank within the hands cards
        int randIndex = rand() % (cpuHand->getSize());
        
        retStr = cpuHand->getRank( randIndex );
       }
    return retStr;
   }


string verifyInput( string inString, Hand *playerHand ) 
   {

    //sentinal flag
    bool inputVerified = false;

    //verification loop
    while( !inputVerified )
       {
        //compare inString to all ranks
        for( int index = 0; index < playerHand->getSize(); index++)
           {
            if(playerHand->isInHand(inString))
               {
                //if match, exit loop
                inputVerified = true;
                return inString;
               }
           }
        //if invalid, alert user, continue looping
        cout << "\nThe Rank You Guess Must Be In Your Hand!\n";
        cout << "Ask for a rank/number (A-10, J, K, Q): ";
        cin >> inString;                                                        // GETTING INPUT, CHANGE TO NEW FORMAT
       }
    return inString;
   }

//takes in a string of comma-seperated moves (ranks) and returns the next one
// in the list according to a moveListIndex
bool readNextValidInput( string moveList, int *moveListIndex, string *newMove,
                         Hand *playerHand )
   {
    
    string inMove = "";
    *newMove = ""; 
    char inChar;

    bool verified = false;

    //read next move in the "list"
    cout << "\nDEBUG ~ Full?: " << !(*moveListIndex < (int)moveList.size()) << endl;

    while(!verified && *moveListIndex < (int)moveList.size())
       {
        inChar = moveList[*moveListIndex];
        cout << "\nDEBUG ~ INCHAR: " << inChar << endl;

        while( inChar != ',' && *moveListIndex < (int)moveList.size())
           {
            inMove += inChar;

            *moveListIndex = *moveListIndex + 1;
            inChar = moveList[*moveListIndex];
           }
        //prepare index for next loop
        *moveListIndex = *moveListIndex + 1;
        cout << "DEBUG  ~ POS_MOVE: " << inMove << endl;

        //verifying the input
        for( int index = 0; index < playerHand->getSize(); index++)
           {
            if(playerHand->isInHand(inMove))
               {
                cout << "\nDEBUG ~ NEW MOVE: " << inMove;
                //if match, exit loop
                verified = true;
                break;
               }
           }
        if(!verified)
           {
            cout << "\nThe Rank You Guess Must Be In Your Hand!\n";
            cout << "Ask for a rank/number (A-10, J, K, Q): ";

            //clear inMove
            inMove = "";
           }
       }

    //handing end of file and/or invalid input
    if(!verified)
       {
        //only way to get here is thru end of file
        newMove = NULL;
        return false;
       }
    else
       {
        *newMove = inMove;
        return true;
       }
   }

