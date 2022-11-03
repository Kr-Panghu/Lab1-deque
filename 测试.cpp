#include<iostream>
//#include<conio.h>
#include<stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <term.h>
#include <curses.h>
#include <unistd.h>
using namespace std;
int Count = 0;
const int ONE_SECOND = 100000;
char tran[12] = { '0','#','*',' ','1','2','$','@','(',')','+' };
//0ը����1Ӳǽ��2��ǽ��3½�أ�4���ٵ��ߣ�5�������ߣ�6���1��7���2��8������1��9������2,10ը����ըЧ��
static int peek_character = -1;
static struct termios initial_settings, new_settings;
int _kbhit()  
{  
    char ch;  
    int nread;  
    if(peek_character != -1)  
        return 1;  
    new_settings.c_cc[VMIN]=0;  
    tcsetattr(0, TCSANOW, &new_settings);  
    nread = read(0,&ch,1);  
    new_settings.c_cc[VMIN]=1;  
    tcsetattr(0, TCSANOW, &new_settings);  
if(nread == 1) {  
      peek_character = ch;  
      return 1;  
}  
return 0;  
}  

int _getch(void) 
{
        int c=0;
        struct termios org_opts, new_opts;
        int res=0;
        //-----  store old settings -----------
        res=tcgetattr(STDIN_FILENO, &org_opts);
        assert(res==0);
        //---- set new terminal parms --------
        memcpy(&new_opts, &org_opts, sizeof(new_opts));
        new_opts.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ECHOPRT | ECHOKE | ICRNL);
        tcsetattr(STDIN_FILENO, TCSANOW, &new_opts);
        c=getchar();
            //------  restore old settings ---------
        res=tcsetattr(STDIN_FILENO, TCSANOW, &org_opts);assert(res==0);
        return c;
}

int map[17][17] = {
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,3,2,2,1,3,2,1,3,3,3,2,1,3,1,2,1},
	{1,2,1,3,2,2,2,2,1,2,1,2,2,3,2,2,1},
	{1,2,3,1,3,1,1,3,2,2,2,1,3,1,3,1,1},
	{1,1,3,3,8,3,3,1,2,1,2,3,7,3,2,3,1},
	{1,2,3,1,3,1,2,2,2,1,2,1,3,1,1,3,1},
	{1,1,2,1,2,1,3,1,1,1,2,1,3,2,1,2,1},
	{1,3,2,2,2,2,2,1,1,2,2,2,2,2,2,2,1},
	{1,1,2,1,3,1,2,3,3,3,2,1,2,1,3,1,1},
	{1,3,3,1,3,1,1,3,1,2,1,1,3,1,3,3,1},
	{1,2,3,3,6,3,2,3,2,2,2,3,9,3,2,3,1},
	{1,3,1,1,3,1,2,3,1,2,1,1,3,1,1,2,1},
	{1,2,3,1,2,1,2,1,1,3,1,2,3,3,1,3,1},
	{1,3,3,2,2,2,3,2,2,2,2,2,1,2,3,3,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};
bool boom[17][17] = { 0 };//0����û��ը����1������ը��
int props[17][17] = {
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,3,3,3,1,3,4,1,3,3,3,4,1,3,1,3,1},
	{1,3,1,3,3,5,3,3,1,3,1,3,5,3,3,3,1},
	{1,3,3,1,3,1,1,3,3,3,3,1,3,1,3,1,1},
	{1,1,3,3,8,3,3,1,3,1,3,3,7,3,3,3,1},
	{1,3,3,1,3,1,4,5,3,1,3,1,3,1,1,3,1},
	{1,1,3,1,4,1,3,1,1,1,3,1,3,3,1,3,1},
	{1,3,3,3,3,3,3,1,1,3,3,3,3,3,3,3,1},
	{1,1,4,1,3,1,3,3,3,3,5,1,3,1,3,1,1},
	{1,3,3,1,3,1,1,3,1,3,1,1,3,1,3,3,1},
	{1,5,3,3,6,3,3,3,3,3,4,3,9,3,3,3,1},
	{1,3,1,1,3,1,3,3,1,3,1,1,3,1,1,3,1},
	{1,4,3,1,5,1,3,1,1,3,1,3,3,3,1,3,1},
	{1,3,3,4,3,5,3,3,3,5,3,4,1,3,3,3,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};//map��¼���Σ�props��¼����
int bombnumber = 0;
class Bomb {
public:
	int xlocation, ylocation;
	int distance = 0;
	int owner = 0;//1����p1,2����p2,3����p3,4����p4;
	int countdown;
	Bomb* next = NULL;
	Bomb* last = NULL;
	Bomb(int dis, int ow, int x, int y) {
		distance = dis;
		owner = ow;
		countdown = 3;
		xlocation = x;
		ylocation = y;
	}
	void BOOM();
	void reduce_countdown();
};
class Boomed_Bomb {
public:
	int count;
	int x, y;
	Boomed_Bomb* last;
	Boomed_Bomb* next;
	Boomed_Bomb(int a, int b){
		count = 1;
		x = a;
		y = b;
		next = last = nullptr;
	};
};
class BaseCharacter {
public:
	int blood = 1;
	int symbol = 0, score = 0;
	int x_coordinate = 0, y_coordinate = 0;
	int movespeed = 3;
	int attack_force = 1;
	int move_time_limit = 0;
	int bomb_time_limit = 0;
	void update_location(int type);
	BaseCharacter(int x, int y, int plnum) {
		x_coordinate = x;
		y_coordinate = y;
		symbol = plnum;
	}
	int getscore() {
		return score;
	}
	//�����ͷ�ը��������
	void Addlimit() {
		if (move_time_limit != 0) {
			move_time_limit = move_time_limit + movespeed;
			if (move_time_limit > ONE_SECOND)move_time_limit = 0;
		}
		if (bomb_time_limit != 0) {
			bomb_time_limit = bomb_time_limit + 3;
			if (bomb_time_limit > ONE_SECOND)bomb_time_limit = 0;
		}
	}
};
BaseCharacter p1(10, 4, 6);
BaseCharacter p2(4, 12, 7);
BaseCharacter robot1(4, 4, 8);
BaseCharacter robot2(10, 12, 9);
BaseCharacter* find_character[5];
Bomb* Head = new Bomb(1, 1, 1, 1);
Bomb* Tail = new Bomb(1, 1, 1, 1);
Boomed_Bomb* Bhead = new Boomed_Bomb(0, 0);
Boomed_Bomb* Btail = new Boomed_Bomb(0, 0);
void init() {
	find_character[1] = &p1;
	find_character[2] = &p2;
	find_character[3] = &robot1;
	find_character[4] = &robot2;
	Head->next = Tail;
	Tail->last = Head;
	Bhead->next = Btail;
	Btail->last = Bhead;
}
void display();
void deal_with_input();
void deal_with_timer();
int main() {
	init();//ȫ�ֳ�ʼ��
	display();
	while (1) {
		deal_with_input();
		Count = Count + 1;
		p1.Addlimit();
		p2.Addlimit();
		if (Count == ONE_SECOND) {//����ONE_SECONDʹ���ifÿ��һ��
			deal_with_timer();
			Count = 0;
		}
	}
	return 0;
}
void display() {
	system("clear");
	for (int i = 1; i <= 13; i++) {
		for (int j = 1; j <= 15; j++) {
			cout << tran[map[i][j]];
		}
		cout << endl;
	}
	int t1 = p1.score;
	int t2 = p2.score;
	int t3 = robot1.score;
	int t4 = robot2.score;
	cout << "score: player1=" << t1 << "   player2=" << t2 << endl;
	cout << "       robot 1=" << t3 << "   robot 1=" << t4 << endl;
}
void deal_with_input() {
	char ch;
	if (_kbhit()) {//�а����򷵻���
		ch = _getch();
		//WASD�ƶ����ո��ͷ�ը��/IJKL�ƶ����س��ͷ�ը��
		if ((p1.blood == 1 && ch == 'w' || ch == 'W') || (ch == 'a' || ch == 'A') || (ch == 's' || ch == 'S') || (ch == 'd' || ch == 'D')) {
			if (p1.move_time_limit == 0) {
				if (ch == 'w' || ch == 'W') {
					p1.update_location(3);
					p1.move_time_limit = p1.movespeed;
				}
				else if (ch == 's' || ch == 'S') {
					p1.update_location(4);
					p1.move_time_limit = p1.movespeed;
				}
				else if (ch == 'A' || ch == 'a') {
					p1.update_location(1);
					p1.move_time_limit = p1.movespeed;
				}
				else {
					p1.update_location(2);
					p1.move_time_limit = p1.movespeed;
				}
				display();
			}
		}
		else if (p1.blood == 1 && ch == ' ') {
			if (p1.bomb_time_limit == 0) {
				p1.bomb_time_limit = 3;
				boom[p1.x_coordinate][p1.y_coordinate] = 1;
				Bomb bom(p1.attack_force, 1, p1.x_coordinate, p1.y_coordinate);
				Bomb* bompo = &bom;
				bompo->next = Tail;
				bompo->last = Tail->last;
				Tail->last->next = bompo;
				Tail->last = bompo;
			}
		}
		else if ((p2.blood == 1 && ch == 'i' || ch == 'I') || (ch == 'j' || ch == 'J') || (ch == 'k' || ch == 'K') || (ch == 'l' || ch == 'L')) {
			if (p2.move_time_limit == 0) {
				if (ch == 'i' || ch == 'I') {
					p2.update_location(3);
					p2.move_time_limit = p2.movespeed;
				}
				else if (ch == 'k' || ch == 'K') {
					p2.update_location(4);
					p2.move_time_limit = p2.movespeed;
				}
				else if (ch == 'j' || ch == 'J') {
					p2.update_location(1);
					p2.move_time_limit = p2.movespeed;
				}
				else {
					p2.update_location(2);
					p2.move_time_limit = p2.movespeed;
				}
				display();
			}
		}
		else if (p2.blood == 1 && ch == '\r') {
			if (p2.bomb_time_limit == 0) {
				p2.bomb_time_limit = 3;
				boom[p2.x_coordinate][p2.y_coordinate] = 1;
				Bomb bom(p2.attack_force, 2, p2.x_coordinate, p2.y_coordinate);
				Bomb* bompo = &bom;
				bompo->next = Tail;
				bompo->last = Tail->last;
				Tail->last->next = bompo;
				Tail->last = bompo;
			}
		}
	}
}
void BaseCharacter::update_location(int type) {//1����2�ң�3�ϣ�4��
	if (type == 3 && map[x_coordinate - 1][y_coordinate] > 2 && map[x_coordinate - 1][y_coordinate] < 6) {
		if (props[x_coordinate - 1][y_coordinate] == 4) {
			movespeed++;
			score = score + 50;
		}
		else if (props[x_coordinate - 1][y_coordinate] == 5) {
			attack_force++;
			score = score + 50;
		}
		if (boom[x_coordinate][y_coordinate]) map[x_coordinate][y_coordinate] = 0;
		else map[x_coordinate][y_coordinate] = 3;
		x_coordinate = x_coordinate - 1;
		map[x_coordinate][y_coordinate] = symbol;
		display();
	}
	else if (type == 4 && map[x_coordinate + 1][y_coordinate] > 2 && map[x_coordinate + 1][y_coordinate] < 6) {
		if (props[x_coordinate + 1][y_coordinate] == 4) {
			movespeed++;
			score = score + 50;
		}
		else if (props[x_coordinate + 1][y_coordinate] == 5) {
			attack_force++;
			score = score + 50;
		}
		if (boom[x_coordinate][y_coordinate]) map[x_coordinate][y_coordinate] = 0;
		else map[x_coordinate][y_coordinate] = 3;
		x_coordinate = x_coordinate + 1;
		map[x_coordinate][y_coordinate] = symbol;
		display();
	}
	else if (type == 1 && map[x_coordinate][y_coordinate - 1] > 2 && map[x_coordinate][y_coordinate - 1] < 6) {
		if (props[x_coordinate][y_coordinate - 1] == 4) {
			movespeed++;
			score = score + 50;
		}
		else if (props[x_coordinate][y_coordinate - 1] == 5) {
			attack_force++;
			score = score + 50;
		}
		if (boom[x_coordinate][y_coordinate])map[x_coordinate][y_coordinate] = 0;
		else map[x_coordinate][y_coordinate] = 3;
		y_coordinate = y_coordinate - 1;
		map[x_coordinate][y_coordinate] = symbol;
		display();
	}
	else if (type == 2 && map[x_coordinate][y_coordinate + 1] > 2 && map[x_coordinate][y_coordinate + 1] < 6) {
		if (props[x_coordinate][y_coordinate + 1] == 4) {
			movespeed++;
			score = score + 50;
		}
		else if (props[x_coordinate][y_coordinate + 1] == 5) {
			attack_force++;
			score = score + 50;
		}
		if (boom[x_coordinate][y_coordinate])map[x_coordinate][y_coordinate] = 0;
		else map[x_coordinate][y_coordinate] = 3;
		y_coordinate = y_coordinate + 1;
		map[x_coordinate][y_coordinate] = symbol;
		display();
	}
	return;
}
void deal_with_timer() {//ը���ĵ���ʱ��ը��Ч���Ļ���
	Bomb* po = Head->next;
	while (po != Tail) {
		po->reduce_countdown();
		po = po->next;
	}
	Boomed_Bomb* po1 = Bhead->next;
	while (po1 != Btail) {
		if (po1->count > 0)po1->count--;
		else {
			map[po1->x][po1->y] = props[po1->x][po1->y];
			po1->last->next = po1->next;
			po1->next->last = po1->last;
		}
		po1 = po1->next;
	}
	display();
}
void Bomb::BOOM() {
	int point = 0;
	bool flagup = 1, flagdown = 1, flagright = 1, flagleft = 1;
	if (map[xlocation][ylocation] >= 6 && map[xlocation][ylocation] <= 9) {
		find_character[owner]->blood = 0;
		map[find_character[owner]->x_coordinate][find_character[owner]->y_coordinate] = 3;
	}
	Boomed_Bomb newbomb(xlocation, ylocation);
	newbomb.next = Btail;
	newbomb.last = Btail->last;
	for (int i = 1; i <= distance; i++) {
		if (flagup == 1) {
			if (map[xlocation - i][ylocation] == 1)flagup = 0;
			else {
				if (map[xlocation - i][ylocation] == 2) {
					flagup = 0;
					map[xlocation - i][ylocation] = 10;
					point = point + 10;
				}
				else if (map[xlocation - i][ylocation] >= 6 && map[xlocation - i][ylocation] <= 9) {
					int tem = map[xlocation - i][ylocation];
					point = point + 100;
					find_character[tem - 5]->blood = 0;
					map[xlocation - i][ylocation] = 10;
				}
				Boomed_Bomb newbomb(xlocation - i, ylocation);
				newbomb.next = Btail;
				newbomb.last = Btail->last;
				Btail->last->next = &newbomb;
				Btail->last = &newbomb;

			}
		}
		else if (flagdown == 1) {
			if (map[xlocation + i][ylocation] == 1)flagdown = 0;
			else {
				if (map[xlocation + i][ylocation] == 2) {
					flagdown = 0;
					map[xlocation + i][ylocation] = 10;
					point = point + 10;
				}
				else if (map[xlocation + i][ylocation] >= 6 && map[xlocation + i][ylocation] <= 9) {
					int tem = map[xlocation + i][ylocation];
					point = point + 100;
					find_character[tem - 5]->blood = 0;
					map[xlocation + i][ylocation] = 10;
				}
				Boomed_Bomb newbomb(xlocation + i, ylocation);
				newbomb.next = Btail;
				newbomb.last = Btail->last;
				Btail->last->next = &newbomb;
				Btail->last = &newbomb;
			}
		}
		else if (flagright == 1) {
			if (map[xlocation][ylocation + i] == 1)flagright = 0;
			else {
				if (map[xlocation][ylocation + i] == 2) {
					flagright = 0;
					map[xlocation][ylocation + i] = 10;
					point = point + 10;
				}
				else if (map[xlocation][ylocation + i] >= 6 && map[xlocation][ylocation + i] <= 9) {
					int tem = map[xlocation][ylocation + i];
					point = point + 100;
					find_character[tem - 5]->blood = 0;
					map[xlocation][ylocation + i] = 10;
				}
				Boomed_Bomb newbomb(xlocation, ylocation + i);
				newbomb.next = Btail;
				newbomb.last = Btail->last;
				Btail->last->next = &newbomb;
				Btail->last = &newbomb;
			}
		}
		else if (flagleft == 1) {
			if (map[xlocation][ylocation - i] == 1)flagleft = 0;
			else {
				if (map[xlocation][ylocation - i] == 2) {
					flagleft = 0;
					map[xlocation][ylocation - i] = 10;
					point = point + 10;
				}
				else if (map[xlocation][ylocation - i] >= 6 && map[xlocation][ylocation - i] <= 9) {
					int tem = map[xlocation][ylocation - i];
					point = point + 100;
					find_character[tem - 5]->blood = 0;
					map[xlocation][ylocation + i] = 10;
				}
				Boomed_Bomb newbomb(xlocation, ylocation - i);
				newbomb.next = Btail;
				newbomb.last = Btail->last;
				Btail->last->next = &newbomb;
				Btail->last = &newbomb;
			}
		}
	}
	find_character[owner]->score = find_character[owner]->score + point;
	display();
}
void Bomb::reduce_countdown() {
	if (countdown > 0) countdown--;
	else {
		BOOM();
		last->next = next;
		next->last = last;
	}
	return;
}

