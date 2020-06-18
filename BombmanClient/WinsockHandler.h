#pragma once
#ifndef WINSOCK_HANDLER_H
#define WINSOCK_HANDLER_H

#include <Windows.h>
#include <queue>

#include "ConsoleHandler.h"
#include "Bomb.h"
#include "ErrorCodes.h"

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

//#define DEBUG_INFO

#define POS_STATUS 0, 20
#define END_STATUS 80, 20
#define SECT_STATUS POS_STATUS, END_STATUS

#define POS_SEND_X 2
#define POS_RECEIVE_X 55

#define POS_CONNECT 2
#define POS_UPDATE 2
#define POS_DISCONN 11

#define END_SEND_X 25
#define END_RECEIVE_X 75

#define STATE_LOBBY 0
#define STATE_READY 1
#define STATE_INIT 2
#define STATE_PLAYING 3
#define STATE_GAMEOVER 4

#define MAX_BOMB 10

struct data{
	int posX;
	int posY;
	int dir;
	int state;
	int bombCount;
	Bomb updateBomb[MAX_BOMB];
};

struct client{
	int order;		//起始為-1 **Server用資訊
	int ready;		//遊戲房間中人數
	int state;		// 遊戲階段 0:遊戲大廳 1:遊戲中 2:遊戲結束(顯示成績) 
	int ID;			//起始為-1 **Server用資訊
	bool confirm;	//確認回復用
	bool dataChanged;
	data updateData[4];
};

WSADATA wsaData;
SOCKET hServer;

client con2serv(client player)
{
	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
	}

	string ip;
	gotoXY(POS_STATUS);
	cout << "Input server IP:";
	cin >> ip;

	hServer = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

	sockaddr_in saServer;
	saServer.sin_family = AF_INET;
	saServer.sin_port = htons(10000);
	saServer.sin_addr.s_addr = inet_addr(ip.c_str());
	iResult = connect(hServer, (sockaddr*)&saServer, sizeof(sockaddr));
	if (iResult == 0) {
#if defined DEBUG_INFO
		clearSection(POS_SEND_X, POS_CONNECT, END_SEND_X, POS_CONNECT + 9);
		gotoXY(POS_SEND_X, POS_CONNECT);
		cout << "**********************" << endl;
		gotoXY(POS_SEND_X, POS_CONNECT + 1);
		cout << "Connect Sent Data" << endl;
		gotoXY(POS_SEND_X, POS_CONNECT + 2);
		cout << "Order: " << player.order << endl;
		gotoXY(POS_SEND_X, POS_CONNECT + 3);
		cout << "Ready: " << player.ready << endl;
		gotoXY(POS_SEND_X, POS_CONNECT + 4);
		cout << "State: " << player.state << endl;
		gotoXY(POS_SEND_X, POS_CONNECT + 5);
		cout << "ID: " << player.ID << endl;
		gotoXY(POS_SEND_X, POS_CONNECT + 6);
		cout << "Confirm: " << player.confirm << endl;
		gotoXY(POS_SEND_X, POS_CONNECT + 7);
		cout << "**********************";
#endif

		int test = send(hServer, (char*)&player, sizeof(client), 0);
		if (test == sizeof(client))
			test = recv(hServer, (char*)&player, sizeof(client), 0);
		else {
			clearSection(SECT_STATUS);
			gotoXY(POS_STATUS);
			cout << "Sent data length doesn't match the specified length";
			exit(CON2SERV_ERROR);
		}

		if (test != sizeof(client)) {
			clearSection(SECT_STATUS);
			gotoXY(POS_STATUS);
			cout << "Recieved data length doesn't match the specified length";
			exit(CON2SERV_ERROR);
		}

#if defined DEBUG_INFO
		clearSection(POS_RECEIVE_X, POS_CONNECT, END_RECEIVE_X, POS_CONNECT + 9);
		gotoXY(POS_RECEIVE_X, POS_CONNECT);
		cout << "**********************" << endl;
		gotoXY(POS_RECEIVE_X, POS_CONNECT + 1);
		cout << "Connect Receive Data" << endl;
		gotoXY(POS_RECEIVE_X, POS_CONNECT + 2);
		cout << "Order: " << player.order << endl;
		gotoXY(POS_RECEIVE_X, POS_CONNECT + 3);
		cout << "Ready: " << player.ready << endl;
		gotoXY(POS_RECEIVE_X, POS_CONNECT + 4);
		cout << "State: " << player.state << endl;
		gotoXY(POS_RECEIVE_X, POS_CONNECT + 5);
		cout << "ID: " << player.ID << endl;
		gotoXY(POS_RECEIVE_X, POS_CONNECT + 6);
		cout << "Confirm: " << player.confirm << endl;
		gotoXY(POS_RECEIVE_X, POS_CONNECT + 7);
		cout << "**********************" << endl;

//		system("PAUSE");
#endif
	}
	else {
		clearSection(SECT_STATUS);
		gotoXY(POS_STATUS);
		std::cout << "Can't connect to server" << std::endl;
	}

	return player;
}

client closeConn(client player)
{
#if defined DEBUG_INFO
	clearSection(POS_SEND_X, POS_DISCONN, END_SEND_X, POS_DISCONN + 9);
	gotoXY(POS_SEND_X, POS_DISCONN);
	cout << "**********************" << endl;
	gotoXY(POS_SEND_X, POS_DISCONN + 1);
	cout << "Disconnect Sent Data" << endl;
	gotoXY(POS_SEND_X, POS_DISCONN + 2);
	cout << "Order: " << player.order << endl;
	gotoXY(POS_SEND_X, POS_CONNECT + 3);
	cout << "Ready: " << player.ready << endl;
	gotoXY(POS_SEND_X, POS_CONNECT + 4);
	cout << "State: " << player.state << endl;
	gotoXY(POS_SEND_X, POS_DISCONN + 5);
	cout << "ID: " << player.ID << endl;
	gotoXY(POS_SEND_X, POS_DISCONN + 6);
	cout << "Confirm: " << player.confirm << endl;
	gotoXY(POS_SEND_X, POS_DISCONN + 7);
	cout << "**********************";
#endif

	int test = send(hServer, (char*)&player, sizeof(client), 0);
	if (test == sizeof(client))
		test = recv(hServer, (char*)&player, sizeof(client), 0);
	else {
		clearSection(SECT_STATUS);
		gotoXY(POS_STATUS);
		cout << "Sent data length doesn't match the specified length";
		exit(CLOSECONN_ERROR);
	}

	if (test != sizeof(client)) {
		clearSection(SECT_STATUS);
		gotoXY(POS_STATUS);
		cout << "Recieved data length doesn't match the specified length";
		exit(CLOSECONN_ERROR);
	}

#if defined DEBUG_INFO
	clearSection(POS_RECEIVE_X, POS_DISCONN, END_RECEIVE_X, POS_DISCONN + 9);
	gotoXY(POS_RECEIVE_X, POS_DISCONN);
	cout << "**********************" << endl;
	gotoXY(POS_RECEIVE_X, POS_DISCONN + 1);
	cout << "Disconnect Receive Data" << endl;
	gotoXY(POS_RECEIVE_X, POS_DISCONN + 2);
	cout << "Order: " << player.order << endl;
	gotoXY(POS_RECEIVE_X, POS_DISCONN + 3);
	cout << "Ready: " << player.ready << endl;
	gotoXY(POS_RECEIVE_X, POS_DISCONN + 4);
	cout << "State: " << player.state << endl;
	gotoXY(POS_RECEIVE_X, POS_DISCONN + 5);
	cout << "ID: " << player.ID << endl;
	gotoXY(POS_RECEIVE_X, POS_DISCONN + 6);
	cout << "Confirm: " << player.confirm << endl;
	gotoXY(POS_RECEIVE_X, POS_DISCONN + 7);
	cout << "**********************";
#endif

	clearSection(SECT_STATUS);
	gotoXY(POS_STATUS);
	std::cout << "Disconnecting from server" << std::endl;
	closesocket(hServer);
	WSACleanup();
	return player;
}
client updateServ(client player)
{
#if defined DEBUG_INFO
	clearSection(POS_SEND_X, POS_UPDATE, END_SEND_X, POS_UPDATE + 9);
	gotoXY(POS_SEND_X, POS_UPDATE);
	cout << "**********************" << endl;
	gotoXY(POS_SEND_X, POS_UPDATE + 1);
	cout << "Update Sent Data" << endl;
	gotoXY(POS_SEND_X, POS_UPDATE + 2);
	cout << "Order: " << player.order << endl;
	gotoXY(POS_SEND_X, POS_UPDATE + 3);
	cout << "Ready: " << player.ready << endl;
	gotoXY(POS_SEND_X, POS_UPDATE + 4);
	cout << "State: " << player.state << endl;
	gotoXY(POS_SEND_X, POS_UPDATE + 5);
	cout << "ID: " << player.ID << endl;
	gotoXY(POS_SEND_X, POS_UPDATE + 6);
	cout << "Confirm: " << player.confirm << endl;
	gotoXY(POS_SEND_X, POS_UPDATE + 7);
	cout << "Data Changed: " << player.dataChanged << endl;
	gotoXY(POS_SEND_X, POS_UPDATE + 8);
	cout << "P1 Position: " << player.updateData[0].posX << " " << player.updateData[0].posY << " " << player.updateData[0].dir << endl;
	gotoXY(POS_SEND_X, POS_UPDATE + 9);
	cout << "P2 Position: " << player.updateData[1].posX << " " << player.updateData[1].posY << " " << player.updateData[1].dir << endl;
	gotoXY(POS_SEND_X, POS_UPDATE + 10);
	cout << "P3 Position: " << player.updateData[2].posX << " " << player.updateData[2].posY << " " << player.updateData[2].dir << endl;
	gotoXY(POS_SEND_X, POS_UPDATE + 11);
	cout << "P4 Position: " << player.updateData[3].posX << " " << player.updateData[3].posY << " " << player.updateData[3].dir << endl;
	gotoXY(POS_SEND_X, POS_UPDATE + 12);
	cout << "**********************";
#endif

	int test = send(hServer, (char*)&player, sizeof(client), 0);
	if (test == sizeof(client))
		test = recv(hServer, (char*)&player, sizeof(client), 0);
	else {
		clearSection(SECT_STATUS);
		gotoXY(POS_STATUS);
		cout << "Sent data length doesn't match the specified length";
		exit(UPDATESERV_ERROR);
	}

	if (test != sizeof(client)) {
		clearSection(SECT_STATUS);
		gotoXY(POS_STATUS);
		cout << "Recieved data length doesn't match the specified length";
		exit(UPDATESERV_ERROR);
	}

#if defined DEBUG_INFO
	clearSection(POS_RECEIVE_X, POS_UPDATE, END_RECEIVE_X, POS_UPDATE + 9);
	gotoXY(POS_RECEIVE_X, POS_UPDATE);
	cout << "**********************" << endl;
	gotoXY(POS_RECEIVE_X, POS_UPDATE + 1);
	cout << "Update Receive Data" << endl;
	gotoXY(POS_RECEIVE_X, POS_UPDATE + 2);
	cout << "Order: " << player.order << endl;
	gotoXY(POS_RECEIVE_X, POS_UPDATE + 3);
	cout << "Ready: " << player.ready << endl;
	gotoXY(POS_RECEIVE_X, POS_UPDATE + 4);
	cout << "State: " << player.state << endl;
	gotoXY(POS_RECEIVE_X, POS_UPDATE + 5);
	cout << "ID: " << player.ID << endl;
	gotoXY(POS_RECEIVE_X, POS_UPDATE + 6);
	cout << "Confirm: " << player.confirm << endl;
	gotoXY(POS_RECEIVE_X, POS_UPDATE + 7);
	cout << "Data Changed: " << player.dataChanged << endl;
	gotoXY(POS_RECEIVE_X, POS_UPDATE + 8);
	cout << "P1 Position: " << player.updateData[0].posX << " " << player.updateData[0].posY << " " << player.updateData[0].dir << endl;
	gotoXY(POS_RECEIVE_X, POS_UPDATE + 9);
	cout << "P2 Position: " << player.updateData[1].posX << " " << player.updateData[1].posY << " " << player.updateData[1].dir << endl;
	gotoXY(POS_RECEIVE_X, POS_UPDATE + 10);
	cout << "P3 Position: " << player.updateData[2].posX << " " << player.updateData[2].posY << " " << player.updateData[2].dir << endl;
	gotoXY(POS_RECEIVE_X, POS_UPDATE + 11);
	cout << "P4 Position: " << player.updateData[3].posX << " " << player.updateData[3].posY << " " << player.updateData[3].dir << endl;
	gotoXY(POS_RECEIVE_X, POS_UPDATE + 12);
	cout << "**********************";
#endif

	return player;
}

#endif