
#include <iostream>
#include <string>
#include <ctime>
#include <cctype>
#include <algorithm>
#include <limits>
#include <fstream>
#include <sstream>
#include <iomanip>
#include<queue>
using namespace std;
// ---------------------- Structures ----------------------
struct Book {
	string bookID;
	string title;
	string author;
	bool isIssued = false;
	time_t issueDate = 0;
	time_t returnDate = 0;
	Book* next = nullptr;
	Book* prev = nullptr;
};
struct User {
	string username;
	string password;
	bool isPremium = false;
	time_t premiumExpiry = 0;
	User* next = nullptr;
	User* prev = nullptr;
};
struct Member {
	string memberID;
	string name;
	bool isPremium = false;
	time_t premiumExpiry = 0;
	Member* next = nullptr;
	Member* prev = nullptr;
};
struct IssuedBook {
	string bookID;
	string memberID;
	time_t issueDate = 0;
	IssuedBook* next = nullptr;
	IssuedBook* prev = nullptr;
};
// ---------------------- Global Pointers ----------------------
Book* bookHead = nullptr;
Book* bookTail = nullptr;
Member* memberHead = nullptr;
Member* memberTail = nullptr;
IssuedBook* issuedHead = nullptr;
IssuedBook* issuedTail = nullptr;
User* userHead = nullptr;
User* userTail = nullptr;
struct CompareBookTitle {
	bool operator()(Book* a, Book* b) {
		return a->title > b->title; // smallest title comes out first
	}
};
// ---------------------- Helper Functions ----------------------
string timeToString(time_t time) {
	if (time == 0) return "Not set";
	char buffer[80];
	struct tm timeinfo;
	localtime_s(&timeinfo, &time);
	strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
	return string(buffer);
}
time_t stringToTime(const string& timeStr) {
	if (timeStr == "Not set") return 0;
	struct tm tm = { 0 };
	istringstream iss(timeStr);
	iss >> get_time(&tm, "%Y-%m-%d %H:%M:%S");
	return mktime(&tm);
}
bool caseInsensitiveCompare(const string& str1, const string& str2) {
	if (str1.length() != str2.length()) return false;
	for (size_t i = 0; i < str1.length(); ++i) {
		if (tolower(str1[i]) != tolower(str2[i])) return false;
	}
	return true;
}
// ---------------------- File Handling Functions ----------------------
void saveBooksToFile() {
	ofstream outFile("books.txt");
	Book* temp = bookHead;
	while (temp) {
		outFile << "Book ID: " << temp->bookID << "\n"
			<< "Title: " << temp->title << "\n"
			<< "Author: " << temp->author << "\n"
			<< "Status: " << (temp->isIssued ? "Issued" : "Available") << "\n";
		if (temp->isIssued) {
			outFile << "Issued Date: " << timeToString(temp->issueDate) << "\n"
				<< "Return Date: " << timeToString(temp->returnDate) << "\n";
		}
		outFile << "-----------------\n";
		temp = temp->next;
	}
	outFile.close();
}
void loadBooksFromFile() {
	ifstream inFile("books.txt");
	if (!inFile) return;
	// Clear existing books
	while (bookHead) {
		Book* temp = bookHead;
		bookHead = bookHead->next;
		delete temp;
	}
	bookHead = bookTail = nullptr;
	string line;
	Book* currentBook = nullptr;
	bool readingBook = false;
	while (getline(inFile, line)) {
		if (line.find("Book ID: ") != string::npos) {
			if (currentBook) {
				// Add the previous book to the list
				currentBook->next = bookHead;
				if (bookHead) bookHead->prev = currentBook;
				else bookTail = currentBook;
				bookHead = currentBook;
			}
			currentBook = new Book();
			currentBook->bookID = line.substr(9);
			readingBook = true;
		}
		else if (readingBook && currentBook) {
			if (line.find("Title: ") != string::npos) {
				currentBook->title = line.substr(7);
			}
			else if (line.find("Author: ") != string::npos) {
				currentBook->author = line.substr(8);
			}
			else if (line.find("Status: ") != string::npos) {
				currentBook->isIssued = (line.substr(8) == "Issued");
			}
			else if (line.find("Issued Date: ") != string::npos) {
				currentBook->issueDate = stringToTime(line.substr(13));
			}
			else if (line.find("Return Date: ") != string::npos) {
				currentBook->returnDate = stringToTime(line.substr(13));
			}
			else if (line == "-----------------") {
				readingBook = false;
			}
		}
	}
	// Add the last book to the list
	if (currentBook) {
		currentBook->next = bookHead;
		if (bookHead) bookHead->prev = currentBook;
		else bookTail = currentBook;
		bookHead = currentBook;
	}
	inFile.close();
}
void saveMembersToFile() {
	ofstream outFile("members.txt");
	Member* temp = memberHead;
	while (temp) {
		outFile << "Member ID: " << temp->memberID << "\n"
			<< "Name: " << temp->name << "\n"
			<< "Type: " << (temp->isPremium ? "Premium" : "Normal") << "\n";
		if (temp->isPremium) {
			outFile << "Premium Expiry: " << timeToString(temp->premiumExpiry) << "\n";
		}
		outFile << "-----------------\n";
		temp = temp->next;
	}
	outFile.close();
}
void loadMembersFromFile() {
	ifstream inFile("members.txt");
	if (!inFile) return;
	// Clear existing members
	while (memberHead) {
		Member* temp = memberHead;
		memberHead = memberHead->next;
		delete temp;
	}
	memberHead = memberTail = nullptr;
	string line;
	Member* currentMember = nullptr;
	bool readingMember = false;
	while (getline(inFile, line)) {
		if (line.find("Member ID: ") != string::npos) {
			if (currentMember) {
				// Add the previous member to the list
				currentMember->next = memberHead;
				if (memberHead) memberHead->prev = currentMember;
				else memberTail = currentMember;
				memberHead = currentMember;
			}
			currentMember = new Member();
			currentMember->memberID = line.substr(11);
			readingMember = true;
		}
		else if (readingMember && currentMember) {
			if (line.find("Name: ") != string::npos) {
				currentMember->name = line.substr(6);
			}
			else if (line.find("Type: ") != string::npos) {
				currentMember->isPremium = (line.substr(6) == "Premium");
			}
			else if (line.find("Premium Expiry: ") != string::npos) {
				currentMember->premiumExpiry = stringToTime(line.substr(16));
			}
			else if (line == "-----------------") {
				readingMember = false;
			}
		}
	}
	// Add the last member to the list
	if (currentMember) {
		currentMember->next = memberHead;
		if (memberHead) memberHead->prev = currentMember;
		else memberTail = currentMember;
		memberHead = currentMember;
	}
	inFile.close();
}
void saveUsersToFile() {
	ofstream outFile("users.txt");
	User* temp = userHead;
	while (temp) {
		outFile << "Username: " << temp->username << "\n"
			<< "Password: " << temp->password << "\n"
			<< "Type: " << (temp->isPremium ? "Premium" : "Normal") << "\n";
		if (temp->isPremium) {
			outFile << "Premium Expiry: " << timeToString(temp->premiumExpiry) << "\n";
		}
		outFile << "-----------------\n";
		temp = temp->next;
	}
	outFile.close();
}
void loadUsersFromFile() {
	ifstream inFile("users.txt");
	if (!inFile) return;
	// Clear existing users
	while (userHead) {
		User* temp = userHead;
		userHead = userHead->next;
		delete temp;
	}
	userHead = userTail = nullptr;
	string line;
	User* currentUser = nullptr;
	bool readingUser = false;
	while (getline(inFile, line)) {
		if (line.find("Username: ") != string::npos) {
			if (currentUser) {
				// Add the previous user to the list
				currentUser->next = userHead;
				if (userHead) userHead->prev = currentUser;
				else userTail = currentUser;
				userHead = currentUser;
			}
			currentUser = new User();
			currentUser->username = line.substr(10);
			readingUser = true;
		}
		else if (readingUser && currentUser) {
			if (line.find("Password: ") != string::npos) {
				currentUser->password = line.substr(10);
			}
			else if (line.find("Type: ") != string::npos) {
				currentUser->isPremium = (line.substr(6) == "Premium");
			}
			else if (line.find("Premium Expiry: ") != string::npos) {
				currentUser->premiumExpiry = stringToTime(line.substr(16));
			}
			else if (line == "-----------------") {
				readingUser = false;
			}
		}
	}
	// Add the last user to the list
	if (currentUser) {
		currentUser->next = userHead;
		if (userHead) userHead->prev = currentUser;
		else userTail = currentUser;
		userHead = currentUser;
	}
	inFile.close();
}
void saveIssuedBooksToFile() {
	ofstream outFile("issued_books.txt");
	IssuedBook* temp = issuedHead;
	while (temp) {
		outFile << "Book ID: " << temp->bookID << "\n"
			<< "Member ID: " << temp->memberID << "\n"
			<< "Issue Date: " << timeToString(temp->issueDate) << "\n"
			<< "-----------------\n";
		temp = temp->next;
	}
	outFile.close();
}
void loadIssuedBooksFromFile() {
	ifstream inFile("issued_books.txt");
	if (!inFile) return;
	// Clear existing issued books
	while (issuedHead) {
		IssuedBook* temp = issuedHead;
		issuedHead = issuedHead->next;
		delete temp;
	}
	issuedHead = issuedTail = nullptr;
	string line;
	IssuedBook* currentIssue = nullptr;
	bool readingIssue = false;
	while (getline(inFile, line)) {
		if (line.find("Book ID: ") != string::npos) {
			if (currentIssue) {
				// Add the previous issue to the list
				currentIssue->next = issuedHead;
				if (issuedHead) issuedHead->prev = currentIssue;
				else issuedTail = currentIssue;
				issuedHead = currentIssue;
			}
			currentIssue = new IssuedBook();
			currentIssue->bookID = line.substr(9);
			readingIssue = true;
		}
		else if (readingIssue && currentIssue) {
			if (line.find("Member ID: ") != string::npos) {
				currentIssue->memberID = line.substr(11);
			}
			else if (line.find("Issue Date: ") != string::npos) {
				currentIssue->issueDate = stringToTime(line.substr(12));
			}
			else if (line == "-----------------") {
				readingIssue = false;
			}
		}
	}
	// Add the last issue to the list
	if (currentIssue) {
		currentIssue->next = issuedHead;
		if (issuedHead) issuedHead->prev = currentIssue;
		else issuedTail = currentIssue;
		issuedHead = currentIssue;
	}
	inFile.close();
}
// ---------------------- Book Functions ----------------------
void addBook() {
	Book* newBook = new Book;
	cout << "Enter Book ID: ";
	cin >> newBook->bookID;
	Book* temp = bookHead;
	while (temp) {
		if (caseInsensitiveCompare(temp->bookID, newBook->bookID)) {
			cout << "Error: Book ID already exists!\n";
			delete newBook;
			return;
		}
		temp = temp->next;
	}
	cout << "Enter Title: ";
	cin.ignore();
	getline(cin, newBook->title);
	cout << "Enter Author: ";
	getline(cin, newBook->author);
	newBook->next = bookHead;
	if (bookHead) bookHead->prev = newBook;
	else bookTail = newBook;
	bookHead = newBook;
	saveBooksToFile();
	cout << "Book added successfully!\n";
}
void viewBooks() {
	// If there are no books at all, just notify and return
	if (!bookHead) {
		cout << "No books in the library.\n";
		return;
	}
	// Build a min-heap (priority_queue) of all Book* sorted by title
	priority_queue<Book*, vector<Book*>, CompareBookTitle> pq;
	Book* temp = bookHead;
	while (temp) {
		pq.push(temp);
		temp = temp->next;
	}
	cout << "\n--- Book List (sorted by Title) ---\n";
	while (!pq.empty()) {
		Book* b = pq.top();
		pq.pop();
		cout << "ID: " << b->bookID
			<< ", Title: " << b->title
			<< ", Author: " << b->author
			<< ", Status: "
			<< (b->isIssued
				? "Issued (on " + timeToString(b->issueDate) +
				", Return by: " + timeToString(b->returnDate) + ")"
				: "Available")
			<< "\n";
	}
}
// ---------------------- Member Functions ----------------------
void addMember() {
	Member* newMember = new Member;
	cout << "Enter Member ID: ";
	cin >> newMember->memberID;
	Member* temp = memberHead;
	while (temp) {
		if (caseInsensitiveCompare(temp->memberID, newMember->memberID)) {
			cout << "Error: Member ID already exists!\n";
			delete newMember;
			return;
		}
		temp = temp->next;
	}
	cout << "Enter Name: ";
	cin.ignore();
	getline(cin, newMember->name);
	cout << "Premium member? (1 for Yes, 0 for No): ";
	int premiumChoice;
	cin >> premiumChoice;
	newMember->isPremium = (premiumChoice == 1);
	if (newMember->isPremium) {
		newMember->premiumExpiry = time(0) + (365 * 24 * 60 * 60);
		cout << "Premium membership added for 1 year!\n";
	}
	newMember->next = memberHead;
	if (memberHead) memberHead->prev = newMember;
	else memberTail = newMember;
	memberHead = newMember;
	saveMembersToFile();
	cout << "Member added successfully!\n";
}
void viewMembers() {
	Member* temp = memberHead;
	if (!temp) {
		cout << "No members registered.\n";
		return;
	}
	cout << "\n--- Member List ---\n";
	while (temp) {
		cout << "ID: " << temp->memberID << "\n"
			<< "Name: " << temp->name << "\n"
			<< "Type: " << (temp->isPremium ? "Premium" : "Normal");
		if (temp->isPremium) {
			cout << "\nPremium Expiry: " << timeToString(temp->premiumExpiry);
		}
		cout << "\n-----------------\n";
		temp = temp->next;
	}
}
// ---------------------- User Functions ----------------------
void registerUser() {
	User* newUser = new User;
	cout << "Choose a username: ";
	cin >> newUser->username;
	if (caseInsensitiveCompare(newUser->username, "admin")) {
		cout << "Error: Cannot use 'admin' as username.\n";
		delete newUser;
		return;
	}
	User* temp = userHead;
	while (temp) {
		if (caseInsensitiveCompare(temp->username, newUser->username)) {
			cout << "Error: Username already exists.\n";
			delete newUser;
			return;
		}
		temp = temp->next;
	}
	cout << "Choose a password: ";
	cin >> newUser->password;
	cout << "Upgrade to premium? (1 for Yes, 0 for No): ";
	int premiumChoice;
	cin >> premiumChoice;
	newUser->isPremium = (premiumChoice == 1);
	if (newUser->isPremium) {
		newUser->premiumExpiry = time(0) + (365 * 24 * 60 * 60);
		cout << "Premium membership activated for 1 year!\n";
	}
	newUser->next = userHead;
	if (userHead) userHead->prev = newUser;
	else userTail = newUser;
	userHead = newUser;
	saveUsersToFile();
	cout << "Registration successful!\n";
}
bool loginUser() {
	string username, password;
	cout << "Username: ";
	cin >> username;
	cout << "Password: ";
	cin >> password;
	User* temp = userHead;
	while (temp) {
		if (caseInsensitiveCompare(temp->username, username) && temp->password ==
			password) {
			cout << "Login successful! Welcome, " << username << "!\n";
			if (temp->isPremium) {
				time_t now = time(0);
				if (now > temp->premiumExpiry) {
					cout << "Your premium membership has expired.\n";
					temp->isPremium = false;
					saveUsersToFile();
				}
				else {
					cout << "Premium status: Active (expires "
						<< timeToString(temp->premiumExpiry) << ")\n";
				}
			}
			return true;
		}
		temp = temp->next;
	}
	cout << "Error: Invalid credentials.\n";
	return false;
}
// ---------------------- Issue/Return Functions ----------------------
Book* findBook(const string& id) {
	Book* temp = bookHead;
	while (temp) {
		if (caseInsensitiveCompare(temp->bookID, id)) return temp;
		temp = temp->next;
	}
	return nullptr;
}
Member* findMember(const string& id) {
	Member* temp = memberHead;
	while (temp) {
		if (caseInsensitiveCompare(temp->memberID, id)) return temp;
		temp = temp->next;
	}
	return nullptr;
}
void issueBook() {
	string bookID, memberID;
	cout << "Enter Book ID to issue: ";
	cin >> bookID;
	cout << "Enter Member ID: ";
	cin >> memberID;
	Book* book = findBook(bookID);
	Member* member = findMember(memberID);
	if (!book) {
		cout << "Error: Book not found!\n";
		return;
	}
	if (!member) {
		cout << "Error: Member not found!\n";
		return;
	}
	if (book->isIssued) {
		cout << "Error: Book is already issued.\n";
		return;
	}
	int maxDays;
	if (member->isPremium) {
		maxDays = 365; // 1 year for premium
		cout << "Premium member: Book issued for 1 year. no deposit charged.\n";
	}
	else {
		cout << "Enter number of days to keep (1-14): ";
		cin >> maxDays;
		if (maxDays < 1 || maxDays > 14) {
			cout << "Error: Normal users can borrow for 1-14 days only.\n";
			return;
		}
		// Charge $25 deposit from normal member (covers $20 book charge + $5 refundable)
		cout << "Collected $25 deposit from normal member (includes $20 book charge + $5 refundable).\n";
	}
	time_t now = time(0);
	time_t returnDate = now + (maxDays * 24LL * 60 * 60);
	// Add to the head of the issued‐books doubly linked list
	IssuedBook* newIssue = new IssuedBook{ bookID, memberID, now, issuedHead };
	if (issuedHead != nullptr) {
		issuedHead->prev = newIssue;
	}
	else {
		issuedTail = newIssue;
	}
	issuedHead = newIssue;
	book->isIssued = true;
	book->issueDate = now;
	book->returnDate = returnDate;
	saveBooksToFile();
	saveIssuedBooksToFile();
	cout << "Book issued successfully!\n";
	cout << "Issued on: " << timeToString(now) << endl;
	cout << "Return by: " << timeToString(returnDate) << endl;
}
void returnBook() {
	string bookID;
	cout << "Enter Book ID to return: ";
	cin >> bookID;
	IssuedBook* temp = issuedHead;
	while (temp && !caseInsensitiveCompare(temp->bookID, bookID)) {
		temp = temp->next;
	}
	if (!temp) {
		cout << "Error: No such issued book found.\n";
		return;
	}
	// Remove from the issued‐books linked list
	if (temp->prev) temp->prev->next = temp->next;
	else issuedHead = temp->next;
	if (temp->next) temp->next->prev = temp->prev;
	else issuedTail = temp->prev;
	// Update book status
	Book* book = findBook(bookID);
	if (book) {
		book->isIssued = false;
		book->returnDate = time(0);
		saveBooksToFile();
	}
	// Check on‐time vs. late
	Member* member = findMember(temp->memberID);
	time_t now = time(0);
	double durationDays = difftime(now, temp->issueDate) / (60 * 60 * 24);
	cout << "\nBook returned successfully!\n";
	cout << "Issued on: " << timeToString(temp->issueDate) << endl;
	cout << "Returned on: " << timeToString(now) << endl;
	cout << "Duration: " << durationDays << " days\n";
	if (member && !member->isPremium) {
		// Compare now against the stored expected returnDate
		if (now <= book->returnDate) {
			// On time: refund $5 (since $20 book charge was covered by deposit)
			cout << "Book returned on time. $5 refunded to member.\n";
		}
		else {
			// Late: keep full $25 deposit (i.e., $20 charge + $5 late‐fee)
			cout << "Book returned late. $5 fine charged (no refund).\n";
		}
	}
	else if (member && member->isPremium) {
		cout << "Premium member – no deposit or fines apply.\n";
	}
	delete temp;
}
void viewIssuedBooks() {
	IssuedBook* temp = issuedHead;
	if (!temp) {
		cout << "No books currently issued.\n";
		return;
	}
	cout << "\n--- Issued Books ---\n";
	while (temp) {
		Member* member = findMember(temp->memberID);
		cout << "Book ID: " << temp->bookID << "\n"
			<< "Member ID: " << temp->memberID << "\n"
			<< "Member Type: " << (member ? (member->isPremium ? "Premium" : "Normal") :
				"Unknown") << "\n"
			<< "Issued on: " << timeToString(temp->issueDate) << "\n"
			<< "-----------------\n";
		temp = temp->next;
	}
}
// ---------------------- Search Function ----------------------
void searchBook() {
	int option;
	string query;
	cout << "\n--- Search Book ---\n"
		<< "1. By ID\n2. By Title\n3. By Author\nChoose option: ";
	cin >> option;
	cin.ignore();
	cout << "Enter search term: ";
	getline(cin, query);
	transform(query.begin(), query.end(), query.begin(), ::tolower);
	Book* temp = bookHead;
	bool found = false;
	while (temp) {
		string field;
		switch (option) {
		case 1: field = temp->bookID; break;
		case 2: field = temp->title; break;
		case 3: field = temp->author; break;
		default: cout << "Invalid option.\n"; return;
		}
		transform(field.begin(), field.end(), field.begin(), ::tolower);
		if (field.find(query) != string::npos) {
			cout << "Found: " << temp->title << " (ID: " << temp->bookID
				<< ", Author: " << temp->author
				<< ", Status: " << (temp->isIssued ? "Issued" : "Available") << ")\n";
			found = true;
		}
		temp = temp->next;
	}
	if (!found) cout << "No matching books found.\n";
}
// ---------------------- Mode Functions ----------------------
void adminMode() {
	string username, password;
	cout << "Admin username: ";
	cin >> username;
	cout << "Admin password: ";
	cin >> password;
	if (username != "admin" || password != "admin123") {
		cout << "Access denied!\n";
		return;
	}
	int choice;
	do {
		cout << "\n===== ADMIN MENU =====\n"
			<< "1. Add Book\n"
			<< "2. View Books\n"
			<< "3. Add Member\n"
			<< "4. View Members\n"
			<< "5. View Issued Books\n"
			<< "0. Logout\n"
			<< "Enter choice: ";
		while (!(cin >> choice)) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Invalid input. Enter a number: ";
		}
		switch (choice) {
		case 1: addBook(); break;
		case 2: viewBooks(); break;
		case 3: addMember(); break;
		case 4: viewMembers(); break;
		case 5: viewIssuedBooks(); break;
		case 0: cout << "Logging out...\n"; break;
		default: cout << "Invalid choice!\n";
		}
	} while (choice != 0);
}
void userMode() {
	if (!loginUser()) return;
	int choice;
	do {
		cout << "\n===== USER MENU =====\n"
			<< "1. View Books\n"
			<< "2. Issue Book\n"
			<< "3. Return Book\n"
			<< "4. View Issued Books\n"
			<< "5. Search Books\n"
			<< "0. Logout\n"
			<< "Enter choice: ";
		while (!(cin >> choice)) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Invalid input. Enter a number: ";
		}
		switch (choice) {
		case 1: viewBooks(); break;
		case 2: issueBook(); break;
		case 3: returnBook(); break;
		case 4: viewIssuedBooks(); break;
		case 5: searchBook(); break;
		case 0: cout << "Logging out...\n"; break;
		default: cout << "Invalid choice!\n";
		}
	} while (choice != 0);
}
// ---------------------- Main Function ----------------------
int main() {
	// Load data from files
	loadBooksFromFile();
	loadMembersFromFile();
	loadUsersFromFile();
	loadIssuedBooksFromFile();
	int choice;
	do {
		cout << "\n===== LIBRARY SYSTEM =====\n"
			<< "1. Admin Login\n"
			<< "2. User Login\n"
			<< "3. Register User\n"
			<< "0. Exit\n"
			<< "Enter choice: ";
		while (!(cin >> choice)) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Invalid input. Enter a number: ";
		}
		switch (choice) {
		case 1: adminMode(); break;
		case 2: userMode(); break;
		case 3: registerUser(); break;
		case 0: cout << "Exiting program...\n"; break;
		default: cout << "Invalid choice!\n";
		}
	} while (choice != 0);
	// Cleanup memory
	while (bookHead) {
		Book* temp = bookHead;
		bookHead = bookHead->next;
		delete temp;
	}
	while (memberHead) {
		Member* temp = memberHead;
		memberHead = memberHead->next;
		delete temp;
	}
	while (issuedHead) {
		IssuedBook* temp = issuedHead;
		issuedHead = issuedHead->next;
		delete temp;
	}
	while (userHead) {
		User* temp = userHead;
		userHead = userHead->next;
		delete temp;
	}
	return 0;
}