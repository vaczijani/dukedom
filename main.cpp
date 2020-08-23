#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

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

float M = 1.95f;
int Y = 0;  // year
float C;
float C1 = 3.95f;
float U1 = 0;  // unrest
float U2 = 0;
int K = 0;  // king state
float D = 0;
int P = 100;  // peasants
int L = 600;  // land
int G = 4177; // grain

int PA[9] = {0, 96, 0, 0, 0, 0, 0, -4, 8}; 							// peasant changes
int LA[4] = {0, 600, 0, 0};                							// land changes
int GA[11] = {0, 5193, -1344, 0, -768, 0, 0, 0, 1516, -120, -300};	// grain changes
int SA[7] = {0, 216, 200, 184, 0, 0, 0};							// land quality: 100%, 80%, ...
int U[7] = {0,0,0,0,0,0,0};

const char* PS[9] = {"", "Peasants at start", "Starvations", "King's Levy", "War casulties", "Looting victims", "Disease victims", "Natural deaths", "Births"};
const char* LS[4] = {"", "Land at start", "Bought/sold", "Fruits of war"};
const char* GS[11] = {"", "Grain at start", "Used for food", "Land deals", "Seeding", "Rat losses", "Mercenary hire", "Fruits of war", "Crop yield", "Castle expense", "Royal tax"};
const char* SS[7] = {"", "100%", "80%", "60%", "40%", "20%", "Depl"};

float X1;
float X2;

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

void last_year() {
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
	++Y;
	for (int i = 1; i <= 8; ++i) PA[i] = 0;
	for (int i = 1; i <= 3; ++i) LA[i] = 0;
	for (int i = 1; i <= 10; ++i) GA[i] = 0;
	PA[1] = P;
	LA[1] = L;
	GA[1] = G;
}

void test_end_game() {
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
		printf("The peasants tired of war and starvation\n"
				"You are deposed\n");
		exit(1);

	}
	if (Y > 45 && K == 0) {
		printf("You have reached the age of retirement\n");
		exit(0);
	}
	U1 = 0;
	if (K > 0) {
		printf("The King demands twice the royal tax in\n"
				"THE HOPE TO PROVOKE WAR. WILL YOU PAY");
		char v = getchar();
		K = 2;
		if (v == 'n') K = -1;
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
		printf("Grain for food =");
		int v;
		scanf("%d", &v);
		v = (v >= 100) ? v : v * P;
		if (v > G) {
			not_enough_grain();
		} else {
			print_tab(GS[2], 25, v);
			if (v/P < 11 && v != G)
				printf("The peasants demonstrate before the\ncastle\n");
			GA[2] = -v;
			G += GA[2];
			X1 = v/P;
			if (X1 < 13) {
				printf("Some peasants have starved\n");
				PA[2] = -(int)(P - v/13);
				print_tab(PS[2], 25, PA[2]);
				P += PA[2];
			}
			X1 -= 14;
			X1 = (X1 <= 4) ? -X1 : -4;
			int rest = 3 * PA[2] - 2 * X1;
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
		printf("Land to buy at %g hl/ha = ", X1);
		int v;
		scanf("%d", &v);
		GA[3] = -v * X1;
		if (-GA[3] > G)
			not_enough_grain();
		else {
			LA[2] = v;
			SA[3] += v;
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
	X2 = SA[1] + SA[2] + SA[3];
	for (int j = 1; j <= 3; ++j) {
		--X1;
		printf("Land to sell at %g hl/ha = ", X1);
		int v;
		scanf("%d", &v);
		if (v > X2) 
			printf("But you only have %g ha of good land\n", X2);
		else {
			if (v * X1 <= 4000) {
				GA[3] = v * X1;
				LA[2] = -v;
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
				printf("No buyers have that much grain try less\n");
		}
	}
	printf("Buyers have lost interest\n");
}

void produce_grain() {
	do {
		printf("Land to be planted = ");
		int v;
		scanf("%d", &v);
		if (v > L)
			not_enough_land();
		else if (2 * v > G)
			not_enough_grain();
		else if (4 * P < v)
			not_enough_peasant();
		else {
			GA[4] = -2 * v;
			GA[8] = v;
			print_tab(GS[4], 25, GA[4]);
			G += GA[4];
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

void crop_yield() {
	C = FNX(2) + 9;
	if (Y % 7 == 0) {
		printf("Seven year locusts\n");
		C = (int)(C * 0.65);
	}
	X1 = 0;
	for (int j = 1; j <= 5; ++j) {
		X1 += U[j]*(1.2-0.2*j);
	}
	if (GA[8] == 0) {
		C1 = 0;
		C = 0;
	} else {
		C1 = (int)(C * X1 / GA[8] * 100) / 100;
		C = C1;
	}
	printf("Yield = %g hl/ha\n", C);
	GA[8] = (int)(C * X1);
	X1 = FNX(3) + 3;
	GA[5] = -(int)(X1 * G / 83);
	G += GA[8] + GA[5];
}

void reproduce() {
	X1 = FNX(8) + 4;
	X1 = (PA[5] == 0) ? X1 : 4.5;
	PA[8] = (int)(P / X1);
	PA[7] = (int)(0.3 - P / 22);
	P += PA[7] + PA[8];
	--D;
	//GA[8] = (int)(C * GA[8]);
	//G += GA[8];
	X1 = GA[8] - 4000;
	GA[9] = (X1 <= 0) ? -GA[9] : -(int)(0.1 * X1);
	GA[9] -= 120;
	G += GA[9];
}

int main() {
	init_random_base_table();
	do {
		last_year();
		test_end_game();
		feed_peasants();
		if (U1 > 88 || P < 33) test_end_game();
		trade_land();
		if (L < 10) test_end_game();
		produce_grain();
		crop_yield();
		reproduce();
		U2 = (int)(U2 * 0.85);
	} while (true);
	return 0;
}

