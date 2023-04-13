#include "Header.h"
struct HoursMinutes
{
	int minutes;
	int hours;
};

struct Ampoule
{
	char name[50];
	//int validity_hours;
	//int validity_minutes;
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
	//cout << file_name << endl;
	ofstream file(file_name, ios::binary);
	cout << "Enter ampoule information (enter \"stop\" to stop):" << endl;
	int i = 0;
	while (flag == 'y' || flag == 'Y')
	{
		cout << i + 1 << ". " << "Enter ampoule name: ";
		cin.getline(ampoule.name, sizeof(ampoule.name));
		cout << ampoule.name << "|" << endl;
		cout << ampoule.name << " == \"stop\": " << (ampoule.name == "stop") << endl;
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
		cout << ampoule.shelf_life << endl;
		file.write((char*)&ampoule, sizeof(Ampoule));
		cout << "Continue? (y/n) "; cin >> flag; cin.ignore();
		i++;
		cout << "==========================================" << endl;
	}

	file.close();

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
		cout << file2_name << endl;
		if (file2_name == file1_name)
		{
			cout << "Error. Name of first and second files shouldn't be equal. Please enter second file name again." << endl;
			error = 1;
		}
	} while (error);

	ifstream file1(file1_name, ios::binary);
	ofstream file2(file2_name, ios::binary);
	//cout << "Ampoule names:" << endl;
	cout << "Enter ampoule opening time:" << endl;
	while (file1.read((char*)&a1, sizeof(Ampoule)))
	{
		cout << "Name: " << a1.name << endl;
		cout << "Was it opened?(y/n) "; cin >> flag; cin.ignore();
		if (flag == 'y' || flag == 'Y')
		{
			//opened_ampoule.name = a1.name;
			strcpy_s(opened_ampoule.name, sizeof(a1.name), a1.name);

			do
			{
				error = 0;
				cout << "opening time: ";
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

			cout << "Hours: " << hours << ", minutes: " << minutes << endl;
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
			cout << "Best before: " << opened_ampoule.expiration_time.hours << ", minutes: " << opened_ampoule.expiration_time.minutes << endl;
			file2.write((char*)&opened_ampoule, sizeof(OpenedAmpoule));

		}
	}
	cout << "==========================================" << endl;
	file1.close(); file2.close();
	return file2_name;
}

void delete_expired(string second_file_name)
{
	ifstream second_file(second_file_name, ios::binary);
	ofstream copy_file("copy_file.txt", ios::binary);
	OpenedAmpoule a;
	time_t now = time(0);
	bool expired = 1;

	cout << "Number of sec since January 1,1970 is:: " << now << endl;

	tm* ltm = new tm;
	localtime_s(ltm, &now);

	cout << "Current local time: " << ltm->tm_hour << " hours, " << ltm->tm_min << " minutes" << endl;
	while (second_file.read((char*)&a, sizeof(OpenedAmpoule)))
	{
		expired = 1;
		//cout << "Exp. hours: " << a.expiration_time.hours << " openining hours: " << a.opening_time.hours << endl;
		//cout << "a.expiration_time.hours > a.opening_time.hours = " << (a.expiration_time.hours > a.opening_time.hours) << endl;
		//cout << "ltm->tm_hour < a.expiration_time.hours = " << (ltm->tm_hour < a.expiration_time.hours) << endl;
		//cout << "ltm->tm_hour > a.opening_time.hours = " << (ltm->tm_hour > a.opening_time.hours) << endl;
		if (a.expiration_time.hours > a.opening_time.hours)
		{
			if (ltm->tm_hour < a.expiration_time.hours && ltm->tm_hour > a.opening_time.hours) expired = 0;
			/*cout << "ltm->tm_hour < a.expiration_time.hours && ltm->tm_hour > a.opening_time.hours = " << (ltm->tm_hour < a.expiration_time.hours && ltm->tm_hour > a.opening_time.hours) << endl;*/
		}
		else
		{
			if ((ltm->tm_hour < a.expiration_time.hours) || (ltm->tm_hour > a.opening_time.hours)) expired = 0;
			//cout << "(ltm->tm_hour < a.expiration_time.hours) || (ltm->tm_hour > a.opening_time.hours) = " << ((ltm->tm_hour < a.expiration_time.hours) || (ltm->tm_hour > a.opening_time.hours)) << endl;
		}
		cout << a.name << ", expired: " << expired << endl;

		if (!expired) copy_file.write((char*)&a, sizeof(OpenedAmpoule));
		cout << "==========================================" << endl;
	}
	second_file.close(); copy_file.close();
	remove(second_file_name.c_str());
	if (rename("copy_file.txt", second_file_name.c_str()) != 0) cout << "Error renaming file \"copy_file.txt\"" << endl;

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