#include "RestaurantManagement.h"

int main(void) {
	int Choice;
	int NumTables = GetValidTableCount("Enter number of tables in the restaurant: ");

	Table* TableList = NULL;
	for (int count = 1; count <= NumTables; count++) {
		AddTable(&TableList, count);
	}

	MenuItem* MenuTable[HASH_SIZE] = { 0 };
	Order* ListOrder = NULL;
	OrderQueue QueueOrder = { NULL, NULL };
	int NextOrderID = 1;
	Bill* ListBill = NULL;
	int NextBillID = 1;

	LoadDefaultMenuItems(MenuTable);
	system("cls");

	while (1) {
		PrintMenu();
		Choice = GetValidChoice();

		switch ((MenuOption)Choice) {
		case ASSIGN_TABLE:
			AssignTable(TableList);
			break;

		case CREATE_ORDER:
			TakeOrder(MenuTable, &ListOrder, &QueueOrder, &NextOrderID, TableList, NumTables);
			break;

		case PROCESS_ORDER:
			ProcessNextOrder(&QueueOrder);
			break;

		case GENERATE_BILL:
			GenerateBill(&ListOrder, &ListBill, &NextBillID, TableList);
			break;

		case MANAGE_MENU:
			ManageMenuItems(MenuTable);
			break;

		case VIEW_BILL:
			PrintBillById(ListBill);
			break;

		case SHOW_MENU:
			DisplayMenuItems(MenuTable);
			break;

		case SHOW_ORDERS:
			DisplayOrders(ListOrder, ListBill);
			break;

		case SHOW_TABLES:
			DisplayTables(TableList);
			break;

		case EXIT:
			printf("Exiting system. Cleaning up...\n");
			Cleanup(TableList, MenuTable, &ListOrder, &QueueOrder, &ListBill);
			return 0;

		default:
			printf("Invalid choice. Please try again.\n");
		}
		PressEnterToContinue();
	}
	return 0;
}

void Cleanup(Table* TableList, MenuItem** MenuTable, Order** ListOrder, OrderQueue* QueueOrder, Bill** ListBill) {
	if (TableList) {
		Table* Current = TableList->Next;
		while (Current != TableList) {
			Table* NextTable = Current->Next;
			free(Current);
			Current = NextTable;
		}
		free(TableList);
	}

	for (int Count = 0; Count < HASH_SIZE; Count++) {
		MenuItem* MenuItemPtr = MenuTable[Count];
		while (MenuItemPtr) {
			MenuItem* NextMenuItem = MenuItemPtr->Next;
			free(MenuItemPtr);
			MenuItemPtr = NextMenuItem;
		}
		MenuTable[Count] = NULL;
	}

	Order* OrderPtr = *ListOrder;
	while (OrderPtr) {
		Order* NextOrder = OrderPtr->Next;
		free(OrderPtr);
		OrderPtr = NextOrder;
	}
	*ListOrder = NULL;
	QueueOrder->Front = NULL;
	QueueOrder->Back = NULL;

	Bill* BillPtr = *ListBill;
	while (BillPtr) {
		Order* BillOrder = BillPtr->Orders;
		while (BillOrder) {
			Order* NextBillOrder = BillOrder->Next;
			free(BillOrder);
			BillOrder = NextBillOrder;
		}
		Bill* NextBill = BillPtr->Next;
		free(BillPtr);
		BillPtr = NextBill;
	}
	*ListBill = NULL;
}

int Hash(const char* InputName) {
	int Total = 0;
	for (int Index = 0; InputName[Index] != '\0'; Index++) {
		Total += InputName[Index];
	}
	return Total % HASH_SIZE;
}

void InsertMenuItem(MenuItem** MenuTable, int ItemId, const char* ItemName, float ItemPrice) {
	int HashIndex = Hash(ItemName);
	MenuItem* NewMenuItem = InitializeMenuItem();
	NewMenuItem->Id = ItemId;
	strncpy_s(NewMenuItem->Name, sizeof(NewMenuItem->Name), ItemName, MAX_MENU_NAME);
	NewMenuItem->Price = ItemPrice;
	NewMenuItem->Next = MenuTable[HashIndex];
	MenuTable[HashIndex] = NewMenuItem;
	printf("Menu item '%s' added.\n", ItemName);
}

void LoadDefaultMenuItems(MenuItem** MenuTable) {
	InsertMenuItem(MenuTable, 1, "Burger", (float)5.99);
	InsertMenuItem(MenuTable, 2, "Pizza", (float)8.99);
	InsertMenuItem(MenuTable, 3, "Salad", (float)4.99);
	InsertMenuItem(MenuTable, 4, "Soda", (float)1.99);
	InsertMenuItem(MenuTable, 5, "Fries", (float)2.99);
	InsertMenuItem(MenuTable, 6, "Hot Dog", (float)3.99);
}

MenuItem* LookupMenuItem(MenuItem** MenuTable, const char* ItemName) {
	int HashIndex = Hash(ItemName);
	MenuItem* CurrentItem = MenuTable[HashIndex];
	while (CurrentItem != NULL) {
		if (strcmp(CurrentItem->Name, ItemName) == 0)
			return CurrentItem;
		CurrentItem = CurrentItem->Next;
	}
	return NULL;
}

void DeleteMenuItem(MenuItem** MenuTable, const char* ItemName) {
	int HashIndex = Hash(ItemName);
	MenuItem* CurrentItem = MenuTable[HashIndex];
	MenuItem* PreviousItem = NULL;

	while (CurrentItem != NULL) {
		if (strcmp(CurrentItem->Name, ItemName) == 0) {
			if (PreviousItem == NULL)
				MenuTable[HashIndex] = CurrentItem->Next;
			else
				PreviousItem->Next = CurrentItem->Next;

			free(CurrentItem);
			printf("Menu item '%s' deleted.\n", ItemName);
			return;
		}
		PreviousItem = CurrentItem;
		CurrentItem = CurrentItem->Next;
	}

	printf("Menu item '%s' not found.\n", ItemName);
}

void DisplayMenuItems(MenuItem** MenuTable) {
	printf("     --- Menu Items ---\n");
	printf("ID\tName\t\tPrice\n");
	printf("--\t----\t\t-----\n");

	for (int Index = 0; Index < HASH_SIZE; Index++) {
		MenuItem* CurrentItem = MenuTable[Index];
		while (CurrentItem) {
			printf("%d\t%s\t\t$%.2f\n",
				CurrentItem->Id,
				CurrentItem->Name,
				CurrentItem->Price);
			CurrentItem = CurrentItem->Next;
		}
	}

	printf("-----------------------------\n");
}

void AddTable(Table** TableList, int TableNumber) {
	Table* NewTable = InitializeTable();

	NewTable->TableNumber = TableNumber;
	NewTable->Status = FREE;

	if (*TableList == NULL) {
		*TableList = NewTable;
		NewTable->Next = NewTable;
	}
	else {
		Table* CurrentTable = *TableList;
		while (CurrentTable->Next != *TableList) {
			CurrentTable = CurrentTable->Next;
		}
		CurrentTable->Next = NewTable;
		NewTable->Next = *TableList;
	}
}

Table* FindFreeTable(Table* TableList) {
	if (TableList == NULL) {
		return NULL;
	}

	Table* CurrentTable = TableList;
	do {
		if (CurrentTable->Status == FREE) {
			return CurrentTable;
		}
		CurrentTable = CurrentTable->Next;
	} while (CurrentTable != TableList);

	return NULL;
}

void DisplayTables(Table* TableList) {
	if (!TableList) return;

	printf("\n\t--- Table Statuses ---\n");

	Table* CurrentTable = TableList;
	int TableCount = 0;

	do {
		const char* StatusText;
		switch (CurrentTable->Status) {
		case FREE:     StatusText = "Free";     break;
		case OCCUPIED: StatusText = "Occupied"; break;
		default:       StatusText = "Pending";  break;
		}

		printf("T%d [ %s ]  ", CurrentTable->TableNumber, StatusText);

		TableCount++;
		if (TableCount % 3 == 0) {
			printf("\n");
		}

		CurrentTable = CurrentTable->Next;
	} while (CurrentTable != TableList);

	if (TableCount % 3 != 0) {
		printf("\n");
	}
}

void InsertOrder(Order** ListOrder, Order* NewOrder) {
	if (*ListOrder == NULL || NewOrder->OrderId < (*ListOrder)->OrderId) {
		NewOrder->Next = *ListOrder;
		NewOrder->Prev = NULL;

		if (*ListOrder != NULL) {
			(*ListOrder)->Prev = NewOrder;
		}

		*ListOrder = NewOrder;
	}
	else {
		Order* CurrentOrder = *ListOrder;
		while (CurrentOrder->Next != NULL && CurrentOrder->Next->OrderId < NewOrder->OrderId) {
			CurrentOrder = CurrentOrder->Next;
		}

		NewOrder->Next = CurrentOrder->Next;
		NewOrder->Prev = CurrentOrder;

		if (CurrentOrder->Next != NULL) {
			CurrentOrder->Next->Prev = NewOrder;
		}

		CurrentOrder->Next = NewOrder;
	}
}

void EnqueueOrder(OrderQueue* QueueOrder, Order* NewOrder) {
	NewOrder->Next = NULL;
	NewOrder->Prev = NULL;

	if (QueueOrder->Back == NULL) {
		QueueOrder->Front = NewOrder;
		QueueOrder->Back = NewOrder;
	}
	else {
		QueueOrder->Back->Next = NewOrder;
		NewOrder->Prev = QueueOrder->Back;
		QueueOrder->Back = NewOrder;
	}
}

Order* DequeueOrder(OrderQueue* QueueOrder) {
	if (QueueOrder->Front == NULL)
		return NULL;

	Order* RemovedOrder = QueueOrder->Front;
	QueueOrder->Front = QueueOrder->Front->Next;

	if (QueueOrder->Front == NULL)
		QueueOrder->Back = NULL;

	return RemovedOrder;
}

Order* InitializeOrder() {
	Order* NewOrder = (Order*)malloc(sizeof(Order));
	if (NewOrder == NULL) {
		printf("Insufficient Memory");
		exit(EXIT_FAILURE);
	}

	NewOrder->OrderId = 0;
	NewOrder->Next = NULL;
	NewOrder->Prev = NULL;

	return NewOrder;
}

MenuItem* InitializeMenuItem() {
	MenuItem* NewMenuItem = (MenuItem*)malloc(sizeof(MenuItem));
	if (NewMenuItem == NULL) {
		printf("Insufficient Memory");
		exit(EXIT_FAILURE);
	}

	NewMenuItem->Id = 0;
	NewMenuItem->Next = NULL;

	return NewMenuItem;
}

Table* InitializeTable() {
	Table* NewTable = (Table*)malloc(sizeof(Table));
	if (NewTable == NULL) {
		printf("Insufficient Memory");
		exit(EXIT_FAILURE);
	}

	NewTable->TableNumber = 0;
	NewTable->Status = FREE;
	NewTable->Next = NULL;

	return NewTable;
}

void PrintOrderList(const char* Title, Order* OrderList) {
	printf("\n--- %s ---\n", Title);
	if (!OrderList) {
		printf("  (none)\n");
		return;
	}

	for (Order* CurrentOrder = OrderList; CurrentOrder; CurrentOrder = CurrentOrder->Next) {
		printf("Order ID: %d, Table: %d, Status: %s\n",
			CurrentOrder->OrderId,
			CurrentOrder->TableNumber,
			CurrentOrder->Status == ORDER_PENDING ? "Pending" : "Done");

		for (int Index = 0; Index < CurrentOrder->ItemCount; Index++) {
			printf("    %s x%d\n",
				CurrentOrder->Items[Index]->Name,
				CurrentOrder->Quantities[Index]);
		}
	}
}

void PrintBillList(Bill* ListBill) {
	printf("\n--- Billed Orders ---\n");

	if (!ListBill) {
		printf("  (none)\n");
		return;
	}

	for (Bill* CurrentBill = ListBill; CurrentBill; CurrentBill = CurrentBill->Next) {
		printf("\n=== Bill ID: %d (Table %d) ===\n",
			CurrentBill->BillId,
			CurrentBill->TableNumber);
		PrintOrderList("Orders in this Bill", CurrentBill->Orders);
	}
}

void DisplayOrders(Order* ListOrder, Bill* ListBill) {
	PrintOrderList("Active Orders", ListOrder);
	PrintBillList(ListBill);
	printf("\n");
}

void TrimNewline(char* InputString) {
	int Length = (int)strlen(InputString);
	if (Length > 0 && InputString[Length - 1] == '\n') {
		InputString[Length - 1] = '\0';
	}
}

void AssignTable(Table* TableList) {
	Table* FreeTable = FindFreeTable(TableList);
	if (FreeTable != NULL) {
		FreeTable->Status = OCCUPIED;
		printf("Table %d assigned to customer.\n", FreeTable->TableNumber);
	}
	else {
		printf("No free table available.\n");
	}
}

MenuItem* GetValidMenuItem(MenuItem** MenuTable, const char* PromptMessage) {
	char ItemName[MAX_MENU_NAME];
	MenuItem* FoundItem;

	do {
		GetValidWord(ItemName, sizeof(ItemName), PromptMessage);
		TrimNewline(ItemName);
		FoundItem = LookupMenuItem(MenuTable, ItemName);

		if (FoundItem == NULL) {
			printf("'%s' is not on the menu. Please try again.\n", ItemName);
		}
	} while (FoundItem == NULL);

	return FoundItem;
}

int GetValidItemName(char* ItemName, const char* PromptMessage) {
	while (1) {
		printf("%s", PromptMessage);
		if (fgets(ItemName, MAX_MENU_NAME, stdin) == NULL) {
			continue;
		}

		ItemName[strcspn(ItemName, "\n")] = '\0';

		if (strcmp(ItemName, "-1") == 0) {
			return 0;
		}

		if (ItemName[0] == '\0' || ItemName[0] == ' ') {
			printf("INVALID INPUT! Please enter a menu item name (letters and spaces only), or -1 to finish.\n");
			continue;
		}

		int IsValid = 1;
		for (int Index = 0; Index < (int)strlen(ItemName); Index++) {
			if (!((ItemName[Index] >= 'A' && ItemName[Index] <= 'Z') ||
				(ItemName[Index] >= 'a' && ItemName[Index] <= 'z') ||
				ItemName[Index] == ' ')) {
				IsValid = 0;
				break;
			}
		}

		if (!IsValid) {
			printf("INVALID INPUT! Please enter a menu item name (letters and spaces only), or -1 to finish.\n");
			continue;
		}

		return 1;
	}
}

void TakeOrder(MenuItem** MenuTable, Order** ListOrder, OrderQueue* QueueOrder, int* NextOrderID, Table* TableList, int MaxTableNumber) {
	system("cls");

	DisplayMenuItems(MenuTable);

	Order* NewOrder = InitializeOrder();
	NewOrder->Status = ORDER_PENDING;
	NewOrder->ItemCount = 0;

	printf("Enter each item and quantity. To finish, enter item name \"-1\".\n");

	while (NewOrder->ItemCount < MAX_ORDER_ITEMS) {
		char ItemName[MAX_MENU_NAME];
		int IsValid = GetValidItemName(ItemName, "\nItem name (-1 to finish): ");
		if (IsValid == 0) {
			break;
		}

		MenuItem* MenuItemFound = LookupMenuItem(MenuTable, ItemName);
		if (!MenuItemFound) {
			printf("'%s' is not on the menu. Try again.\n", ItemName);
			continue;
		}

		int Quantity = 0;
		do {
			Quantity = GetValidNumber("Quantity: ");
			if (Quantity <= 0) {
				printf("Quantity must be greater than 0.\n");
			}
		} while (Quantity <= 0);

		int ItemIndex = NewOrder->ItemCount++;
		NewOrder->Items[ItemIndex] = MenuItemFound;
		NewOrder->Quantities[ItemIndex] = Quantity;
	}

	if (NewOrder->ItemCount == 0) {
		printf("\nNo items entered; order cancelled.\n");
		free(NewOrder);
		return;
	}

	int TableNumber = GetValidNumberInRange(MaxTableNumber, "Enter table number for order: ");

	Table* CurrentTable = TableList;
	int Found = 0;

	do {
		if (CurrentTable->TableNumber == TableNumber) {
			Found = 1;
			if (CurrentTable->Status == FREE) {
				CurrentTable->Status = OCCUPIED;
				printf("Table %d was free and is now assigned.\n", TableNumber);
			}
			break;
		}
		CurrentTable = CurrentTable->Next;
	} while (CurrentTable != TableList);

	if (!Found) {
		printf("Invalid table number; order cancelled.\n");
		free(NewOrder);
		return;
	}

	NewOrder->OrderId = (*NextOrderID)++;
	NewOrder->TableNumber = TableNumber;

	InsertOrder(ListOrder, NewOrder);
	EnqueueOrder(QueueOrder, NewOrder);

	printf("\nOrder %d taken for table %d:\n", NewOrder->OrderId, TableNumber);
	for (int Index = 0; Index < NewOrder->ItemCount; Index++) {
		printf("  %s x%d\n",
			NewOrder->Items[Index]->Name,
			NewOrder->Quantities[Index]);
	}
}

void ProcessNextOrder(OrderQueue* QueueOrder) {
	Order* NextOrder = DequeueOrder(QueueOrder);
	if (NextOrder == NULL) {
		printf("No orders to process.\n");
		return;
	}

	NextOrder->Status = ORDER_DONE;
	printf("Order %d for table %d has been processed and marked as done.\n",
		NextOrder->OrderId, NextOrder->TableNumber);
}

void GenerateBill(Order** ListOrder, Bill** ListBill, int* NextBillID, Table* TableList) {
	int TableNumber = GetValidNumber("Enter table number to generate bill: ");
	int HasProcessedOrder = 0;
	int HasAnyOrderForTable = 0;
	int TableExists = 0;

	if (TableList) {
		Table* CurrentTable = TableList;
		do {
			if (CurrentTable->TableNumber == TableNumber) {
				TableExists = 1;
				break;
			}
			CurrentTable = CurrentTable->Next;
		} while (CurrentTable != TableList);
	}

	if (!TableExists) {
		printf("Table %d does not exist.\n", TableNumber);
		return;
	}

	for (Order* CurrentOrder = *ListOrder; CurrentOrder; CurrentOrder = CurrentOrder->Next) {
		if (CurrentOrder->TableNumber == TableNumber) {
			HasAnyOrderForTable = 1;
			if (CurrentOrder->Status == ORDER_PENDING) {
				printf("Please process all orders before billing.\n");
				return;
			}
			if (CurrentOrder->Status == ORDER_DONE) {
				HasProcessedOrder = 1;
			}
		}
	}

	if (!HasAnyOrderForTable || !HasProcessedOrder) {
		printf("No completed orders found for table %d.\n", TableNumber);
		return;
	}

	Bill* NewBill = (Bill*)malloc(sizeof(Bill));
	if (!NewBill) {
		printf("Error: could not allocate memory for bill.\n");
		return;
	}

	NewBill->BillId = (*NextBillID)++;
	NewBill->TableNumber = TableNumber;
	NewBill->Orders = NULL;
	NewBill->GrandTotal = 0.0f;
	NewBill->Next = NULL;

	printf("\n===== BILL #%d FOR TABLE %d =====\n", NewBill->BillId, TableNumber);

	Order* CurrentOrder = *ListOrder;
	while (CurrentOrder) {
		Order* NextOrder = CurrentOrder->Next;
		if (CurrentOrder->TableNumber == TableNumber && CurrentOrder->Status == ORDER_DONE) {
			printf("\n-- Order #%d --\n", CurrentOrder->OrderId);
			printf("Item\tPrice\tQty\tSubtotal\n");
			printf("--------------------------------\n");

			float OrderTotal = 0.0f;
			for (int Index = 0; Index < CurrentOrder->ItemCount; Index++) {
				float LineTotal = CurrentOrder->Items[Index]->Price * CurrentOrder->Quantities[Index];
				OrderTotal += LineTotal;

				printf("%s\t%.2f\t%d\t%.2f\n",
					CurrentOrder->Items[Index]->Name,
					CurrentOrder->Items[Index]->Price,
					CurrentOrder->Quantities[Index],
					LineTotal);
			}

			printf("--------------------------------\n");
			printf("Order Total:\t\t\t%.2f\n", OrderTotal);
			NewBill->GrandTotal += OrderTotal;

			if (CurrentOrder->Prev) CurrentOrder->Prev->Next = CurrentOrder->Next;
			if (CurrentOrder->Next) CurrentOrder->Next->Prev = CurrentOrder->Prev;
			if (CurrentOrder == *ListOrder) *ListOrder = CurrentOrder->Next;

			CurrentOrder->Prev = NULL;
			CurrentOrder->Next = NewBill->Orders;
			if (NewBill->Orders) NewBill->Orders->Prev = CurrentOrder;
			NewBill->Orders = CurrentOrder;
		}
		CurrentOrder = NextOrder;
	}

	printf("\nGRAND TOTAL:\t\t\t%.2f\n", NewBill->GrandTotal);
	printf("===============================\n\n");

	if (!*ListBill) {
		*ListBill = NewBill;
	}
	else {
		Bill* Tail = *ListBill;
		while (Tail->Next) Tail = Tail->Next;
		Tail->Next = NewBill;
	}

	for (Table* CurrentTable = TableList; ; CurrentTable = CurrentTable->Next) {
		if (CurrentTable->TableNumber == TableNumber) {
			CurrentTable->Status = FREE;
			break;
		}
		if (CurrentTable->Next == TableList) break;
	}

	printf("Table %d is now free.\n\n", TableNumber);
}

void PrintBillById(Bill* ListBill) {
	if (ListBill == NULL) {
		printf("\nNo bills have been generated yet.\n");
		return;
	}

	int BillId = GetValidNumber("Enter Bill ID to view: ");

	for (Bill* CurrentBill = ListBill; CurrentBill; CurrentBill = CurrentBill->Next) {
		if (CurrentBill->BillId == BillId) {
			printf("\n===== BILL #%d FOR TABLE %d =====\n",
				CurrentBill->BillId, CurrentBill->TableNumber);
			printf("Item\tPrice\tQty\tSubtotal\n");
			printf("--------------------------------\n");

			for (Order* BillOrder = CurrentBill->Orders; BillOrder; BillOrder = BillOrder->Next) {
				for (int Index = 0; Index < BillOrder->ItemCount; Index++) {
					float LineTotal = BillOrder->Items[Index]->Price * BillOrder->Quantities[Index];
					printf("%s\t%.2f\t%d\t%.2f\n",
						BillOrder->Items[Index]->Name,
						BillOrder->Items[Index]->Price,
						BillOrder->Quantities[Index],
						LineTotal);
				}
			}

			printf("--------------------------------\n");
			printf("GRAND TOTAL:\t\t\t%.2f\n", CurrentBill->GrandTotal);
			printf("===============================\n\n");
			return;
		}
	}

	printf("Bill ID %d not found.\n", BillId);
}

void ManageMenuItems(MenuItem** MenuTable) {
	int UserChoice;
	char ItemName[MAX_MENU_NAME];
	int ItemId;
	float ItemPrice;

	while (1) {
		system("cls");
		printf("--- Manage Menu Items ---\n");
		printf("1. Add Menu Item\n");
		printf("2. Lookup Menu Item\n");
		printf("3. Delete Menu Item\n");
		printf("4. Return to the Main Menu\n");

		UserChoice = MenuValidChoice();

		switch ((ManageMenuOption)UserChoice) {
		case ITEM_ADD:
			ItemId = GetValidNumber("Enter Menu Item ID: ");
			GetValidWord(ItemName, sizeof(ItemName), "Enter Menu Item Name: ");
			TrimNewline(ItemName);
			ItemPrice = GetValidPrice("Enter Price: ");
			InsertMenuItem(MenuTable, ItemId, ItemName, ItemPrice);
			break;

		case ITEM_LOOKUP:
			GetValidWord(ItemName, sizeof(ItemName), "Enter Menu Item Name to lookup: ");
			TrimNewline(ItemName);
			{
				MenuItem* FoundItem = LookupMenuItem(MenuTable, ItemName);
				if (FoundItem != NULL) {
					printf("Found: ID: %d, Name: %s, Price: $%.2f\n",
						FoundItem->Id, FoundItem->Name, FoundItem->Price);
				}
				else {
					printf("Menu item not found.\n");
				}
			}
			break;

		case ITEM_DELETE:
			GetValidWord(ItemName, sizeof(ItemName), "Enter Menu Item Name to delete: ");
			TrimNewline(ItemName);
			DeleteMenuItem(MenuTable, ItemName);
			break;

		case ITEM_RETURN:
			printf("\nReturning to Main Menu\n");
			return;

		default:
			printf("Invalid choice.\n");
			break;
		}

		PressEnterToContinue();
	}
}

int GetValidChoice() {
	char InputChoice[MAX_ITEM_LENGTH];
	int Choice;

	while (1) {
		printf("Enter your choice: ");

		if (fgets(InputChoice, sizeof(InputChoice), stdin) == NULL) {
			continue;
		}
		InputChoice[strcspn(InputChoice, "\n")] = '\0';

		if ((int)strlen(InputChoice) == 0) {
			printf("\nINVALID INPUT! Please enter a single digit (0-9)\n");
			continue;
		}

		if (strlen(InputChoice) != 1 || !(InputChoice[0] >= '0' && InputChoice[0] <= '9')) {
			printf("\nINVALID INPUT! Please enter only a single digit (0-9).\n");
			continue;
		}

		if (sscanf_s(InputChoice, "%d", &Choice) == 1) {
			return Choice;
		}
	}
	return 0;
}

int MenuValidChoice() {
	char InputChoice[MAX_ITEM_LENGTH];
	int Choice;

	while (1) {
		printf("Enter your choice: ");

		if (fgets(InputChoice, sizeof(InputChoice), stdin) == NULL) {
			continue;
		}
		InputChoice[strcspn(InputChoice, "\n")] = '\0';

		if ((int)strlen(InputChoice) == 0) {
			printf("\nINVALID INPUT! Please enter a single digit (1-4)\n");
			continue;
		}

		if (strlen(InputChoice) != 1 || !(InputChoice[0] >= '1' && InputChoice[0] <= '4')) {
			printf("\nINVALID INPUT! Please enter only a single digit (1-4).\n");
			continue;
		}

		if (sscanf_s(InputChoice, "%d", &Choice) == 1) {
			return Choice;
		}
	}
	return 0;
}

void GetValidWord(char* InputWord, size_t InputWordSize, const char* PromptMessage) {
	int ValidWord;
	do {
		ValidWord = 1;
		printf("%s", PromptMessage);
		if (fgets(InputWord, (int)InputWordSize, stdin) == NULL) {
			printf("\nINVALID INPUT! Please try again.\n");
			ValidWord = 0;
			continue;
		}

		InputWord[strcspn(InputWord, "\n")] = '\0';

		if ((int)strlen(InputWord) == 0 || InputWord[0] == ' ') {
			printf("\nINVALID INPUT! Please enter a valid word (letters only, no numbers or symbols).\n");
			ValidWord = 0;
			continue;
		}

		for (int count = 0; count < (int)strlen(InputWord); count++) {
			if (!((InputWord[count] >= 'A' && InputWord[count] <= 'Z') || (InputWord[count] >= 'a' && InputWord[count] <= 'z') ||
				InputWord[count] == ' ')) {
				ValidWord = 0;
				break;
			}
		}

		if (ValidWord == 0) {
			printf("\nINVALID INPUT! Please enter a valid word (letters only, no numbers or symbols).\n");
		}
	} while (ValidWord == 0);
}

int GetValidNumber(const char* PromptMessage) {
	char InputNumber[MAX_ITEM_LENGTH];
	int ValidNumber, Number;

	do {
		ValidNumber = 1;
		printf("%s", PromptMessage);
		fgets(InputNumber, MAX_ITEM_LENGTH, stdin);

		InputNumber[strcspn(InputNumber, "\n")] = '\0';

		if (strlen(InputNumber) == 0) {
			printf("\nINVALID INPUT! Please enter a valid number (digits only, no letters or symbols).\n");
			ValidNumber = 0;
			continue;
		}

		for (int Count = 0; Count < (int)strlen(InputNumber); Count++) {
			if (!(InputNumber[Count] >= '0' && InputNumber[Count] <= '9')) {
				ValidNumber = 0;
				break;
			}
		}

		if (ValidNumber == 0) {
			printf("\nINVALID INPUT! Please enter a valid number (digits only, no letters or symbols).\n");
		}
	} while (ValidNumber == 0);

	sscanf_s(InputNumber, "%d", &Number);
	return Number;
}

int GetValidNumberInRange(int MaxValue, const char* PromptMessage) {
	char InputBuffer[MAX_ITEM_LENGTH];
	int IsValid, ParsedNumber;

	do {
		IsValid = 1;
		printf("%s", PromptMessage);

		if (fgets(InputBuffer, sizeof(InputBuffer), stdin) == NULL) {
			IsValid = 0;
			continue;
		}

		InputBuffer[strcspn(InputBuffer, "\n")] = '\0';

		if (InputBuffer[0] == '\0') {
			IsValid = 0;
		}
		else {
			for (int Index = 0; InputBuffer[Index] != '\0'; Index++) {
				if (InputBuffer[Index] < '0' || InputBuffer[Index] > '9') {
					IsValid = 0;
					break;
				}
			}
		}

		if (!IsValid) {
			printf("INVALID INPUT! Please enter a table number between 1 and %d.\n", MaxValue);
			continue;
		}

		ParsedNumber = atoi(InputBuffer);
		if (ParsedNumber < 1 || ParsedNumber > MaxValue) {
			printf("INVALID RANGE! Please enter a table number between 1 and %d.\n", MaxValue);
			IsValid = 0;
		}

	} while (!IsValid);

	return ParsedNumber;
}

float GetValidPrice(const char* PromptMessage) {
	char InputBuffer[MAX_ITEM_LENGTH];
	int IsValidPrice;
	float ParsedPrice;
	int DecimalPointCount;

	do {
		IsValidPrice = 1;
		DecimalPointCount = 0;

		printf("%s", PromptMessage);
		fgets(InputBuffer, MAX_ITEM_LENGTH, stdin);
		InputBuffer[strcspn(InputBuffer, "\n")] = '\0';

		if (strlen(InputBuffer) == 0) {
			printf("\nINVALID INPUT! Please enter a valid price (digits and one decimal point allowed).\n");
			IsValidPrice = 0;
			continue;
		}

		for (int Index = 0; Index < (int)strlen(InputBuffer); Index++) {
			if (InputBuffer[Index] == '.') {
				DecimalPointCount++;
				if (DecimalPointCount > 1) {
					IsValidPrice = 0;
					break;
				}
			}
			else if (InputBuffer[Index] < '0' || InputBuffer[Index] > '9') {
				IsValidPrice = 0;
				break;
			}
		}

		if (!IsValidPrice) {
			printf("\nINVALID INPUT! Please enter a valid price (digits and one decimal point allowed).\n");
		}

	} while (!IsValidPrice);

	sscanf_s(InputBuffer, "%f", &ParsedPrice);
	return ParsedPrice;
}

int GetValidTableCount(const char* PromptMessage) {
	char InputBuffer[MAX_ITEM_LENGTH];
	int IsValidInput, TableCount;

	do {
		IsValidInput = 1;

		printf("%s", PromptMessage);
		fgets(InputBuffer, MAX_ITEM_LENGTH, stdin);
		InputBuffer[strcspn(InputBuffer, "\n")] = '\0';

		if (strlen(InputBuffer) == 0) {
			printf("\nINVALID INPUT! Please enter a valid number of tables (digits only, no letters or symbols).\n");
			IsValidInput = 0;
			continue;
		}

		for (int Index = 0; Index < (int)strlen(InputBuffer); Index++) {
			if (InputBuffer[Index] < '0' || InputBuffer[Index] > '9') {
				IsValidInput = 0;
				break;
			}
		}

		if (!IsValidInput) {
			printf("\nINVALID INPUT! Please enter a valid number of tables (digits only, no letters or symbols).\n");
			continue;
		}

		sscanf_s(InputBuffer, "%d", &TableCount);

		if (TableCount <= 1) {
			printf("\nINVALID INPUT! Number of tables must be greater than 1.\n");
			IsValidInput = 0;
		}

	} while (!IsValidInput);

	return TableCount;
}

void PrintMenu() {
	printf("--- Restaurant Management System ---\n");
	printf("1. Assign a Table to Customer\n");
	printf("2. Create a New Order\n");
	printf("3. Process Next Pending Order\n");
	printf("4. Generate Bill & Release Table\n");
	printf("5. Manage Menu Items\n");
	printf("6. View Bill by ID\n");
	printf("7. Show All Menu Items\n");
	printf("8. Show All Orders\n");
	printf("9. Show Table Statuses\n");
	printf("0. Exit Application\n");
}

void PressEnterToContinue() {
	printf("\nPRESS ENTER TO CONTINUE...");

	while (_getch() != '\r' && _getch() != '\n');

	system("cls");
}
