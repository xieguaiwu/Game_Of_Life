#include<iostream>
#ifdef _WIN32
#include<windows.h>
#include <conio.h> // _WIN32 already includes conio.h for getch()
#else
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
//custom getch function
int getch() {
	struct termios oldt, newt;
	int ch;
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	ch = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	return ch;
}
#endif
#include<time.h>
#include<cstdlib>
#include<string>
#include<cstring>
#include<cmath>
#include <fstream>
#include <sstream>

using namespace std;

const unsigned char MAX_X = 100, MAX_Y = 100;

char key;
char c[MAX_X][MAX_Y]; //current
char next_c[MAX_X][MAX_Y]; //intermedia map
unsigned char dmap[MAX_X][MAX_Y]; //counter
unsigned int mapX, mapY;//x-cordinate and y-cordinate to input
int dx[8] = { -1, -1, -1, 0, 1, 1, 1, 0};
int dy[8] = { -1, 0, 1, 1, 1, 0, -1, -1};

void clear_screen() {
#ifdef _WIN32
	system("cls");
#else
	system("clear");
#endif
}

void pause(bool screen);
void notice();
bool reset();
bool reload_file();
void simulate(int x);

int main() {
	bool isGoodLoad = false;
	notice();
	key = getch();
	switch (key) {
	default:
		break;
	case 'R':
	case 'r':
		isGoodLoad = reload_file();
		break;
	}
	if (!isGoodLoad) {
		if (!reset())return 0;
	}
	simulate(0);
	return 0;
}

void notice() {
	cout << "Type \"+\" to create a new cell, and type \"-\" to create empty space.\n";
	cout << "Set the map as follows:\n1)--++--\n2)-+-+--\n...\nn)+-++--\n";
	cout << "Press 'R' to load a custom map file, or other keys to reset map within this program.\n";
}

void pause(bool screen = false) {
	cout << " (Press any key to continue...)\n";
	getch();
	if (screen)clear_screen();
}

void print_map() {
	for (int i = 1; i <= mapX; i++) {
		for (int j = 1; j <= mapY; j++) {
			if (c[i][j] == '+') cout << "⬜";
			else cout << "⬛";
		}
		cout << "\n";
	}
}

bool isGoodCharacter(int checkCharX, int checkCharY) {
	if (c[checkCharX][checkCharY] != '-' && c[checkCharX][checkCharY] != '+') {
		cout << "Illegal character!\n";
		return false;
	}
	return true;
}

bool reset() {
	cout << "Input the size of map.\nX: ";
	//prevent bad int
	if (!(cin >> mapX)) return false;
	cout << "\nY: ";
	if (!(cin >> mapY)) return false;

	cout << "\nGame: " << mapX << " * " << mapY << "\n";
	if (mapX > 100 || mapY > 100 || mapX == 0 || mapY == 0) {
		cout << "x and y must be in the interval (0 , 100]\n";
		return false;
	}
	cout << "Please reset the map\n";
	for (int i = 1; i <= mapX; i++) {
		for (int j = 1; j <= mapY; j++) {
			cin >> c[i][j];
			if (!isGoodCharacter(i, j))return false;
		}
	}
	print_map();
	pause(0);
	return true;
}

bool reload_file() {
	while (1) {
		bool all_okay = true;
		string file_name;
		cout << "Input the file name of your custom map, type \"q!\" to quit:\n";
		cin >> file_name;
		if (file_name == "q!") return false;
		ifstream reader(file_name);
		// detect for errors
		if (!reader.is_open()) {
			cout << "No such file. Try again.\n";
			all_okay = false;
			continue;
		}
		// read dimensions first
		if (!(reader >> mapX >> mapY) ||
		        (mapX > MAX_X || mapY > MAX_Y || mapX == 0 || mapY == 0)) {
			cout << "The x and y coordinates of the map must be in the interval (0 , 100]\n";
			reader.close();
			all_okay = false;
			continue;
		}
		// then read the map to detect illegal character
		for (int i = 1; i <= mapX; i++) {
			for (int j = 1; j <= mapY; j++) {
				reader >> c[i][j];
				if (!isGoodCharacter(i, j)) {
					cout << "Illegal character in map at position (" << i << ", " << j << ")\n";
					reader.close();
					all_okay = false;
					break; // jump away
				}
			}
			if (!all_okay) break;
		}
		if (!all_okay) continue;//start again from the beginning
		else break;//now everything's fine
	}
	cout << "Game: " << mapX << " * " << mapY << "\n";
	print_map();
	pause(0);
	return true;
}

void calculate_neighbors() {
	memset(dmap, 0, sizeof(dmap));
	//reset to zero
	for (unsigned int x = 1; x <= mapX; x++) {
		for (unsigned int y = 1; y <= mapY; y++) {
			for (int i = 0; i < 8; i++) {
				int tx = x + dx[i];
				int ty = y + dy[i];

				// check boundary
				if (tx >= 1 && tx <= mapX && ty >= 1 && ty <= mapY) {
					if (c[tx][ty] == '+') {
						dmap[x][y]++;
					}
				}
			}
		}
	}
}

void apply_rules() {
	for (unsigned int x = 1; x <= mapX; x++) {
		for (unsigned int y = 1; y <= mapY; y++) {
			unsigned char live_neighbors = dmap[x][y];

			next_c[x][y] = c[x][y];

			if (c[x][y] == '+') { //living cells
				if (live_neighbors < 2 || live_neighbors > 3) {
					next_c[x][y] = '-';
				}
			} else { //dead cells
				if (live_neighbors == 3) {
					next_c[x][y] = '+';
				}
			}
		}
	}

	for (unsigned int x = 1; x <= mapX; x++) {
		for (unsigned int y = 1; y <= mapY; y++) {
			c[x][y] = next_c[x][y];
		}
	}
}

void simulate(int generation) {
	while (1) { //main loop for simulation
		clear_screen();
		cout << "Generation: " << generation << "\n";
		print_map();
		calculate_neighbors();
		apply_rules();
		cout << " (Press 'q' to quit, or any other key to continue...)\n";
		generation++;
		key = getch();
		if (key == 'q' || key == 'Q') break;
	}
}
