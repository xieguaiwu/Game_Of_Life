#include<iostream>
#ifdef _WIN32
#include<windows.h>
#include <conio.h> // _WIN32 already includes conio.h for getch()
#else
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
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
using namespace std;

const unsigned char MAX_X = 100, MAX_Y = 100;

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
bool inp();
void mony(int x);

int main() {
	notice();
	if (!inp())return 0;
	mony(0);
	return 0;
}

void notice() {
	cout << "Type \"+\" to create a new cell, and type \"-\" to create empty space.\n";
	cout << "Set the map as follows:\n1)--++--\n2)-+-+--\n...\nn)+-++--\n";
}

void pause(bool screen = false) {
	cout << " (Press any key to continue...)\n";
	getch();
	if (screen)clear_screen();
}

void outp() {
	for (int i = 1; i <= mapX; i++) {
		for (int j = 1; j <= mapY; j++) {
			if (c[i][j] == '+') cout << "⬜";
			else cout << "⬛";
		}
		cout << "\n";
	}
}

bool inp() {
	cout << "Input the size of map.\nX: ";
	// 修正: 检查输入是否成功
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
			if (c[i][j] != '-' && c[i][j] != '+') {
				cout << "Illegal character!\n";
				return false;
			}
		}
	}
	outp();
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
            
			if (c[x][y] == '+') { // 活细胞
				if (live_neighbors < 2 || live_neighbors > 3) {
					next_c[x][y] = '-';
				}
			} else { // 死细胞
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

void mony(int generation) {
	while(true) { // 无限循环模拟
		clear_screen();
		cout << "Generation: " << generation << "\n";
		outp();
		calculate_neighbors(); 
		apply_rules();         
		cout << " (Press 'q' to quit, or any other key to continue...)\n";
		generation++;
        char key = getch();
        if (key == 'q' || key == 'Q') break;
	}
}
