#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#ifndef MIN
 #define MIN(a,b) (a<b)?(a):(b)
#endif

float RND(int d) {
	if (d == 0 || d == 1)
		return (float)rand() / RAND_MAX;
	else
		return (rand() % d) + 1;
}

int FNR(int Q1, int Q2) {
	return (int)(RND(1) * (1 + Q2 - Q1) + Q1);
}

int R[9];
int F3 = 2;
int FNX(int Q1) {
	return FNR(-F3, F3) + R[Q1];	
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

void init_random_base_table() {
	Q1 = 4;
	Q2 = 7;
	R[1] = gauss();
	Q2 = 8;
	R[3] = gauss();
	Q2 = 6;
	R[3] = gauss();
	Q1 = 3;
	R[4] = gauss();
	Q1 = 5;
	R[5] = gauss();
	Q1 = 3;
	Q2 = 6;
	R[6] = gauss();
	Q2 = 8;
	R[7] = gauss();
	Q1 = 1;
	R[8] = gauss();
	printf("Random base table:");
	for (int i = 1; i <= 8; ++i)
		printf("R[%d]=%d ", i, R[i]);
	printf("\n");
}

float M = 1.95f;	// mercenary attack power?
int Y = 0;			// year
float C;			// crop yield?
float C1 = 3.95f;	// last year's yield?
float U1 = 0;		// unrest
float U2 = 0;		// unrest?
int K = 0;			// king state 0=neutral? -1=hiring -2=levy 1=in fear 2=doubling tax!
float D = 0;
// THE RESOURCES: land, grain, peasants
int P = 100;  // peasants
int L = 600;  // land
int G = 4177; // grain

int PA[9] = {0, 96, 0, 0, 0, 0, 0, -4, 8}; 							// peasant changes
int LA[4] = {0, 600, 0, 0};                							// land changes
int GA[11] = {0, 5193, -1344, 0, -768, 0, 0, 0, 1516, -120, -300};	// grain changes
int SA[7] = {0, 216, 200, 184, 0, 0, 0};							// land quality: 100%, 80%, ...
int U[7] = {0,0,0,0,0,0,0};											// usable land for production per quality: 100%, 80%, ...

const char* PS[9] = {"", "Peasants at start", "Starvations", "King's Levy", "War casualties", "Looting victims", "Disease victims", "Natural deaths", "Births"};
const char* LS[4] = {"", "Land at start", "Bought/sold", "Fruits of war"};
const char* GS[11] = {"", "Grain at start", "Used for food", "Land deals", "Seeding", "Rat losses", "Mercenary hire", "Fruits of war", "Crop yield", "Castle expense", "Royal tax"};
const char* SS[7] = {"", "100%", "80%", "60%", "40%", "20%", "Depl"};

float X1;
float X2;
float X3;
float X4;
float X5;
float X6;

void print_tab(const char* text, int tab, int value) {
	printf("%s%*s%d\n", text, tab-strlen(text), "", value);
}

void land_values() {
	for (int j = 1; j <= 6; ++j) printf("%5s", SS[j]);
	printf("\n");
	for (int j = 1; j <= 6; ++j) printf("%5d", SA[j]);
	printf("\n");
}

void new_land_values() {
	for (int j = 1; j <= 6; ++j) printf("%5s", SS[j]);
	printf("\n");
	for (int j = 1; j <= 6; ++j) printf("%5d", U[j]);
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
}

void last_year() {
	// 720
	printf("\nYear %d Peasants %d Land %d Grain %d\n\n", Y, P, L, G);
	for (int j = 1; j <= 8; ++j) 
		if (PA[j] != 0 || j == 1) print_tab(PS[j], 25, PA[j]);
	print_tab("Peasants at end", 25, P);
	for (int j = 1; j <= 3; ++j)
		if (LA[j] != 0 || j == 1) print_tab(LS[j], 25, LA[j]);
	print_tab("Land at end of year", 25, L);
	printf("\n");
	land_values();
	printf("\n");
	for (int j = 1; j <= 10; ++j)
		if (GA[j] != 0 || j == 1) print_tab(GS[j], 25, GA[j]);
	print_tab("Grain at end of year", 25, G);
	if (Y <= 0) printf("(Severe crop damage due to seven\n year locusts)\n");
	printf("\n");
	printf("King's status: %s\n", king_status(K));
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

void king_raising_tax() {
	// 960
	if (K > 0) {
		printf("The King demands twice the royal tax in\n"
			"THE HOPE TO PROVOKE WAR. WILL YOU PAY");
		char v = getchar();
		K = 2;
		if (v == 'n')
			K = -1;
		printf("King's status:%s\n", king_status(K));
	}
}

void not_enough_grain() {
	printf("But you don't have enough grain\n"
			"You have %d hl of grain left,\n", G);
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

void feed_peasants() {
	do {
		printf("Grain for food (less than 100\n"
			" means per peasant)=");
		int v;
		scanf("%d", &v);
		v = (v >= 100) ? v : v * P;  // P = Peasants
		if (v > G) {
			not_enough_grain();
		} else {
			print_tab(GS[2], 25, v); // G[2]= "Used for food"
			if (v/P < 11 && v != G)
				printf("The peasants demonstrate before the\ncastle\n");
			GA[2] = -v;  // "Used for food"
			G += GA[2];  // "Used for food"
			X1 = v/P;
			if (X1 < 13) {
				printf("Some peasants have starved\n");
				PA[2] = -(int)(P - v/13); // "Starvations"
				print_tab(PS[2], 25, PA[2]); // "Starvations"
				P += PA[2]; // P[2]= "Starvations"
			}
			X1 -= 14;
			X1 = (X1 <= 4) ? -X1 : -4;
			int rest = 3 * PA[2] - 2 * X1; // "Starvations"
			U1 -= rest;
			printf("Food made an extra %d unrest. Unrest level is %g\n", -rest, U1);
			break;
		}
	} while (true);
}

void trade_land() {
	float c = C1;
	X1 = (int)(2 * c + FNX(1) - 5);
	X1 = (X1 < 30) ? X1 : 30;
	do {
		printf("Land to buy at %g hl/ha [0-%d]= ", X1, (int)(G/X1));
		int v;
		scanf("%d", &v);
		GA[3] = -v * X1;  // G[3]= "Land deals" negative=buying
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
		land_values();
		return;
	}
	// SELL
	X2 = SA[1] + SA[2] + SA[3];  // size of good (>=60%) land
	// maximum of 3 rounds of "auction"
	for (int j = 1; j <= 3; ++j) {
		--X1;
		printf("Land to sell at %g hl/ha [0-%g]= ", X1, X2);
		int v;
		scanf("%d", &v);
		if (v > X2) 
			printf("But you only have %g ha of good land (yield >= 60%)\n", X2);
		else {
			if (v * X1 <= 4000) {
				GA[3] = v * X1; // G[3]= "Land deals"
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

void produce_grain() {
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
					U[j] = v;
					SA[j] -= v;
					break;
				} else {
					v -= SA[j];
					U[j] = SA[j];
					SA[j] = 0;
				}
			}
			// upgrade land?
			SA[1] += SA[2];
			SA[2] = 0;
			for (int j = 3; j <= 6; ++j) {
				SA[j-2] += SA[j];
				SA[j] = 0;
			}
			for (int j = 1; j <= 5; ++j) {
				SA[j+1] += U[j];
			}
			SA[6] += U[6];
			printf("land:\n");
			land_values();
			printf("seeded:\n");
			new_land_values();
			break;
		}
	} while (true);
}

// simulation: no user input
void harvest() {
	C = FNX(2) + 9;
	if (Y % 7 == 0) {
		printf("Seven year locusts destroyed 35%% of the yield\n");
		C = (int)(C * 0.65);
	}
	X1 = 0;
	// yields: 100%, 80%, 60%...
	for (int j = 1; j <= 5; ++j) {
		X1 += U[j] * (1.2 - 0.2 * j);
	}
	if (GA[8] == 0) {  // G[8]= "Crop yield"
		C1 = 0;
		C = 0;
	} else {
		C1 = (int)(C * X1 / GA[8] * 100) / 100;
		C = C1;
	}
	printf("Yield = %g hl/ha\n", C);
	GA[8] = (int)(C * X1);
	G += GA[8];
}

void rat_infest() {
	X1 = FNX(3) + 3;
	printf("Rat infestation chance: %g < 9\n", X1);
	if (X1 < 9) {
		printf("No infestation\n");
		return;
	}

	GA[5] = -(int)(X1 * G / 83); // G[5]= "Rat losses"
	G += GA[5];
	printf("Rats infest the granary\n");
}

void king_levy() {
	if (P < 67 || K != -1) return;
	X1 = FNX(4);
	printf("King levy chance %g > %g\n", X1, P/30);
	if (X1 > P/30) return;

	printf("The king requires %.0g peasants for\n"
		"his estates and mines. Will you supply\n"
		"for them (y)es or pay %d hl of\n"
		"grain instead (n)o ?");
	char v = getchar();
	if (v == 'n') {
		GA[10] = -100 * X1;  // G[10]= "Royal tax"
		G += GA[10];
	} else {
		PA[3] = -X1; // P[3]= "King's levy"
		P += PA[3];
	}
}

void war() {
	printf("War chances. King's status = %s\n", king_status(K));
	if (K != -1) {
		// 1780
		X1 = (int)(11 - 1.5 * C);	// depends on crop yield?
		X1 = (X1 >= 2) ? -X1 : -2;
		if (K != 0 || P <= 109 || 17 * (L - 400) + G <= 10600) {
			// 1820
			X2 = 0;
		} else {
			printf("The High King grows uneasy and may\n"
				"be subsidizing wars against you\n");
			X1 += 2;
			X2 = Y + 5;
		}
		// 1830
		X3 = FNX(5);
		printf("War chances %g > %g\n", X3, X1);
		if (X3 > X1) {
			// GOTO 2180
			printf("No war\n");
			return;
		}
		printf("A nearby duke threatens war; \n");
		X2 = (int)(X2 + 85 + 18 * FNX(6)); // enemy's war potential?
		X4 = 1.2 - U1 / 16;
		X5 = (int)(P * X4) + 13;  // your war potential?
		printf("Will you attack first? ");
		char c = getchar();
		if (c == 'y') {
			// GOTO 1920
		} else {
			if (X2 >= X5) {
				printf("First strike failed - you need\n"
					"professionals\n");
				PA[4] = -X3 - X1 - 2; // P[4]= "War casualties"
				X2 += 3 * PA[4];
			} else {
				printf("Peace negotiations successful\n");
				PA[4] = -X1 - 1;
				X2 = 0;
				// GOTO 1910
			}
			// 1910
			P += PA[4];
			if (X2 < 1) {
				U1 -= 2 * PA[4] + 3 * PA[5]; // P[5]= "Looting victims"
				return;
			}
		}
		// 1920
		int v;
		do {
			printf("How many mercenaries will you hire\n"
				"at 40 hl each = ");
			scanf("%d", &v);
			if (v > 75) {
				printf("There are only 75 available for hire\n");
				v = -1;
			}
		} while (v < 0);
		X2 = (int)(X2 * M);
		X5 = (int)(P * X4) + 7 * v + 13;
		X6 = X2 - 4 * v - (int)(0.25 * X5);
		X2 = X5 - X2;
		LA[3] = (int)(0.8 * X2);
		if (-LA[3] <= 0.67 * L) {
			X1 = LA[3];
			for (int j = 1; j <= 3; ++j) {
				X3 = (int)(X1 / (4 - j));
				if (-X3 <= SA[j])
					X5 = X3;
				else
					X5 = -SA[j];
				SA[j] += X5;
				X1 -= X5;
			}
			if (LA[3] < 399) {
				// GOTO 2050
				if (X2 < 1) {
					// GOTO 2070
					printf("You have lost the war\n");
					X4 = GA[8] / L; // G[8]="Crop yield"
				} else {
					printf("You have won the war\n");
					X4 = 0.67;
					GA[7] = (int)(1.7 * LA[3]); // G[7]="Fruits of war" L[3]="Fruits of war"
					G + GA[7];
					// GOTO 2080
				}
				// 2080
				if (X6 <= 9)
					X6 = 0;
				else
					X6 = (int)(X6 / 10);
			} else {
				printf("You have overrun the enemy and annexed\n"
					"his entire dukedom\n");
				GA[7] = 3513; // G[7] = "Fruits of war"
				G += GA[7];
				X6 = -47;
				X4 = 0.55;				
				if (K > 0) {
					// GOTO 2090
				} else {
					K = 1;
					printf("King's status:%s\n", king_status(K));
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
		X6 = (X6 <= P) ? -X6 : -P;
		PA[4] -= X6; // P[4]="War casualties"
		P += PA[4];
		GA[8] += (int)(X4 * LA[3]); // G[8]="Crop yield"
		X6 = 40 * v;
		if (X6 <= G)
			GA[6] = -X6;  // "Mercenary hire"
			// GOTO 2130
		else {
			GA[6] = -G;
			PA[5] = -(int)((X6 - G) / 7) -1; // "Looting victims"
			printf("There isn't enough grain to pay the\n"
				"mercenaries\n");
		}
		// 2130
		G += GA[6]; // "Mercenary hire"
		PA[5] = (PA[5] <= P) ? -PA[5] : P; // "Looting victims"
		P += PA[5];
		L += LA[3]; // L[3]="Fruits of war"
		U1 -= 2 * PA[4] + 3 * PA[5];  // P[4]="War casualties"
	} else {
		// 1760
		printf("The High King calls for peasant levies\n"
			"and hires many foreign mercenaries\n");
		K = -2;
		printf("King's status:%s\n", king_status(K));
	}
}

// simulation
void plague() {
	X1 = FNX(8) + 1;
	printf("Plague chance %g > 3\n", X1);
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
	X1 = FNX(8) + 4;
	X1 = (PA[5] == 0) ? X1 : 4.5; // P[5]= "Looting victims"
	PA[8] = (int)(P / X1); // P[8]= "Births"
	printf("%d peasants born\n", PA[8]);
	PA[7] = (int)(0.3 - P / 22); // P[7]= "Natural deaths"
	printf("%d peasants died\n", -PA[7]);
	P += PA[7] + PA[8];
}

void castle_upkeep() {
	X1 = GA[8] - 4000; // G[8]= "Crop yield"
	GA[9] = (X1 <= 0) ? -GA[9] : -(int)(0.1 * X1); // G[9]= "Castle expense"
	GA[9] -= 120;
	printf("Castle upkeep is %d after last year's %d harvest\n", -GA[9], GA[8]);
	G += GA[9];
}

void tax() {
	// 2280
	if (K < 0) {
		// GOTO 2360
		return;
	} else {
		X1 = (int)(L / 2);
		printf("Your tax to pay after your %d ha land is %g\n", L, X1);
	}
	X1 = (K < 2) ? -X1 : -2 * X1;
	if (K >= 2) printf("The King decided to double your tax!\n");
	if (-X1 <= G) {
		GA[10] += X1;
		G += X1;
	} else {
		// 2310
		printf("You have insufficient grain to pay\n"
			"the royal tax\n");
		// GOTO 2430
		exit(1);
	}
}

void test() {
	for (int i = 0; i < 1000; ++i)
		printf("%10d", FNX(2) + 9);
}

int main() {
	init_random_base_table();
	//test();return 0;
	do {
		// 720
		last_year();
		test_end_game();
		king_raising_tax();
		printf("Peasants need at least 14 hl\n"
			"of grain to not starve.\n");
		feed_peasants();
		if (U1 > 88 || P < 33) test_end_game();
		printf("End of year tax will be Land / 2\n"
			"hl of grain.\n");
		trade_land();
		if (L < 10) test_end_game();
		produce_grain();
		harvest();
		rat_infest();
		king_levy();
		war();
		plague();
		reproduce();
		castle_upkeep();
		tax();
		U2 = (int)(U2 * 0.85) + U1;
		// GOTO 720
	} while (true);
	return 0;
}

