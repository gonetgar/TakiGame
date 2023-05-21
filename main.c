#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define NO_COLOR 0
#define YELLOW 1
#define RED 2
#define BLUE 3
#define GREEN 4
#define MAX_SIZE_OF_COLOR 7
#define MAX_SIZE_OF_CARD_TYPE 7

#define PLUS_CARD 10
#define STOP_CARD 11
#define CHANGE_DIRECTION 12
#define TAKI_CARD 13
#define CHANGE_COLOR 14

#define TOTAL_NUM_OF_CARDS 14
#define TAKE_CARD_FROM_PACK 0
#define END_TAKI_CARD_TURN 0
#define NO_MORE_CARDS 0
#define MIN_NUM_OF_CARDS 4
#define MAX_NAME 21 // no spaces and the last one is '\0'
#define MAX_LETTERS_ON_CARD 5
#define WINNER -1

typedef struct card
{
	int cardType;
	int color;
} Card;

typedef struct player
{
	char name[MAX_NAME]; // name
	Card* currentHand; // array of cards
	int logSize; // number of cards the player has in every part of the game
	int phySize; // physical size of the cards array
} Player;

typedef struct statistics
{
	int cardNum;
	int freq;
} Stats;

void initStatsArray(Stats arr[], int size);
int askHowManyPlayers();
void askNames(Player* playersArray, int numOfPlayers);
Card getFirstCardOnTheTable();
Card getCard();
void initGame(Player* playersArray, int numOfPlayers, Card* cardOnTheTable, Stats statisticArr[]);
char* changeCardTypeToString(Card cardToPrint);
char changeColorToChar(Card cardToPrint);
void printCardShape(Card cardToPrint);
void printCardsFunction(Player* playersArray, Card* cardOnTheTable, int* currentTurn, int* numOfCards);
int askCardFromUser(Player* playersArray, int* currentTurn);
void takeCardFromTheMiddle(Player* playersArray, int* currentTurn, Stats statisticArr[]);
Card* reallocateMoreCards(Card* currentHand, int previousSize, int newSize);
void swapCard(Player* playersArray, int* currentTurn, int playersChoice, int* numOfCards);
void changeColorOption(Player* playersArray, int* currentTurn, int playersChoice, Card* cardOnTheTable);
int askForNewCard(Player* playersArray, int* currentTurn, Card* cardOnTheTable);
void takiCardFunction(Player* playersArray, int* currentTurn, Card* cardOnTheTable, int colorOfTheCurrentTakiCard);
Player* changeDirectionFunction(Player* playersArray, int numOfPlayers, int* currentTurn);
bool checkCardRange(Player* playersArray, int* currentTurn, int playersChoice);
bool checkCardMatch(Player* playersArray, int* currentTurn, int playersChoice, Card* cardOnTheTable);
void changeCardOnTheTable(Player* playersArray, int* currentTurn, Card* cardOnTheTable, Card currentCard, int playersChoice);
int gameCourse(Player* playersArray, int* currentTurn, int numOfPlayers, Card* cardOnTheTable, Stats statisticArr[]);
void finishGameAndClearArray(Player* playersArray, int numOfPlayers);
void mergeSort(Stats arr[], int size);
void merge(Stats arr1[], int size1, Stats arr2[], int size2, Stats res[]);
void copyArr(Stats destiny[], Stats source[], int size);
void printStatistics(Stats arr[]);

void main()
{
	Player* playersArray = NULL; // array of all the players currently in the game.
	Card cardOnTheTable;
	Stats statisticArr[TOTAL_NUM_OF_CARDS];

	int numOfPlayers, currentTurn = 0;

	printf("************  Welcome to TAKI game !!! ***********\n");
	initStatsArray(statisticArr, TOTAL_NUM_OF_CARDS);

	numOfPlayers = askHowManyPlayers();
	playersArray = (Player*)malloc(sizeof(Player) * numOfPlayers);
	if (playersArray == NULL)
	{
		printf("Error! Memory allocation failed.\n");
		exit(1);
	}

	askNames(playersArray, numOfPlayers);
	srand(time(0));
	initGame(playersArray, numOfPlayers, &cardOnTheTable, statisticArr);

	// here the game takes place, everytime the current turn is the number of players, it will reset it to 0, until we find the winner
	while (currentTurn != WINNER)
	{
		while (currentTurn < numOfPlayers)
		{
			currentTurn = gameCourse(playersArray, &currentTurn, numOfPlayers, &cardOnTheTable, statisticArr);

			if (currentTurn == WINNER)
			{
				finishGameAndClearArray(playersArray, numOfPlayers);
				printStatistics(statisticArr);
				return;
			}
		}

		if (currentTurn != WINNER)
		{
			currentTurn = 0;
		}
	}
}

// initiallize the statistics array. putting the cards numbers and starting their frequencies to 0.
void initStatsArray(Stats arr[], int size)
{
	int i;

	for (i = 0; i < size; i++)
	{
		arr[i].cardNum = i + 1;
		arr[i].freq = 0;
	}
}

// asks how many players to start the game
int askHowManyPlayers()
{
	int res;

	printf("Please enter the number of players: \n");
	scanf("%d", &res);

	return res;
}

// fills the name field in the players array
void askNames(Player* playersArray, int numOfPlayers)
{
	int i;

	for (i = 0; i < numOfPlayers; i++)
	{
		printf("Please enter the first name of player #%d:\n", i + 1);
		scanf("%s", playersArray[i].name);
	}
}

// deals the first card on the table. it can only be a regular card.
Card getFirstCardOnTheTable()
{
	Card res;

	res.cardType = 1 + rand() % (TOTAL_NUM_OF_CARDS - 4); // exclude 0. also the top card receive only a regular number
	if (res.cardType == CHANGE_COLOR)
	{
		res.color = NO_COLOR;

	}
	else
	{
		res.color = rand() % GREEN + 1; // GREEN = 4 => number of colors possible
	}

	return res;
}

// deals a random card. returns a Card type.
Card getCard()
{
	Card res;

	res.cardType = 1 + rand() % TOTAL_NUM_OF_CARDS; // from 1 to 14

	if (res.cardType == CHANGE_COLOR)
	{
		res.color = NO_COLOR;
	}
	else
	{
		res.color = rand() % GREEN + 1; // GREEN = number of colors possible
	}

	return res;
}

// input cards to each players and to the table at the beggining of the game.
void initGame(Player* playersArray, int numOfPlayers, Card* cardOnTheTable, Stats statisticArr[])
{
	int i, j;
	*cardOnTheTable = getFirstCardOnTheTable();
	statisticArr[cardOnTheTable->cardType - 1].freq++;

	for (i = 0; i < numOfPlayers; i++) // deal the players random cards
	{
		j = 0;

		playersArray[i].currentHand = (Card*)malloc(sizeof(Card) * MIN_NUM_OF_CARDS);
		if (playersArray[i].currentHand == NULL)
		{
			printf("Error! Memory allocation failed.\n");
			exit(1);
		}

		playersArray[i].phySize = MIN_NUM_OF_CARDS;
		playersArray[i].logSize = MIN_NUM_OF_CARDS;

		for (j = 0; j < MIN_NUM_OF_CARDS; j++)
		{
			playersArray[i].currentHand[j] = getCard();
			statisticArr[playersArray[i].currentHand[j].cardType - 1].freq++; // update statistics array
		}
	}
}

// this function takes the number of the color and converts it into a character
char changeColorToChar(Card cardToPrint)
{
	char colorLetter;

	switch (cardToPrint.color)
	{
	case(YELLOW):
	{
		colorLetter = 'Y';
		break;
	}
	case(RED):
	{
		colorLetter = 'R';
		break;
	}
	case(BLUE):
	{
		colorLetter = 'B';
		break;
	}
	case(GREEN):
	{
		colorLetter = 'G';
		break;
	}
	case(NO_COLOR):
	{
		colorLetter = ' ';
		break;
	}
	}

	return colorLetter;
}

// this function changes the number of the card type into a string
char* changeCardTypeToString(Card cardToPrint)
{
	char tempStr[2];
	char* cardTypeStr = (char*)malloc(sizeof(char) * MAX_SIZE_OF_CARD_TYPE);
	if (cardTypeStr == NULL)
	{
		printf("Error! Memory allocation failed.\n");
		exit(1);
	}

	if (cardToPrint.cardType > TAKE_CARD_FROM_PACK && cardToPrint.cardType < PLUS_CARD) // regular card
	{
		strcpy(cardTypeStr, "   ");
		tempStr[0] = cardToPrint.cardType + '0'; // convert the number into a char
		tempStr[1] = '\0';
		strcat(cardTypeStr, tempStr);
		strcat(cardTypeStr, "   ");
	}
	else // special card
	{
		switch (cardToPrint.cardType)
		{
		case(PLUS_CARD):
		{
			strcpy(cardTypeStr, "   +   ");
			break;
		}
		case(STOP_CARD):
		{
			strcpy(cardTypeStr, "  STOP ");
			break;
		}
		case(CHANGE_DIRECTION):
		{
			strcpy(cardTypeStr, "  <->  ");
			break;
		}
		case(TAKI_CARD):
		{
			strcpy(cardTypeStr, "  TAKI ");
			break;
		}
		case(CHANGE_COLOR):
		{
			strcpy(cardTypeStr, " COLOR ");
			break;
		}
		}
	}

	return cardTypeStr;
}

// prints the card on the table
void printCardShape(Card cardToPrint)
{
	char* cardTypeStr;
	char cardColorLetter;

	cardTypeStr = changeCardTypeToString(cardToPrint);
	cardColorLetter = changeColorToChar(cardToPrint);

	switch (cardColorLetter) // set the printing into colors
	{
	case('Y'):
	{
		printf("\033[0;33m");
		break;
	}
	case('R'):
	{
		printf("\033[0;31m");
		break;
	}
	case('B'):
	{
		printf("\033[0;34m");
		break;
	}
	case('G'):
	{
		printf("\033[0;32m");
		break;
	}
	default:
		break;
	}

	printf("*********\n");
	printf("*       *\n");
	printf("*%s*\n", cardTypeStr);
	printf("*%4c   *\n", cardColorLetter);
	printf("*       *\n");
	printf("*********\n");
	printf("\033[0m"); // reset colors
}

// this function prints the upper card, and then the whole hand of the current player
void printCardsFunction(Player* playersArray, Card* cardOnTheTable, int* currentTurn, int* numOfCards)
{
	int i;

	printf("\nUpper card:\n");
	printCardShape(*cardOnTheTable);

	printf("\n%s's turn:\n", playersArray[*currentTurn].name);
	for (i = 0; i < *numOfCards; i++)
	{
		printf("\nCard #%d:\n", i + 1);
		printCardShape(playersArray[*currentTurn].currentHand[i]);
	}
}

// returns INDEX of the current hand array
int askCardFromUser(Player* playersArray, int* currentTurn)
{
	int numOfCards = playersArray[*currentTurn].logSize;
	int playersChoice;

	printf("or 1-%d if you want to put one of your cards in the middle:\n", numOfCards);
	scanf("%d", &playersChoice); // chose an INDEX from the player's array of currentHand

	return playersChoice;
}

// this function is used once the player decides to take a card from the middle.
// it checks to see if there is enough space in his array for more cards. if not, it reallocates more space.
// it deals the player a random card and then update the playersArray and statistics.
void takeCardFromTheMiddle(Player* playersArray, int* currentTurn, Stats statisticArr[])
{
	int logSize = playersArray[*currentTurn].logSize; // easier to read
	int phySize = playersArray[*currentTurn].phySize; // easier to read
	Card cardGivenToUser;

	if (logSize == phySize) // if there is no space for more cards
	{
		(playersArray[*currentTurn].phySize) *= 2;
		phySize *= 2;
		playersArray[*currentTurn].currentHand = reallocateMoreCards(playersArray[*currentTurn].currentHand, logSize, phySize); // allocate more space to cards
	}

	playersArray[*currentTurn].currentHand[logSize] = getCard();		// give the user a random card from the middle
	(playersArray[*currentTurn].logSize)++;								// update current size of array
	cardGivenToUser = playersArray[*currentTurn].currentHand[logSize]; // easier to read
	statisticArr[cardGivenToUser.cardType].freq++;						// update statistics
}

// if the current hand array ran out of space, it allocates an array twice its size and copies it all into it.
// then it frees the previous array and returns the new bigger array.
Card* reallocateMoreCards(Card* currentHand, int previousSize, int newSize)
{
	int i;
	Card* newArr = (Card*)malloc(newSize * sizeof(Card)); // the new array we will copy everything into.

	if (newArr == NULL)
	{
		printf("Error! Memory allocation failed.\n");
		exit(1);
	}
	else
	{
		for (i = 0; i < newSize; i++) // copying the old array to the new array
		{
			newArr[i] = currentHand[i];
		}
	}

	free(currentHand); // free the OLD array

	return newArr;
}

// if the player put a card on the table, it will swap between the card chosen and the last card on the deck, and cut down the number of cards.
// if he put his last card it will only cut down the number of cards.
void swapCard(Player* playersArray, int* currentTurn, int playersChoice, int* numOfCards)
{
	if (playersChoice != *numOfCards) // if it's NOT the last card
	{
		// swap between what the player chose, to the LAST card on the array.
		playersArray[*currentTurn].currentHand[playersChoice - 1].cardType = playersArray[*currentTurn].currentHand[(*numOfCards) - 1].cardType;
		playersArray[*currentTurn].currentHand[playersChoice - 1].color = playersArray[*currentTurn].currentHand[(*numOfCards) - 1].color;
	}

	// if logSize == 1, the player put his last card, and finished his cards.
	(*numOfCards)--; // cut down the number of cards
}

// if player chose the "change cholor" card, it will ask them which color they choose, and then change the card on the table
void changeColorOption(Player* playersArray, int* currentTurn, int playersChoice, Card* cardOnTheTable)
{
	int colorChoice;

	printf("Please enter your color choice:\n1 - Yellow\n2 - Red\n3 - Blue\n4 - Green\n");
	scanf("%d", &colorChoice);

	cardOnTheTable->cardType = CHANGE_COLOR;
	cardOnTheTable->color = colorChoice;
}

// this function asks for a new card from the player, for the taki card function
int askForNewCard(Player* playersArray, int* currentTurn, Card* cardOnTheTable)
{
	int playersChoice;

	printCardsFunction(playersArray, cardOnTheTable, currentTurn, &(playersArray[*currentTurn].logSize));

	printf("\nPlease enter %d if you want to finish your turn\n", TAKE_CARD_FROM_PACK);
	playersChoice = askCardFromUser(playersArray, currentTurn); // chooses a card

	return playersChoice;
}

// this function sets the Taki card action.
// it will continue asking the player for more cards of the same color until he input '0'.
// if the card is not in the right color it will print the "wrong card" message, and will ask them again and again until they choose a correct card, or 0 to finish their turn.
void takiCardFunction(Player* playersArray, int* currentTurn, Card* cardOnTheTable, int colorOfTheCurrentTakiCard)
{
	Card currentCard;
	int playersChoice;

	playersChoice = askForNewCard(playersArray, currentTurn, cardOnTheTable);

	while (playersChoice != END_TAKI_CARD_TURN)
	{
		currentCard.cardType = playersArray[*currentTurn].currentHand[playersChoice - 1].cardType;
		currentCard.color = playersArray[*currentTurn].currentHand[playersChoice - 1].color;

		while (currentCard.color != colorOfTheCurrentTakiCard && playersChoice != END_TAKI_CARD_TURN)
		{
			printf("Invalid card! Try again.\n");
			printf("Please enter %d if you want to finish your turn\n", END_TAKI_CARD_TURN);
			playersChoice = askCardFromUser(playersArray, currentTurn); // chooses a card
			currentCard.cardType = playersArray[*currentTurn].currentHand[playersChoice - 1].cardType;
			currentCard.color = playersArray[*currentTurn].currentHand[playersChoice - 1].color;
		}

		// if he got to here, means he put a right card, so we will change the card on the table.
		cardOnTheTable->cardType = currentCard.cardType;
		cardOnTheTable->color = currentCard.color;

		// swap between the chosen card to the last card, and cut down the logSize
		swapCard(playersArray, currentTurn, playersChoice, &(playersArray[*currentTurn].logSize));
		playersChoice = askForNewCard(playersArray, currentTurn, cardOnTheTable);

		if (playersArray[*currentTurn].logSize == NO_MORE_CARDS)
		{
			return;
		}
	}

	if (playersChoice == END_TAKI_CARD_TURN) // if he entered 0 we exit from the function
	{
		return;
	}
}

// this function swaps all players places in the array.
// then we lower the current turn by one, so that the game will continue starting from the previous person.
Player* changeDirectionFunction(Player* playersArray, int numOfPlayers, int* currentTurn)
{
	int i, endOfPlayersArray = numOfPlayers;
	Player* newPlayersArray = (Player*)malloc(sizeof(Player) * numOfPlayers);

	if (newPlayersArray == NULL)
	{
		printf("Error! Memory allocation failed.\n");
		exit(1);
	}

	for (i = 0; i < endOfPlayersArray; i++, numOfPlayers--)
	{
		newPlayersArray[i] = playersArray[numOfPlayers - 1];
	}

	(*currentTurn)--;

	return newPlayersArray;
}

// this function checks if the chosen card is in the right range of the player's hand
bool checkCardRange(Player* playersArray, int* currentTurn, int playersChoice)
{
	if (playersChoice < TAKE_CARD_FROM_PACK || playersChoice > playersArray[*currentTurn].logSize) // card doesn't exist
	{
		printf("Invalid choice! Try again.\n");
		return false;
	}
	else
	{
		return true;
	}
}

// this function checks if the chosen card is a match to the card on the table
bool checkCardMatch(Player* playersArray, int* currentTurn, int playersChoice, Card* cardOnTheTable)
{
	Card chosenCard;

	chosenCard.cardType = playersArray[*currentTurn].currentHand[playersChoice - 1].cardType;
	chosenCard.color = playersArray[*currentTurn].currentHand[playersChoice - 1].color;

	if (chosenCard.cardType != cardOnTheTable->cardType && chosenCard.color != cardOnTheTable->color) // wrong card
	{
		if (chosenCard.cardType == CHANGE_COLOR) // can be placed on any card at any time
		{
			return true;
		}
		else
		{
			printf("Invalid card! Try again.\n");
			return false;
		}
	}
	else
	{
		return true;
	}
}

// changes the upper card and swaps between the chosen card to the last card on the deck.
void changeCardOnTheTable(Player* playersArray, int* currentTurn, Card* cardOnTheTable, Card currentCard, int playersChoice)
{
	cardOnTheTable->cardType = currentCard.cardType;
	cardOnTheTable->color = currentCard.color;

	swapCard(playersArray, currentTurn, playersChoice, &(playersArray[*currentTurn].logSize));
}

// this function controls the entire game. here the player chooses all of his moves
// update CardOnTheTable, returns currentTurn
// if one of the playes finishes his cards, we will return '-1' as his turn number, and then the main will know to finish the game and make him the winner.
int gameCourse(Player* playersArray, int* currentTurn, int numOfPlayers, Card* cardOnTheTable, Stats statisticArr[])
{
	Card currentCard;
	int playersChoice, curTurn = *currentTurn;

	printCardsFunction(playersArray, cardOnTheTable, currentTurn, &(playersArray[*currentTurn].logSize));
	printf("\nPlease enter %d if you want to take a card from the deck\n", TAKE_CARD_FROM_PACK);
	playersChoice = askCardFromUser(playersArray, currentTurn);

	while ((checkCardRange(playersArray, currentTurn, playersChoice) == false || checkCardMatch(playersArray, currentTurn, playersChoice, cardOnTheTable) == false) && playersChoice != TAKE_CARD_FROM_PACK)
	{
		printf("\nPlease enter %d if you want to take a card from the deck\n", TAKE_CARD_FROM_PACK);
		playersChoice = askCardFromUser(playersArray, currentTurn);
	}

	if (playersChoice == TAKE_CARD_FROM_PACK)
	{
		takeCardFromTheMiddle(playersArray, currentTurn, statisticArr);// ==> go to the next turn.
		curTurn++;
	}
	else
	{
		currentCard.cardType = playersArray[*currentTurn].currentHand[playersChoice - 1].cardType;
		currentCard.color = playersArray[*currentTurn].currentHand[playersChoice - 1].color;

		switch (currentCard.cardType)
		{
		case(CHANGE_COLOR):
		{
			changeColorOption(playersArray, currentTurn, playersChoice, cardOnTheTable);
			swapCard(playersArray, currentTurn, playersChoice, &(playersArray[*currentTurn].logSize));
			break;
		}
		case(TAKI_CARD):
		{
			changeCardOnTheTable(playersArray, currentTurn, cardOnTheTable, currentCard, playersChoice);

			takiCardFunction(playersArray, currentTurn, cardOnTheTable, currentCard.color);
			break;
		}
		case(PLUS_CARD):
		{
			changeCardOnTheTable(playersArray, currentTurn, cardOnTheTable, currentCard, playersChoice);

			if (playersArray[*currentTurn].logSize == NO_MORE_CARDS) // if it was their last card
			{
				playersArray[*currentTurn].currentHand[0] = getCard(); // automatically give them another card
				(playersArray[*currentTurn].logSize)++;
			}

			curTurn--; // give the user another turn
			break;
		}
		case(STOP_CARD):
		{
			changeCardOnTheTable(playersArray, currentTurn, cardOnTheTable, currentCard, playersChoice);

			if (playersArray[*currentTurn].logSize == NO_MORE_CARDS && numOfPlayers == 2) // if it was their last card
			{
				playersArray[*currentTurn].currentHand[0] = getCard(); // give him another card. the array is minimum 4 cards, so we don't need to check if the array has enough space.
				(playersArray[*currentTurn].logSize)++;
			}

			if (*currentTurn == numOfPlayers - 1) // if it's the last player on the list
			{ // the first player in the array loses his turn, and we automatically move to the second player
				curTurn = 0;
			}
			else // we skip on the next player
			{
				curTurn++;
			}

			break;
		}
		case(CHANGE_DIRECTION):
		{
			changeCardOnTheTable(playersArray, currentTurn, cardOnTheTable, currentCard, playersChoice);

			playersArray = changeDirectionFunction(playersArray, numOfPlayers, currentTurn);
			curTurn--;

			break;
		}
		default:
		{
			changeCardOnTheTable(playersArray, currentTurn, cardOnTheTable, currentCard, playersChoice);
			break;
		}
		}

		curTurn++;
	}

	if (playersArray[*currentTurn].logSize == NO_MORE_CARDS) // if someone finished his cards ==> won
	{
		printf("The winner is... %s! Congratulations!\n", playersArray[*currentTurn].name);
		return WINNER;
	}
	else
	{
		return curTurn;
	}
}

// END GAME //

// once the game ends we will free all allocated memory of playersArray.
void finishGameAndClearArray(Player* playersArray, int numOfPlayers)
{
	int i;

	for (i = 0; i < numOfPlayers; i++)
	{
		free(playersArray[i].currentHand);
	}

	free(playersArray);
}

// this function sorts the statistics array by the frequency field.
// in every call we split the array by two halves (not necessarily two equals halves) until we reached an array of on item (it's already sorted).
// in the following calls we merges the two arrays back together, but in a sorted way.
// the sorting only reffers to the frequency field, the cardType field does not change.
void mergeSort(Stats arr[], int size)
{
	Stats* tempArr;

	if (size <= 1)
	{
		return;
	}
	else
	{
		mergeSort(arr, size / 2);
		mergeSort(arr + size / 2, size - size / 2);

		tempArr = (Stats*)malloc(sizeof(Stats) * size);
		if (tempArr == NULL)
		{
			printf("Error! Memory allocation failed.\n");
			exit(1);
		}

		merge(arr, size / 2, arr + size / 2, size - size / 2, tempArr);
		copyArr(arr, tempArr, size);
		free(tempArr);
	}
}

// merges two Stats arrays into one.
void merge(Stats arr1[], int size1, Stats arr2[], int size2, Stats res[])
{
	int i1 = 0, i2 = 0, resI = 0;

	while (i1 < size1 && i2 < size2)
	{
		if (arr1[i1].freq >= arr2[i2].freq)
		{
			res[resI].freq = arr1[i1].freq;
			res[resI].cardNum = arr1[i1].cardNum;
			i1++;
		}
		else
		{
			res[resI].freq = arr2[i2].freq;
			res[resI].cardNum = arr2[i2].cardNum;
			i2++;
		}
		resI++;
	}
	while (i1 < size1)
	{
		res[resI].freq = arr1[i1].freq;
		res[resI].cardNum = arr1[i1].cardNum;
		i1++;
		resI++;
	}
	while (i2 < size2)
	{
		res[resI].freq = arr2[i2].freq;
		res[resI].cardNum = arr2[i2].cardNum;
		i2++;
		resI++;
	}
}

// this function copies the source array into the destiny array.
void copyArr(Stats destiny[], Stats source[], int size)
{
	int i;

	for (i = 0; i < size; i++)
	{
		destiny[i].freq = source[i].freq;
		destiny[i].cardNum = source[i].cardNum;
	}
}

// this function prints the sorted statistics array.
void printStatistics(Stats arr[])
{
	int i;

	mergeSort(arr, TOTAL_NUM_OF_CARDS);

	printf("\n************ Game Statistics ************\n"); // check for statistics
	printf("Card # | Frequency\n__________________\n");
	for (i = 0; i < TOTAL_NUM_OF_CARDS; i++)
	{
		if (arr[i].cardNum < 10)
		{
			printf("   %d   |   %d   \n", arr[i].cardNum, arr[i].freq);
		}
		else
		{
			switch (arr[i].cardNum)
			{
			case(PLUS_CARD):
			{
				printf("   +   |   %d   \n", arr[i].freq);
				break;
			}
			case(STOP_CARD):
			{
				printf(" STOP  |   %d   \n", arr[i].freq);
				break;
			}
			case(CHANGE_DIRECTION):
			{
				printf("  <->  |   %d   \n", arr[i].freq);
				break;
			}
			case(TAKI_CARD):
			{
				printf(" TAKI  |   %d   \n", arr[i].freq);
				break;
			}
			case(CHANGE_COLOR):
			{
				printf(" COLOR |   %d   \n", arr[i].freq);
				break;
			}
			}
		}
	}
}