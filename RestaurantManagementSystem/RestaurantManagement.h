#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

#define HASH_SIZE 10
#define MAX_MENU_NAME 100
#define MAX_ITEM_LENGTH 256
#define MAX_ORDER_ITEMS 10

typedef enum {

	FREE,
	OCCUPIED,
	PENDING_BILL

} TableStatus;

typedef struct Table {

	int TableNumber;
	TableStatus Status;
	struct Table* Next;

} Table;

typedef struct MenuItem {

	int Id;
	char Name[MAX_MENU_NAME];
	float Price;
	struct MenuItem* Next;

} MenuItem;

typedef enum {

	ORDER_PENDING,
	ORDER_DONE

} OrderStatus;

typedef struct Order {

	int OrderId;
	int TableNumber;
	MenuItem* Items[MAX_ORDER_ITEMS];
	int Quantities[MAX_ORDER_ITEMS];
	int ItemCount;
	OrderStatus Status;
	struct Order* Prev;
	struct Order* Next;

} Order;

typedef struct OrderQueue {

	Order* Front;
	Order* Back;

} OrderQueue;

typedef struct Bill {

	int BillId;
	int TableNumber;
	Order* Orders;
	float GrandTotal;
	struct Bill* Next;

} Bill;

typedef enum {

	EXIT = 0,
	ASSIGN_TABLE = 1,
	CREATE_ORDER,
	PROCESS_ORDER,
	GENERATE_BILL,
	MANAGE_MENU,
	VIEW_BILL,
	SHOW_MENU,
	SHOW_ORDERS,
	SHOW_TABLES

} MenuOption;

typedef enum {

	ITEM_ADD = 1,
	ITEM_LOOKUP,
	ITEM_DELETE,
	ITEM_RETURN

} ManageMenuOption;

void Cleanup(Table* TableList, MenuItem** MenuTable, Order** ListOrder, OrderQueue* QueueOrder, Bill** ListBill);
int Hash(const char* Name);
void InsertMenuItem(MenuItem** MenuTable, int Id, const char* Name, float Price);
void LoadDefaultMenuItems(MenuItem** MenuTable);
MenuItem* LookupMenuItem(MenuItem** MenuTable, const char* Name);
void DeleteMenuItem(MenuItem** MenuTable, const char* Name);
void DisplayMenuItems(MenuItem** MenuTable);
void AddTable(Table** TableList, int TableNumber);
Table* FindFreeTable(Table* TableList);
void DisplayTables(Table* TableList);
void InsertOrder(Order** ListOrder, Order* NewOrder);
void EnqueueOrder(OrderQueue* QueueOrder, Order* NewOrder);
Order* DequeueOrder(OrderQueue* QueueOrder);
void DisplayOrders(Order* ListOrder, Bill* ListBill);
Order* InitializeOrder();
MenuItem* InitializeMenuItem();
Table* InitializeTable();
void TrimNewline(char* InputString);
void ManageMenuItems(MenuItem** MenuTable);
void TakeOrder(MenuItem** MenuTable, Order** ListOrder, OrderQueue* QueueOrder, int* NextOrderID, Table* TableList, int MaxTableNumber);
void ProcessNextOrder(OrderQueue* QueueOrder);
void GenerateBill(Order** ListOrder, Bill** ListBill, int* NextBillID, Table* TableList);
void PrintBillById(Bill* ListBill);
void AssignTable(Table* TableList);
int GetValidChoice();
int MenuValidChoice();
void GetValidWord(char* Output, size_t OutputSize, const char* PromptMessage);
int GetValidNumber(const char* PromptMessage);
int GetValidNumberInRange(int MaxValue, const char* PromptMessage);
float GetValidPrice(const char* PromptMessage);
int GetValidTableCount(const char* PromptMessage);
void PrintMenu();
void PressEnterToContinue();
