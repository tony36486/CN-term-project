#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <ctime>
#include <math.h>
#include <stdio.h>
#include <Windows.h>

#include "Bomb.h"
#pragma comment(lib, "Ws2_32.lib")
#define TABLE_SIZE 10
#define MAX_BOMB 10
#define GAME_TABLE_WIDTH 20
#define GAME_TABLE_HEIGHT 15

using namespace std;

#define STATE_LOBBY 0
#define STATE_READY 1
#define STATE_INIT 2
#define STATE_PLAYING 3
#define STATE_GAMEOVER 4

struct block{
	unsigned int num;
	block* up;
	block* down;
	block* left;
	block* right;
};

struct data{
	int posX;
	int posY;
	int dir;
	int state;
	int bombCount;
	Bomb updateBomb[MAX_BOMB];
};

struct CStruct{
	int order;
	int ready;
	int state;	// 遊戲階段 0:遊戲大廳 1:準備遊戲 2:遊戲初始化 3:遊戲中 4:遊戲結束(顯示成績) 
	int ID;	//起始為-1 , 由server分配
	bool confirm;	//確認回復用
	bool dataChanged;
	data updateData[4];

	CStruct() //初始化 CStruct 中的
	{
		state = 0;
		ready = 0;
	}

};

#define ROOM_STATE_WAITING 0
#define ROOM_STATE_PLAYING 1
#define ROOM_STATE_TERMINATED 2

struct Room{
	int man;
	int state;
	CStruct player[4];

	Room() //初始化 Room 中的
	{
		man = 0;
		state = ROOM_STATE_WAITING;
	}

};
Room roomMem[TABLE_SIZE];


void toExplode(Bomb*, Room*, int, int);




int main(int argc, char *argv[])
{
	srand(time(NULL));

	//init
	WSADATA wsaData = { 0 };
	WORD wVer = MAKEWORD(2, 2);
	WSAStartup(wVer, &wsaData);

	//socket
	SOCKET hSock; // listen socket
	hSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//bind
	sockaddr_in saListen = { 0 };
	saListen.sin_family = AF_INET;
	saListen.sin_port = htons(10000);
	saListen.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(hSock, (sockaddr*)&saListen, sizeof(sockaddr));

	//listen
	listen(hSock, 5);
	printf_s("waiting for connection ... \n\n");

	SOCKET socket_array[100];	// socket array
	int s, socket_number = 0; // number of connected sockets
	int pair = 0; //Universe Variable for processing
	bool intial = 0;
	int livecount[4][4] = { 0 };//沒爆的炸彈
	bool exploded[4] = { 0 };//
	fd_set set_read;
	Bomb bombTimebuffer[MAX_BOMB];

	int roomNum = 0;
	int randRoomNum, randMan;

	while (5)
	{
		// select
		{
			// clear the set
			FD_ZERO(&set_read);

			// put listen socket into the set
			FD_SET(hSock, &set_read);

			// put client sockets into the set
			for (s = 0; s < socket_number; s++)
				FD_SET(socket_array[s], &set_read);

			// pick out sockets which have incoming data
			select(0, &set_read, NULL, NULL, NULL);
		}

		// check listen socket
		sockaddr_in saClient;

		if (FD_ISSET(hSock, &set_read) != 0)
		{
			//accept
			int nSALen = sizeof(sockaddr);
			socket_array[socket_number] = accept(hSock, (sockaddr*)&saClient, &nSALen);
			socket_number++;
			printf_s("incoming connection accepted \n");
			printf_s("\t total %d users connected\n\n", socket_number);
			printf_s("inc client ip:%x\n", saClient.sin_addr.s_addr);
			printf_s("inc client port:%x\n\n", saClient.sin_port);
		}

		// check client sockets
		for (s = 0; s < socket_number; s++)
		{
			if (FD_ISSET(socket_array[s], &set_read) == 0)
				// not in the set (no incoming data)
				continue;

			printf_s("client socket #%d selected\n\n", s);

			//Universe Variable for data 
			CStruct recv_buffer;
			int recvByte, sendByte;

			// receiving data from client
			recvByte = recv(socket_array[s], (char*)&recv_buffer, sizeof(CStruct), 0);


			printf_s("\t recv test (Normal)\n");
			printf_s("\t client state : %d \n", recv_buffer.state);
			printf_s("\t client ready : %d \n", recv_buffer.ready);
			printf_s("\t client ID : %d \n", recv_buffer.ID);
			printf_s("\t client confirm : %d \n", recv_buffer.confirm);
			printf_s("\t revcByte NEW : %d \n\n", recvByte);

			
			if (recvByte <= 0) {

				printf_s("\t recv test (Close)\n");
				printf_s("\t client state : %d \n", recv_buffer.state);
				printf_s("\t client ready : %d \n", recv_buffer.ready);
				printf_s("\t client ID : %d \n", recv_buffer.ID);
				printf_s("\t client confirm : %d \n", recv_buffer.confirm);
				printf_s("\t revcByte NEW : %d \n\n", recvByte);

				int disConnRoom;
				int disConnPlayer;
				//Find disconnected player using variable s
				bool check = false;
				for (int i = 0; i < TABLE_SIZE; i++) {
					for (int j = 0; j < roomMem[i].man; j++) {
						if (roomMem[i].player[j].order == s) {
							disConnRoom = i;
							disConnPlayer = j;
							check = true;
							break;
						}
					}
					if (check)
						break;
				}
				if (check) {
					//Set the state of disconnected player to STAT_DISCONN
					for (int i = 0; i < roomMem[disConnRoom].man; i++) {
						roomMem[disConnRoom].player[i].updateData[disConnPlayer].state = 3; //3 is the value of STAT_DISCONN in class Player
						roomMem[disConnRoom].player[i].updateData[disConnPlayer].dir = -1; //Indicating that this player is disconnected and won't update
					}
				}

				closesocket(socket_array[s]);
				socket_number--;
				socket_array[s] = socket_array[socket_number];
				continue;
			}

			printf_s("\t %d bytes received\n\n", recvByte);

			for (int i = 0; i < TABLE_SIZE; i++) {
				if (roomMem[i].state == ROOM_STATE_TERMINATED) {
					roomMem[i].man = 0;
					roomMem[i].state = ROOM_STATE_WAITING;
					for (int j = 0; j < roomMem[i].man; j++) {
						roomMem[i].player[j].state = STATE_GAMEOVER;
					}
				}
			}

			//start processing data
			switch (recv_buffer.state){
			case STATE_LOBBY:
				printf_s("\t in MODE_LOBBY\n\n");
				if (recv_buffer.ID == -1){
					if (roomMem[roomNum].man > 3 || roomMem[roomNum].state == ROOM_STATE_PLAYING) {
						if (roomMem[(roomNum + 1) % TABLE_SIZE].state == ROOM_STATE_WAITING && roomMem[(roomNum + 1) % TABLE_SIZE].man < 4)
							roomNum = (roomNum + 1) % TABLE_SIZE;
						else {
							closesocket(socket_array[s]);
							socket_number--;
							continue;
						}
					}

					recv_buffer.ID = roomMem[roomNum].man + 10 * roomNum;
					recv_buffer.confirm = 1;
					recv_buffer.order = s;
					
					roomMem[roomNum].player[roomMem[roomNum].man++] = recv_buffer;

					recv_buffer.ready = roomMem[roomNum].man;
					sendByte = send(socket_array[recv_buffer.order], (char*)&recv_buffer, sizeof(CStruct), 0);

					printf_s("\t send test\n");
					printf_s("\t client state : %d \n", recv_buffer.state);
					printf_s("\t client ready : %d \n", recv_buffer.ready);
					printf_s("\t client ID : %d \n", recv_buffer.ID);
					printf_s("\t client confirm : %d \n", recv_buffer.confirm);
					printf_s("\t sendByte NEW : %d \n\n", recvByte);
				}
				else{
					randRoomNum = recv_buffer.ID / 10;
					randMan = recv_buffer.ID % 10;

					roomMem[randRoomNum].player[randMan].confirm = 1;
					roomMem[randRoomNum].player[randMan].ready = roomMem[randRoomNum].man;
					sendByte = send(socket_array[recv_buffer.order], (char*)&(roomMem[randRoomNum].player[randMan]), sizeof(CStruct), 0);

					printf_s("\t send test\n");
					printf_s("\t client state : %d \n", recv_buffer.state);
					printf_s("\t client ready : %d \n", recv_buffer.ready);
					printf_s("\t client ID : %d \n", recv_buffer.ID);
					printf_s("\t client confirm : %d \n", recv_buffer.confirm);
					printf_s("\t sendByte NEW : %d \n\n", recvByte);
				}
				break;


			case STATE_READY:
				randRoomNum = recv_buffer.ID / 10;
				roomMem[randRoomNum].state = ROOM_STATE_PLAYING;
				for (int i = 0; i < 4; i++) {
					roomMem[randRoomNum].player[i].state = STATE_INIT;
					roomMem[randRoomNum].player[i].ready = roomMem[randRoomNum].man;
				}
				recv_buffer.ready = roomMem[randRoomNum].man;
				recv_buffer.state = STATE_INIT;
				recv_buffer.confirm = 1;
				sendByte = send(socket_array[recv_buffer.order], (char*)&recv_buffer, sizeof(CStruct), 0);

				//Generate wall map
				for (int i = 0; i < 2; i++) {
					for (int j = 0; j < 10; j++) {
						int randPosX = 1 + rand() % (GAME_TABLE_WIDTH - 2);
						int randPosY = 1 + rand() % (GAME_TABLE_HEIGHT - 2);
						for (int k = 0; k < roomMem[randRoomNum].man; k++) {
							roomMem[randRoomNum].player[k].updateData[i].updateBomb[j].setX(randPosX);
							roomMem[randRoomNum].player[k].updateData[i].updateBomb[j].setY(randPosY);
						}
					}
				}
				break;

			case STATE_INIT:
				randRoomNum = recv_buffer.ID / 10;
				randMan = recv_buffer.ID % 10;
				roomMem[randRoomNum].player[randMan].state = STATE_PLAYING;
				sendByte = send(socket_array[recv_buffer.order], (char*)&roomMem[randRoomNum].player[randMan], sizeof(CStruct), 0);
				break;

			case STATE_PLAYING:
			case STATE_GAMEOVER:
				randRoomNum = recv_buffer.ID / 10;
				randMan = recv_buffer.ID % 10;

				FILETIME gottime;
				GetSystemTimeAsFileTime(&gottime);

				//Check if data changed
				if (recv_buffer.dataChanged) {
					//Update all players data
					for (int i = 0; i < roomMem[randRoomNum].man; i++) {
						roomMem[randRoomNum].player[i].updateData[randMan].posX = recv_buffer.updateData[randMan].posX;
						roomMem[randRoomNum].player[i].updateData[randMan].posY = recv_buffer.updateData[randMan].posY;
						roomMem[randRoomNum].player[i].updateData[randMan].dir = recv_buffer.updateData[randMan].dir;
						roomMem[randRoomNum].player[i].updateData[randMan].state = recv_buffer.updateData[randMan].state;
						if (recv_buffer.updateData[randMan].bombCount) {
							for (int j = 0; j < recv_buffer.updateData[randMan].bombCount; j++) {
								recv_buffer.updateData[randMan].updateBomb[j].setTimer(gottime.dwLowDateTime);
								roomMem[randRoomNum].player[i].updateData[randMan].updateBomb[livecount[i][randMan] + j] = recv_buffer.updateData[randMan].updateBomb[j];
							}
							roomMem[randRoomNum].player[i].updateData[randMan].bombCount = livecount[i][randMan] + recv_buffer.updateData[randMan].bombCount;
						}
					}
					livecount[randMan][randMan] += recv_buffer.updateData[randMan].bombCount;
				}
				
				//Check if every player has finished thier first update
				bool check = true;
				for (int i = 0; i < roomMem[randRoomNum].man; i++) {
					if (roomMem[randRoomNum].player[randMan].updateData[i].dir == 0) {
						check = false;
						break;
					}
				}
				if (check) {
					//Count remaining players
					int count = 0;
					for (int i = 0; i < roomMem[randRoomNum].man; i++) {
						if (roomMem[randRoomNum].player[randMan].updateData[i].state == 1) //1 is the value of STAT_ALIVE in class Player
							count++;
					}
					//End game if only one player or none is alive
					if (count <= 1) {
						for (int i = 0; i < roomMem[randRoomNum].man; i++) {
							roomMem[randRoomNum].player[i].state = STATE_GAMEOVER;
						}
						roomMem[randRoomNum].state = ROOM_STATE_TERMINATED;
					}
				}

				int first_player;
				int first_bomb;
				DWORD first_timer;
				bool found;
				//Find detonating bombs
				//Find the smallest detonate timer value
				found = true;
				while (found && (livecount[randMan][0] || livecount[randMan][1] || livecount[randMan][2] || livecount[randMan][3])) {
					first_player = -1;
					first_timer = gottime.dwLowDateTime;
					for (int i = 0; i < roomMem[randRoomNum].man; i++) {
						for (int j = 0; j < roomMem[randRoomNum].player[randMan].updateData[i].bombCount; j++) {
							if (roomMem[randRoomNum].player[randMan].updateData[i].updateBomb[j].getTimer() < first_timer 
								&& roomMem[randRoomNum].player[randMan].updateData[i].updateBomb[j].getState() != BOMB_STATE_EXPLODE) 
							{
								first_timer = roomMem[randRoomNum].player[randMan].updateData[i].updateBomb[j].getTimer();
								first_player = i;
								first_bomb = j;
							}
						}
					}

					//Check if found bomb needs to be detonated
					if (first_player != -1) {
						if ((gottime.dwLowDateTime - roomMem[randRoomNum].player[randMan].updateData[first_player].updateBomb[first_bomb].getTimer()) * 1E-7 >= 5
							&& roomMem[randRoomNum].player[randMan].updateData[first_player].updateBomb[first_bomb].getState() != BOMB_STATE_EXPLODE) 
						{
							roomMem[randRoomNum].player[randMan].updateData[first_player].updateBomb[first_bomb].setState(BOMB_STATE_EXPLODE);

							//Find others that need to be detonated
							for (int i = 0; i < roomMem[randRoomNum].man; i++) {
								for (int j = 0; j < roomMem[randRoomNum].player[randMan].updateData[i].bombCount; j++) {
									if ((abs(roomMem[randRoomNum].player[randMan].updateData[i].updateBomb[j].getX() - roomMem[randRoomNum].player[randMan].updateData[first_player].updateBomb[first_bomb].getX() <= roomMem[randRoomNum].player[randMan].updateData[first_player].updateBomb[first_bomb].getPower())
										&& roomMem[randRoomNum].player[randMan].updateData[i].updateBomb[j].getY() == roomMem[randRoomNum].player[randMan].updateData[first_player].updateBomb[first_bomb].getY())
										|| (abs(roomMem[randRoomNum].player[randMan].updateData[i].updateBomb[j].getY() - roomMem[randRoomNum].player[randMan].updateData[first_player].updateBomb[first_bomb].getY() <= roomMem[randRoomNum].player[randMan].updateData[first_player].updateBomb[first_bomb].getPower())
										&& roomMem[randRoomNum].player[randMan].updateData[i].updateBomb[j].getX() == roomMem[randRoomNum].player[randMan].updateData[first_player].updateBomb[first_bomb].getX())
										&& (roomMem[randRoomNum].player[randMan].updateData[i].updateBomb[j].getTimer() - roomMem[randRoomNum].player[randMan].updateData[first_player].updateBomb[first_bomb].getTimer()) * 1E-7 <= 5
										&& roomMem[randRoomNum].player[randMan].updateData[i].updateBomb[j].getState() != BOMB_STATE_EXPLODE)
									{
										roomMem[randRoomNum].player[randMan].updateData[i].updateBomb[j].setTimer(roomMem[randRoomNum].player[randMan].updateData[first_player].updateBomb[first_bomb].getTimer());
									}
								}
							}
						}
						else
							found = false;
					}
					else
						break;
				}

				sendByte = send(socket_array[recv_buffer.order], (char*)&roomMem[randRoomNum].player[randMan], sizeof(CStruct), 0);
				
				for (int i = 0; i < roomMem[randRoomNum].man; i++) {
					livecount[randMan][i] = 0;
					for (int j = 0; j < roomMem[randRoomNum].player[randMan].updateData[i].bombCount; j++) {
						if (roomMem[randRoomNum].player[randMan].updateData[i].updateBomb[j].getState() == BOMB_STATE_NORMAL)
							livecount[randMan][i]++;
					}
				}
				
				for (int i = 0; i < roomMem[randRoomNum].player[randMan].ready; i++) {
					for (int j = 0; j < roomMem[randRoomNum].player[randMan].updateData[i].bombCount; j++) {
						for (int k = 0; k < roomMem[randRoomNum].player[randMan].updateData[i].bombCount; k++) {
							if (roomMem[randRoomNum].player[randMan].updateData[i].updateBomb[k].getState() > roomMem[randRoomNum].player[randMan].updateData[i].updateBomb[k + 1].getState())
								swap(roomMem[randRoomNum].player[randMan].updateData[i].updateBomb[k], roomMem[randRoomNum].player[randMan].updateData[i].updateBomb[k + 1]);
						}
					}
				}

				for (int i = 0; i < roomMem[randRoomNum].man; i++) {
					roomMem[randRoomNum].player[randMan].updateData[i].bombCount = livecount[randMan][i];
				}				

				break;
			}

		}

	}

	//close
	closesocket(hSock);
	WSACleanup();

	system("PAUSE");
	return EXIT_SUCCESS;
}

