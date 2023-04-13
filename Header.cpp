#include "Header.h"
struct HoursMinutes
{
	int minutes;
	int hours;
};

struct Ampoule
{
	char name[50];
	HoursMinutes validity;
	float shelf_life;
};

struct OpenedAmpoule
{
	char name[50];
	HoursMinutes opening_time;
	HoursMinutes expiration_time;
};

string create_first_file()
{
	string file_name;
	bool error = 0;
	string str;
	Ampoule ampoule;
	int n = 3;
	char flag = 'y';
	cout << "Enter first file name: "; getline(cin, file_name);
	ifstream not_new_file(file_name, ios::binary);
	if (not_new_file)
	{
		cout << "Do you want to append to file? (y/n) "; cin >> flag; cin.ignore();
		if (flag != 'y' && flag != 'Y') 
		{
			not_new_file.close();
			remove(file_name.c_str());
			flag = 'y';
		}
	}
	
	ofstream file(file_name, ios::binary | ios::app);
	cout << "Enter ampoule information:" << endl;
	int i = 0;
	while (flag == 'y' || flag == 'Y')
	{
		cout << i + 1 << ". " << "Enter ampoule name: ";
		cin.getline(ampoule.name, sizeof(ampoule.name));
		do
		{
			error = 0;
			cout << "Enter ampoule validity period (in format HH:MM) : ";
			getline(cin, str);
			sscanf_s(str.c_str(), "%d:%d", &(ampoule.validity.hours), &(ampoule.validity.minutes));
			if (ampoule.validity.minutes >= 60 || ampoule.validity.minutes < 0)
			{
				cout << "Error. Number of minutes shouldn't be less than 0 or bigger than 59. Please try again" << endl;
				error = 1;
			}
		} while (error);

		cout << "Enter ampoule shelf life (in years) : "; cin >> ampoule.shelf_life; cin.ignore();
		file.write((char*)&ampoule, sizeof(Ampoule));
		cout << "Continue? (y/n) "; cin >> flag; cin.ignore();
		i++;
		cout << "==========================================" << endl;
	}
	file.close();
	cout << "So, here's your first file: " << endl;
	print_file1(file_name);

	return file_name;
}

string create_second_file(string file1_name)
{
	string file2_name, input;
	Ampoule a1;
	OpenedAmpoule opened_ampoule;
	int hours = 0, minutes = 0;
	char flag;
	bool error = 0;
	do
	{
		error = 0;
		cout << "Enter second file name: "; getline(cin, file2_name);
		if (file2_name == file1_name)
		{
			cout << "Error. Name of first and second files shouldn't be equal. Please enter second file name again." << endl;
			error = 1;
		}
	} while (error);

	ifstream file1(file1_name, ios::binary);
	ofstream file2(file2_name, ios::binary);
	while (file1.read((char*)&a1, sizeof(Ampoule)))
	{
		cout << "Name: " << a1.name << endl;
		cout << "Was it opened?(y/n) "; cin >> flag; cin.ignore();
		if (flag == 'y' || flag == 'Y')
		{
			strcpy_s(opened_ampoule.name, sizeof(a1.name), a1.name);

			do
			{
				error = 0;
				cout << "Opening time: ";
				getline(cin, input);
				sscanf_s(input.c_str(), "%d:%d", &(hours), &(minutes));

				if (minutes > 59 || minutes < 0)
				{
					cout << "Error. Number of minutes shouldn't be less than 0 or bigger than 59. Please try again" << endl;
					error = 1;
				}
				else if (hours > 23 || hours < 0)
				{
					cout << "Error. Number of hours shouldn't be less than 0 or bigger than 23. Please try again" << endl;
					error = 1;
				}
			} while (error);

			opened_ampoule.opening_time.hours = hours, opened_ampoule.opening_time.minutes = minutes;
			minutes += a1.validity.minutes;
			if (minutes >= 60)
			{
				minutes %= 60;
				hours += 1;
			}
			hours = (hours + a1.validity.hours) % 24;
			opened_ampoule.expiration_time.hours = hours;
			opened_ampoule.expiration_time.minutes = minutes;
			cout << "Best before: " << opened_ampoule.expiration_time.hours << " hours, " << opened_ampoule.expiration_time.minutes << " minutes" << endl;
			file2.write((char*)&opened_ampoule, sizeof(OpenedAmpoule));

		}
	}
	cout << "==========================================" << endl;
	file1.close(); file2.close();
	cout << "And here's your second file:" << endl;
	print_file2(file2_name);
	return file2_name;
}

void delete_expired(string second_file_name)
{
	ifstream second_file(second_file_name, ios::binary);
	ofstream copy_file("copy_file.txt", ios::binary);
	OpenedAmpoule a;
	time_t now = time(0);
	bool expired = 1, same_day, after_opening, before_expiry;

	tm* ltm = new tm;
	localtime_s(ltm, &now);

	while (second_file.read((char*)&a, sizeof(OpenedAmpoule)))
	{
		expired = 1;

		same_day = a.expiration_time.hours >= a.opening_time.hours;
		before_expiry = (ltm->tm_hour < a.expiration_time.hours) || ((ltm->tm_hour == a.expiration_time.hours) && (ltm->tm_min < a.expiration_time.minutes));
		after_opening = (ltm->tm_hour > a.opening_time.hours) || ((ltm->tm_hour == a.opening_time.hours) && (ltm->tm_min > a.opening_time.minutes));

		
		if (same_day)
		{
			if (before_expiry && after_opening) expired = 0;
		}
		else
		{
			if (before_expiry || after_opening) expired = 0;
		}

		if (!expired) copy_file.write((char*)&a, sizeof(OpenedAmpoule));
	}
	second_file.close(); copy_file.close();
	remove(second_file_name.c_str());
	if (rename("copy_file.txt", second_file_name.c_str()) != 0) cout << "Error renaming file \"copy_file.txt\"" << endl;

	cout << "Now let's delete expired ampoules" << endl;
	cout << "==========================================" << endl;
	cout << "Here's your second file after deleting:" << endl;
	print_file2(second_file_name);

}

void print_file1(string name)
{
	Ampoule a;
	ifstream file(name, ios::binary);
	cout << "File: " << name << endl;
	while (file.read((char*)&a, sizeof(Ampoule)))
	{
		cout << "Name: " << a.name << endl;
		cout << "Validity hours: " << a.validity.hours << ", minutes: " << a.validity.minutes << endl;
		cout << "Shelf life: " << a.shelf_life << endl;
	}
	cout << "==========================================" << endl;
	file.close();
}

void print_file2(string name)
{
	OpenedAmpoule a;
	ifstream file(name, ios::binary);
	cout << "File: " << name << endl;
	while (file.read((char*)&a, sizeof(OpenedAmpoule)))
	{
		cout << "Name: " << a.name << endl;
		cout << "Opening hours: " << a.opening_time.hours << " hours, " << a.opening_time.minutes << " minutes" << endl;
		cout << "Best before: " << a.expiration_time.hours << " hours, " << a.expiration_time.minutes << " minutes" << endl;
	}
	cout << "==========================================" << endl;
	file.close();
}
