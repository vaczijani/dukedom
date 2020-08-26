// DUKEDOM: text based land management game
// by
// Rick Merrill and David Ahl
// Lee Schneider, Todd Voros, Vince Talbot, Jamie Hanrahan, David Barber and Richard Kaapke
// Converted from Creative Computing Press: Big Computer Games 1984, Edited by David H. Ahl
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifndef MIN
 #define MIN(a,b) (a<b)?(a):(b)
#endif

#ifndef MAX
 #define MAX(a,b) (a>b)?(a):(b)
#endif

#ifdef SHOW_CALCULATIONS
 #define dbgprintf(...) printf(__VA_ARGS__)
#define dbgland_values(x) land_values(x)
#else
 #define dbgprintf(...)
 #define dbgland_values(x)
#endif

// Replicating BASIC RND() function
// RND(0): (0..1)
// RND(1): [0..1] not sure?!
// RND(n): [1..n]
float RND(int d) {
	if (d == 0 || d == 1)
		return (float)rand() / ((float)RAND_MAX + 1);
	else
		return (float)((rand() % d) + 1);
}

// [Q1..Q2]
int FNR(int Q1, int Q2) {
	int ret = (int)(RND(1) * (1 + Q2 - Q1) + Q1);
	dbgprintf("%40s", "");
	dbgprintf("FNR(%d, %d)=%d\n", Q1, Q2, ret);
	return ret;
}

int R[9];
int F3 = 2;  // looks constant
int FNX(int Q1) {
	// FNR(-F3, F3) = [-2..2]
	int ret = FNR(-F3, F3) + R[Q1];
	dbgprintf("%40s", "");
	dbgprintf("FNX(%d)=%d\n", Q1, ret);
	return ret;
}

int Q1 = 1;
int Q2;
int Q3;
int GO;

int gauss() {
	Q3 = FNR(Q1, Q2);
	Q3 = FNR(Q1, Q2);
	if (FNR(Q1, Q2) > 5)
		GO = (Q3 + FNR(Q1, Q2)) / 2;
	else
		GO = Q3;
	return GO;
}

void init_random_table() {
	Q1 = 4;
	Q2 = 7;
	// [4..7]
	R[1] = gauss();
	Q2 = 8;
	// [4..8]
	//R[3] = gauss(); // ??? more likely that this should go to R[2]
	R[2] = gauss();
	Q2 = 6;
	// [4..6]
	R[3] = gauss();
	Q1 = 3;
	// [3..6]
	R[4] = gauss();
	Q1 = 5;
	// [5..6]
	R[5] = gauss();
	Q1 = 3;
	Q2 = 6;
	// [3..6]
	R[6] = gauss();
	Q2 = 8;
	// [3..8]
	R[7] = gauss();
	Q1 = 1;
	// [1..8]
	R[8] = gauss();
	dbgprintf("%40s", "");
	dbgprintf("Random base table:[");
	for (int i = 1; i <= 8; ++i)
		dbgprintf("%d ", R[i]);
	dbgprintf("]\n");
}

float M = 1.95f;	// mercenary attack power?
int Y = 0;			// year
float C;			// crop yield
float C1 = 3.95f;	// last year's yield
float U1 = 0;		// unrest during the year. resets every year.
float U2 = 0;		// cumulative unrest. moves to next year.
int K = 0;			// king state 0=neutral -1=hiring -2=levy 1=in fear 2=preparing for war
                    // taking a neighboring dukedom makes the king fear you
					// king in fear starts provoking
					// you can calm the provoking king back to hiring
					// king hiring steps up to asking for levy
float D = 0;		// Plague interval?
// THE RESOURCES: land, grain, peasants
int P = 100;		// peasants
int L = 600;		// land
int G = 4177;		// grain

int PA[9] = {0, 96, 0, 0, 0, 0, 0, -4, 8};							// peasant changes
int LA[4] = {0, 600, 0, 0};											// land changes
int GA[11] = {0, 5193, -1344, 0, -768, 0, 0, 0, 1516, -120, -300};	// grain changes
int SA[7] = {0, 216, 200, 184, 0, 0, 0};							// land quality: 100%, 80%, ...
int U[7] = {0,0,0,0,0,0,0};											// usable land for production per quality: 100%, 80%, ...

const char* PS[9] = {"", "Peasants at start", "Starvations", "King's Levy", 
	"War casualties", "Looting victims", "Disease victims", 
	"Natural deaths", "Births"};

const char* LS[4] = {"", "Land at start", "Bought/sold", "Fruits of war"};

const char* GS[11] = {"", "Grain at start", "Used for food", "Land deals", 
	"Seeding", "Rat losses", "Mercenary hire", 
	"Fruits of war", "Crop yield", "Castle expense", 
	"Royal tax"};

const char* SS[7] = {"", "100%", "80%", "60%", "40%", "20%", "Depl"};

// Xn are used for local calculations?
float X1;
float X2;
float X3;
float X4;
float X5;
float X6;

void print_tab(const char* text, int tab, int value) {
	printf("%s%*s%d\n", text, tab-strlen(text), "", value);
}

void land_values(int l[]) {
	for (int j = 1; j <= 6; ++j) printf("%5s", SS[j]);
	printf("\n");
	for (int j = 1; j <= 6; ++j) printf("%5d", l[j]);
	printf("\n");
}

const char* king_status(int stat) {
	switch (stat) {
	case -2: return "asking for levy";
	case -1: return "hiring, raising army";
	case 0: return "neutral";
	case 1: return "in fear";
	case 2: return "seeking war";
	}
	return "INVALID!";
}

void credit() {
	printf("D U K E D O M\n");
	printf("Created by Rick Merrill and David Ahl\n");
	printf("Credit to Lee Schneider\n"
		   "          Todd Voros\n"
		   "          Vince Talbot\n"
		   "          Jamie Hanrahan\n"
		   "          David Barber\n"
		   "      and Richard Kaapke\n\n");
	// I've played it enough that I should be able to give some instructions
	printf(
		"Game info:\n"
		"Your task is resource management:\n"
		"Feed the peasants: each peasant need 13 hL\n"
		"grain for survival.\n"
		"You can overfeed them up to 18 hL grain to\n"
		"make them happy and lower unrest amongst\n"
		"them.\n"
		"If unrest grows high you lose your dukedom!\n"
		"Also, happy peasants fight better.\n"
		"Peasant are needed to tend the land: you\n"
		"need one peasant for every 4 ha land to\n"
		"produce crops.\n\n"
		"Trade land: you need land to produce grain.\n"
		"Grain is your currency!\n"
		"You can buy as much land as you can afford.\n"
		"You can only sell your good quality lands\n"
		"with at least 60%% fertility up to 4000 hL\n"
		"of grain value! You cannot sell more than\n"
		"4000 hL worth!\n\n"
		"Plant crops: grain is your currency! You\n"
		"need to produce as much as you can. The\n"
		"amount of land you can tend depends on how\n"
		"much grain you have: each ha needs 2 hL\n"
		"grain to be planted, and how many peasants\n"
		"you have: 4 ha land needs one peasant worker.\n"
		"The lands have different fertility rate:\n"
		"0 to 100%. Used land gets depleted and rested\n"
		"land gets more fertile.\n\n"
		"To win either:\n"
		"Serve 45 years without major conflict, King's\n"
		"status must be neutral\n"
		"Beat the King's army after winning a\n"
		"neighboring dukedom\n\n"
		"So, you will need to wage wars sometimes: in\n"
		"years with poor crop yield a neighboring duke\n"
		"may threaten you with war. You can either\n"
		"strike first or wait for them to attack.\n"
		"Sitting duck is cheap but you will surely lose\n"
		"some peasants. Striking first is expensive but\n"
		"spares your peasants.\n"
		"To successfully wage war keep your peasants\n"
		"happy and get rich to be able to hire\n"
		"mercenaries for 40 hL grain each.\n"
		);
}

void report() {
	// 720
	printf("\nYear %d Peasants %d Land %d Grain %d\n\n", Y, P, L, G);
	for (int j = 1; j <= 8; ++j) 
		if (PA[j] != 0 || j == 1) print_tab(PS[j], 25, PA[j]);
	print_tab("Peasants at end", 25, P);
	for (int j = 1; j <= 3; ++j)
		if (LA[j] != 0 || j == 1) print_tab(LS[j], 25, LA[j]);
	print_tab("Land at end of year", 25, L);
	printf("\n");
	land_values(SA);
	printf("\n");
	for (int j = 1; j <= 10; ++j)
		if (GA[j] != 0 || j == 1) print_tab(GS[j], 25, GA[j]);
	print_tab("Grain at end of year", 25, G);
	if (Y <= 0) printf("(Severe crop damage due to seven\n year locusts)\n");
	printf("\n");
	dbgprintf("King's status: %s\n", king_status(K));
	++Y;  // move to next year
	// initalise resource dealings
	for (int i = 1; i <= 8; ++i) PA[i] = 0;
	for (int i = 1; i <= 3; ++i) LA[i] = 0;
	for (int i = 1; i <= 10; ++i) GA[i] = 0;
	PA[1] = P;
	LA[1] = L;
	GA[1] = G;
}

void test_end_game() {
	// 890
	if (P < 33) {
		printf("You have so few peasants left that\n"
				"the High King has abolished your Ducal\n"
				"right\n");
		exit(1);
	}
	if (L < 199) {
		printf("You have so little land left that\n"
				"The peasants tired of war and starvation\n"
				"You are deposed\n");
		exit(1);
	}
	if (U1 > 88 || U2 > 99 || G < 429) {
		// 930
		printf("The peasants tired of war and starvation\n"
				"You are deposed\n");
		exit(1);
	}
	if (Y > 45 && K == 0) {
		printf("You have reached the age of retirement\n");
		exit(0);
	}
	U1 = 0;
}

char getYesOrNo() {
	char inStr[2];
	char c;
	do {
		fgets(inStr, 2, stdin);
		c = tolower(inStr[0]);
	} while (c != 'y' && c != 'n');
	return c;
}

void king_raising_tax() {
	// 960
	if (K > 0) {
		printf("The King demands twice the royal tax in\n"
			"THE HOPE TO PROVOKE WAR. WILL YOU PAY? y/n: ");
		char v = getYesOrNo();
		K = 2;
		if (v == 'y')
			K = -1;
		dbgprintf("King's status:%s\n", king_status(K));
	}
}

void not_enough_grain() {
	printf("But you don't have enough grain\n"
			"You have %d hL of grain left,\n", G);
	if (X1 >= 4)
		printf("Enough to buy %d ha of land\n", (int)(G/X1));
	else
		printf("Enough to plant %d ha of land\n", (int)(G/2));
}

void not_enough_land() {
	printf("But you don't have enough land\n"
			"You only have %d ha of land left\n", L);
}

void not_enough_peasant() {
	printf("But you don't have enough peasants\n"
			"Your peasants can only plant %d ha of land\n", 4 * P);
}

void feed() {
	do {
		printf("Grain for food =");
		dbgprintf("\n(less than 100 in per peasant)=");
		int v;
		scanf("%d", &v);
		v = (v >= 100) ? v : v * P;  // P = Peasants
		if (v > G) {
			not_enough_grain();
		} else {
			print_tab(GS[2], 25, v); // G[2]= "Used for food"
			if (v/P < 11 && v != G)
				printf("The peasants demonstrate before the\n"
					"castle\n");
			GA[2] = -v;  // "Used for food"
			G += GA[2];  // "Used for food"
			X1 = (float)v/P;
			if (X1 < 13) {
				printf("Some peasants have starved\n");
				PA[2] = -(int)(P - v/13); // "Starvations"
				print_tab(PS[2], 25, PA[2]); // "Starvations"
				P += PA[2]; // P[2]= "Starvations"
			}
			X1 -= 14;
			X1 = (X1 <= 4) ? -X1 : -4;
			int rest = (int)(3 * PA[2] - 2 * X1); // "Starvations"
			U1 -= rest;
			dbgprintf("Unrest changed by %d to %g\n", -rest, U1);
			break;
		}
	} while (true);
}

void trade() {
	float c = C1;
	X1 = (float)(int)(2 * c + FNX(1) - 5);
	X1 = (X1 < 30) ? X1 : 30;
	do {
		printf("Land to buy at %g hL/ha [0-%d]= ", X1, (int)(G/X1));
		int v;
		scanf("%d", &v);
		GA[3] = (int)(-v * X1);  // G[3]= "Land deals" negative=buying
		if (-GA[3] > G)
			not_enough_grain();
		else {
			LA[2] = v;  // L[2]= "Bought/sold"
			SA[3] += v; // S[3]= "60%" guality
			break;
		}
	} while (true);
	if (LA[2] > 0) {
		G += GA[3];
		L += LA[2];
		print_tab(LS[2], 25, LA[2]);
		print_tab(GS[3], 25, GA[3]);
		dbgland_values(SA);
		return;
	}
	// SELL
	X2 = (float)SA[1] + SA[2] + SA[3];  // size of good (>=60%) land
	printf("You can sell %g ha land\n", X2);
	// maximum of 3 rounds of "auction"
	printf("Sale has max of 3 rounds.\n"
		"Price drops by 1 each round!\n");
	for (int j = 1; j <= 3; ++j) {
		--X1;
		printf("Land to sell at %g hL/ha [0-%g]= ", X1, X2);
		int v;
		scanf("%d", &v);
		if (v > X2) 
			printf("But you only have %g ha of good land (yield >= 60%)\n", X2);
		else {
			if (v * X1 <= 4000) {
				GA[3] = (int)(v * X1); // G[3]= "Land deals"
				LA[2] = -v; // L[2]= "Bought/sold" negative=sold
				// sell 60% land first, then 80% and 100%
				for (int j = 3; j > 0; --j) {
					if (v <= SA[j]) {
						SA[j] -= v;
						break;
					} else {
						v -= SA[j];
						SA[j] = 0;
					}
				}
				L += LA[2];
				if (X1 < 4) {
					printf("The High King appropriates half\n"
							"of your earnings as punishment\n"
							"for selling at such a low price\n");
					GA[3] /= 2;
				}
				G += GA[3];
				print_tab(LS[2], 25, LA[2]);
				print_tab(GS[3], 25, GA[3]);
				return;
			} else
				printf("No buyers have that much grain try less (<4000)\n");
		}
	}
	printf("Buyers have lost interest\n");
}

void king_move() {
	// 1350
	if (K != 2) {
		// GOTO 1460
		return;
	}
	// 1360
	printf("The King's army is about to attack\n"
		"your duchy\n");
	X1 = (float)(int)(G / 100);
	printf("at 100 hL each (pay in advance)\n");
	printf("You have hired %g foreign mercenaries\n", X1);
	dbgprintf("%40s", "");
	dbgprintf("You: %g\n", 8 * X1 + P);
	dbgprintf("%40s", "");
	dbgprintf("Enemy: %d\n", 2399);
	if ((8 * X1 + P) > 2399) {
		// 1400
		printf("Wipe the blood from the crown - you\n"
			"are High King! A near monarchy\n"
			"THREATENS WAR; HOW MANY...\n");
		exit(0);
	} else {
		printf("Your head is placed atop of the\n"
			"castle gate.\n");
		exit(1);
	}
}

void plant() {
	do {
		int landmax = MIN(MIN(L, 4 * P), G/2);
		printf("Land to be planted [0-%d]= ", landmax);
		int v;
		scanf("%d", &v);
		if (v > L)
			not_enough_land();
		else if (2 * v > G)
			not_enough_grain();
		else if (4 * P < v)
			not_enough_peasant();
		else {
			GA[4] = -2 * v; // G[4]= "Seeding" negative: used, will need to subtract
			GA[8] = v; // G[8]= "Crop yield", now it's ha to be planted
			print_tab(GS[4], 25, GA[4]);
			G += GA[4];
			// collect usable crops for grain production
			for (int j = 1; j <= 6; ++j) U[j] = 0;
			for (int j = 1; j <= 6; ++j) {
				if (v <= SA[j]) {
					// 1530
					U[j] = v;
					SA[j] -= v;
					break;
				} else {
					v -= SA[j];
					U[j] = SA[j];
					SA[j] = 0;
				}
			}
			dbgprintf("seeded:\n");
			dbgland_values(U);
			break;
		}
	} while (true);
}

void rotate() {
	// 1530
	// rotate land?
	SA[1] += SA[2];
	SA[2] = 0;
	// 60%->100%, 40%->80%, 20%->60%, 0%->40% : unseeded land gets better!
	for (int j = 3; j <= 6; ++j) {
		if (SA[j] > 0)
			dbgprintf("%d ha %s becomes %s\n", SA[j], SS[j], SS[j-2]);
		SA[j-2] += SA[j];
		SA[j] = 0;
	}
	// seeded: 100%->80%, 80%->60% ... 20%->0% : seeded land gets worse!
	for (int j = 1; j <= 5; ++j) {
		if (U[j] > 0)
			dbgprintf("%d ha %s becomes %s!\n", U[j], SS[j], SS[j+1]);
		SA[j+1] += U[j];
	}
	if (U[6] > 0)
		dbgprintf("%d ha stays depleted!\n", U[6]);
	SA[6] += U[6]; // don't plant to depleted land!
	dbgprintf("rotated land:\n");
	dbgland_values(SA);
}

// simulation: no user input
void harvest() {
	C = (float)FNX(2) + 9;
	if (Y % 7 == 0) {
		printf("Seven year locusts\n"
			"destroyed 35%% of the yield\n");
		C = (float)(int)(C * 0.65);
	}
	X1 = 0;
	// yields: 100%, 80%, 60%...
	dbgprintf("Produced grain:\n");
	for (int j = 1; j <= 5; ++j) dbgprintf("%5s", SS[j]);
	dbgprintf("\n");
	for (int j = 1; j <= 5; ++j) {
		X1 += (float)U[j] * (1.2f - 0.2f * j);
		dbgprintf("%5d", (int)(U[j] * (1.2f - 0.2f * j)));
	}
	dbgprintf("\n");
	dbgprintf("100%% equivalent land: %g\n", X1);
	dbgprintf("Seeded land: %d\n", GA[8]);
	if (GA[8] == 0) {  // G[8]= "Crop yield"
		C1 = 0;
		C = 0;
	} else {
		C1 = (float)(int)(C * X1 / GA[8] * 100) / 100;
		C = C1;
	}
	printf("Yield = %g hL/ha\n", C);
	GA[8] = (int)(C * X1);
	G += GA[8];
}

void store() {
	X1 = (float)FNX(3) + 3;
	dbgprintf("%40s", "");
	dbgprintf("Avoids infestation if %g < 9\n", X1);
	if (X1 < 9) {
		dbgprintf("No infestation\n");
		return;
	}

	GA[5] = -(int)(X1 * G / 83); // G[5]= "Rat losses"
	G += GA[5];
	printf("Rats infest the granary\n");
	print_tab(GS[5], 25, GA[5]);
}

void king_levy() {
	if (P < 67 || K != -1) return;
	X1 = (float)FNX(4);
	dbgprintf("%40s", "");
	dbgprintf("No levy if %g > %g\n", X1, P/30);
	if (X1 > P/30) return;

	printf("The king requires %.0g peasants for\n"
		"his estates and mines. Will you supply\n"
		"for them (y)es or pay %d hL of\n"
		"grain instead (n)o ?");
	char v = getYesOrNo();
	if (v == 'n') {
		GA[10] = (int)(-100 * X1);  // G[10]= "Royal tax"
		G += GA[10];
		print_tab(GS[10], 25, GA[10]);
	} else {
		PA[3] = (int)(-X1); // P[3]= "King's levy"
		P += PA[3];
		printf(PS[3], 25, PA[3]);
	}
}

void war() {
	dbgprintf("Enemy offensive movements.\n"
		"King's status = %s\n", king_status(K));
	if (K != -1) {
		// 1780
		X1 = (float)(int)(11 - 1.5 * C);	// depends on crop yield? [-13..-1]
		dbgprintf("%40s", "");
		dbgprintf("War tension: %g\n", X1);
		//X1 = (X1 >= 2) ? -X1 : -2;
		X1 = (X1 < 2) ? 2 : X1;
		if (K != 0 || P <= 109 || 17 * (L - 400) + G <= 10600) {
			// 1820
			X2 = 0;
		} else {
			// 1800
			printf("The High King grows uneasy and may\n"
				"be subsidizing wars against you\n");
			X1 += 2;
			X2 = (float)Y + 5;
		}
		// 1830
		X3 = (float)FNX(5); // [4..7]
		dbgprintf("%40s", "");
		dbgprintf("Avoids war if %g > %g\n", X3, X1);
		if (X3 > X1) {
			// GOTO 2180
			dbgprintf("No war\n");
			return;
		}
		printf("A nearby duke threatens war; \n");
		X2 = (float)(int)(X2 + 85 + 18 * FNX(6)); // enemy's war potential?
		X4 = 1.2f - U1 / 16;
		dbgprintf("%40s", "");
		dbgprintf("Your peasants power: %g\n", X4);
		X5 = (float)(int)(P * X4) + 13;  // your war potential?
		dbgprintf("%40s", "");
		dbgprintf("You: %g\n", X5);
		printf("You have %d peasants and %d hL grain\n", P, G);
		printf("Will you attack first? ");
		char c = getYesOrNo();
		if (c == 'y') {
			// GOTO 1920
		} else {
			dbgprintf("%40s", "");
			dbgprintf("You / Enemy = %g / %g\n", X5, X2);
			if (X2 >= X5) {
				// 1890
				printf("First strike failed - you need\n"
					"professionals\n");
				PA[4] = (int)(-X3 - X1 - 2); // P[4]= "War casualties"
				X2 += 3.0f * PA[4];
			} else {
				printf("Peace negotiations successful\n");
				PA[4] = (int)(-X1 - 1);
				X2 = 0;				
				// GOTO 1910
			}
			print_tab(PS[4], 25, PA[4]);
			dbgprintf("Enemy = %g\n", X2);
			// 1910
			P += PA[4];
			if (X2 < 1) {
				U1 -= 2 * PA[4] + 3 * PA[5]; // P[5]= "Looting victims"
				print_tab(PS[5], 25, PA[5]);
				dbgprintf("Unrest: %g\n", U1);
				return;
			}
		}
		// 1920
		int v;
		do {
			printf("How many mercenaries will you hire\n"
				"at 40 hL each = ");
			scanf("%d", &v);
			if (v > 75) {
				printf("There are only 75 available for hire\n");
				v = -1;
			}
		} while (v < 0);
		X2 = (float)(int)(X2 * M);
		dbgprintf("%40s", "");
		dbgprintf("Enemy: %g\n", X2);
		X5 = (float)(int)(P * X4) + 7 * v + 13;
		dbgprintf("%40s", "");
		dbgprintf("You: %g\n", X5);
		X6 = X2 - 4 * v - (int)(0.25 * X5); // will be casualties
		X2 = X5 - X2;
		dbgprintf("%40s", "");
		dbgprintf("Your win>1: %g\n", X2);
		LA[3] = (int)(0.8 * X2);
		print_tab(LS[3], 25, LA[3]);
		if (-LA[3] <= 0.67 * L) {
			dbgprintf("%40s", "");
			dbgprintf("Victory: %d <= %g\n", -LA[3], 0.67 * L);
			X1 = (float)LA[3];
			for (int j = 1; j <= 3; ++j) {
				X3 = (float)(int)(X1 / (4 - j));
				if (-X3 <= SA[j])
					X5 = X3;
				else
					X5 = (float)-SA[j];
				dbgprintf("Taking %g ha of %s land\n", X5, SS[j]);
				SA[j] += (int)X5;
				X1 -= X5;
			}
			// 2000
			if (LA[3] < 399) {
				// GOTO 2050
				if (X2 < 1) {
					// GOTO 2070
					printf("You have lost the war\n");
					X4 = (float)GA[8] / L; // G[8]="Crop yield"
				} else {
					printf("You have won the war\n");
					X4 = 0.67f;
					GA[7] = (int)(1.7 * LA[3]); // G[7]="Fruits of war" L[3]="Fruits of war"
					print_tab(GS[7], 25, GA[7]);
					G += GA[7];
					// GOTO 2080
				}
				// 2080
				if (X6 <= 9)
					X6 = 0;
				else
					X6 = (float)(int)(X6 / 10);
			} else {
				printf("You have overrun the enemy and annexed\n"
					"his entire dukedom\n");
				GA[7] = 3513; // G[7] = "Fruits of war"
				print_tab(GS[7], 25, GA[7]);
				G += GA[7];
				X6 = -47;
				X4 = 0.55f;				
				if (K > 0) {
					// GOTO 2090
				} else {
					K = 1;
					dbgprintf("King's status:%s\n", king_status(K));
					printf("The King fears for his throne and\n"
						"may be planning direct action\n");
				}
			}
		} else {
			printf("You have been overrun and have lost\n"
				"your entire dukedom\n");
			// GOTO 1390
			printf("Your had is placed atop of the\n"
				"castle gate.\n");
			// GOTO 2430
			exit(1);
		}
		// 2090
		//X6 = (X6 <= P) ? -X6 : (float)-P;
		X6 = (X6 > P) ? P : X6;  // fixed
		PA[4] -= (int)X6; // P[4]="War casualties"
		P -= (int)X6;
		GA[8] += (int)(X4 * LA[3]); // G[8]="Crop yield"
		X6 = 40.0f * v;
		if (X6 <= G)
			GA[6] = (int)-X6;  // "Mercenary hire"
			// GOTO 2130
		else {
			GA[6] = -G;
			PA[5] = -(int)((X6 - G) / 7) -1; // "Looting victims"
			printf("There isn't enough grain to pay the\n"
				"mercenaries\n");
		}
		// 2130
		print_tab(GS[6], 25, GA[6]);
		G += GA[6]; // "Mercenary hire"
		PA[5] = (PA[5] <= P) ? -PA[5] : P; // "Looting victims"
		print_tab(PS[5], 25, PA[5]);
		P += PA[5];
		print_tab(LS[3], 25, LA[3]);
		L += LA[3]; // L[3]="Fruits of war"
		U1 -= 2 * PA[4] + 3 * PA[5];  // P[4]="War casualties"
		print_tab(PS[4], 25, PA[4]);
		dbgprintf("Unrest: %g\n", U1);
	} else {
		// 1760
		printf("The High King calls for peasant levies\n"
			"and hires many foreign mercenaries\n");
		K = -2;
		dbgprintf("King's status:%s\n", king_status(K));
	}
}

// simulation
void plague() {
	X1 = (float)FNX(8) + 1;
	dbgprintf("%40s", "");
	dbgprintf("Avoids plague if %g > 3\n", X1);
	if (X1 > 3) {
		// GOTO 2240
	} else {
		// 2190
		if (X1 != 1) {
			// 2230
			printf("A POX EPIDEMIC has broken out\n");
			X2 = X1 * 5;
			PA[6] = -(int)(P / X2); // P[6]="Disease victims"
			P += PA[6];
			print_tab(PS[6], 25, PA[6]);
		} else {
			if (D > 0) {
				// GOTO 2240
			} else {
				printf("THE BLACK PLAGUE has struck the area\n");
				D = 13;
				X2 = 3;
				PA[6] = -(int)(P / X2);
				P += PA[6];
				print_tab(PS[6], 25, PA[6]);
				// GOTO 2240
			}
		}
	}
	--D;
}

void reproduce() {
	// 2240
	X1 = (float)FNX(8) + 4;
	X1 = (PA[5] == 0) ? X1 : 4.5f; // P[5]= "Looting victims"
	PA[8] = (int)(P / X1); // P[8]= "Births"
	printf("%d peasants born\n", PA[8]);
	PA[7] = (int)(0.3 - P / 22); // P[7]= "Natural deaths"
	printf("%d peasants died\n", -PA[7]);
	P += PA[7] + PA[8];
}

void upkeep() {
	X1 = (float)GA[8] - 4000; // G[8]= "Crop yield"
	GA[9] = (X1 <= 0) ? -GA[9] : -(int)(0.1 * X1); // G[9]= "Castle expense"
	GA[9] -= 120;
	printf("Upkeep of %d harvest is %d\n", GA[8], -GA[9]);
	G += GA[9];
}

void tax() {
	// 2280
	if (K < 0) {
		// GOTO 2360
		return;
	} else {
		X1 = (float)(int)(L / 2);
		printf("Tax on %d ha land is %g\n", L, X1);
	}
	//X1 = (K < 2) ? -X1 : -2 * X1;
	if (K >= 2) {
		X1 *= 2;
		printf("The King decided to double your tax!\n");
	}
	if (X1 <= G) {
		GA[10] -= (int)X1;
		G -= (int)X1;
	} else {
		// 2310
		printf("You have insufficient grain to pay\n"
			"the royal tax\n");
		// GOTO 2430
		exit(1);
	}
}


int main() {
	credit();
	init_random_table();
	do {
		// 720
		report();
		test_end_game();
		king_raising_tax();
		printf("Feed 13 hL to avoid starving\n");
		feed();
		if (U1 > 88 || P < 33) test_end_game();
		printf("Tax will be land * 0.5 hL grain.\n");
		trade();
		king_move();
		if (L < 10) test_end_game();
		plant();
		rotate();
		harvest();
		store();
		king_levy();
		war();
		plague();
		reproduce();
		upkeep();
		tax();
		U2 = (int)(U2 * 0.85) + U1;
		printf("Unrest to next year %g\n", U2);
		// GOTO 720
	} while (true);
	return 0;
}
