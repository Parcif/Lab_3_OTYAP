#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <ctype.h>
using namespace std;

enum Signals { Letter, Digit, ComparisonSing, AssignmentSing, ArithmeticSing, SeparatorSing, Other }; // сигналы
// Состояния автомата
enum States { Start, Id_Kw, Const, Comparison, ComplexComparison, Assignment, Arithmetic, Separator, Exception };
// Типы лексем (ключевое слово, оператор сравнения, арифметический оператор, идентификатор, константа)
enum LexemeType { kw, id, vl, co, eq, ao, wl };
static const char* LexemeTypeStrings[7] = { "[kw]", "[id]", "[vl]", "[co]", "[eq]", "[ao]", "[wl]"};
static const char* KeyWords[3] = { "while", "do", "loop" }; // Ключевые слова

void createTable(States table [7][9]) // матрица состояний
{
	table[Letter][Start] = Id_Kw;
	table[Letter][Id_Kw] = Id_Kw;
	table[Letter][Const] = Exception;
	table[Letter][Assignment] = Id_Kw;
	table[Letter][Arithmetic] = Id_Kw;
	table[Letter][Comparison] = Id_Kw;
	table[Letter][ComplexComparison] = Id_Kw;
	table[Letter][Exception] = Id_Kw;
	table[Letter][Separator] = Id_Kw;

	table[Digit][Start] = Const;
	table[Digit][Id_Kw] = Id_Kw;
	table[Digit][Const] = Const;
	table[Digit][Assignment] = Const;
	table[Digit][Arithmetic] = Const;
	table[Digit][Comparison] = Const;
	table[Digit][ComplexComparison] = Const;
	table[Digit][Exception] = Const;
	table[Digit][Separator] = Const;

	table[ComparisonSing][Start] = Comparison;
	table[ComparisonSing][Id_Kw] = Comparison;
	table[ComparisonSing][Const] = Comparison;
	table[ComparisonSing][Assignment] = Comparison;
	table[ComparisonSing][Arithmetic] = Comparison;
	table[ComparisonSing][Comparison] = ComplexComparison;
	table[ComparisonSing][ComplexComparison] = Comparison;
	table[ComparisonSing][Exception] = Comparison;
	table[ComparisonSing][Separator] = Comparison;

	table[AssignmentSing][Start] = Assignment;
	table[AssignmentSing][Id_Kw] = Assignment;
	table[AssignmentSing][Const] = Assignment;
	table[AssignmentSing][Assignment] = Exception;
	table[AssignmentSing][Arithmetic] = Assignment;
	table[AssignmentSing][Comparison] = ComplexComparison;
	table[AssignmentSing][ComplexComparison] = Assignment;
	table[AssignmentSing][Exception] = Assignment;
	table[AssignmentSing][Separator] = Assignment;

	table[ArithmeticSing][Start] = Arithmetic;
	table[ArithmeticSing][Id_Kw] = Arithmetic;
	table[ArithmeticSing][Const] = Arithmetic;
	table[ArithmeticSing][Assignment] = Arithmetic;
	table[ArithmeticSing][Arithmetic] = Exception;
	table[ArithmeticSing][Comparison] = Arithmetic;
	table[ArithmeticSing][ComplexComparison] = Arithmetic;
	table[ArithmeticSing][Exception] = Arithmetic;
	table[ArithmeticSing][Separator] = Arithmetic;

	table[SeparatorSing][Start] = Separator;
	table[SeparatorSing][Id_Kw] = Separator;
	table[SeparatorSing][Const] = Separator;
	table[SeparatorSing][Assignment] = Separator;
	table[SeparatorSing][Arithmetic] = Separator;
	table[SeparatorSing][Comparison] = Separator;
	table[SeparatorSing][ComplexComparison] = Separator;
	table[SeparatorSing][Exception] = Separator;
	table[SeparatorSing][Separator] = Separator;

	table[Other][Start] = Exception;
	table[Other][Id_Kw] = Exception;
	table[Other][Const] = Exception;
	table[Other][Assignment] = Exception;
	table[Other][Arithmetic] = Exception;
	table[Other][Comparison] = Exception;
	table[Other][ComplexComparison] = Exception;
	table[Other][Exception] = Exception;
	table[Other][Separator] = Exception;
}

// Вектор лексем
struct Lexeme 
{
	LexemeType type;
	char* text;
};

ostream& operator << (ostream& os, const Lexeme& lexeme) // перегрузка вывода
{
	return os << lexeme.text << LexemeTypeStrings[lexeme.type];
}

bool isComparison(char c)
{
	return (c == '<' || c == '>');
}

bool isAssigment(char c)
{
	return (c == '=');
}

bool isArithmetic(char c)
{
	return (c == '+' || c == '-');
}

bool isSeparator(char c)
{
	return (c == ' ' || c == '\0' || c == '\r' || c == '\n' || c == '\t');
}

Signals check(char c) // функция проверки элемента, возвращающая тип сигнал
{
	if (isalpha(c)) return Letter;
	if (isdigit(c)) return Digit;
	if (isComparison(c)) return ComparisonSing;
	if (isAssigment(c)) return AssignmentSing;
	if (isArithmetic(c)) return ArithmeticSing;
	if (isSeparator(c)) return SeparatorSing;
	return Other;
}

void lexemeAnalysis(const char* lines, vector<Lexeme>& result, States table [7][9], vector<char*>& identificators) // Функция лексического анализа
{
	long unsigned int size = strlen(lines);
	long unsigned int position = 0;	// текущая позиция в строке
	//const char* str = text, * lexstart;
	States state = Start, prevState;
	Lexeme lexeme;
	int start_lex_pos = 0; // позиция начала лексемы
	//int add;

	do
	{
		char current_char = lines[position]; // текущий символ
		prevState = state;
		//add = true;

		state = table[check(current_char)][state]; // изменяем состояние автомата

		// Добавление лексемы в список
		//if (add) result.push_back()
		// Сохранение начала лексемы если состояние изменилось
		if ((state != prevState) && (state == Id_Kw || state == Const || state == Comparison || state == Assignment || state == Arithmetic || state == Separator) && position && prevState != Separator)
		{			
			int length = position - start_lex_pos;
			lexeme.text = new char[length + 1];
			strncpy(&lexeme.text[0], &lines[0] + start_lex_pos, length); // Вычленение подстроки и запись в лексему			
			lexeme.text[length] = '\0'; // Постановка финализирующего 0

			if (prevState == Id_Kw)
			{
				lexeme.type = id;
				/*long unsigned int i = 0;
				while (KeyWords[i] != NULL)
				{
					if (*lexeme.text == *KeyWords[i])
					{
						lexeme.type = kw;
						break;
					}
					i++;
				}*/
				for (long unsigned int i = 0; KeyWords[i] != NULL; i++)
				{
					if (!strcmp(lexeme.text, KeyWords[i]))
					{
						lexeme.type = kw;
					}
				}

				if (lexeme.type == id)
					identificators.push_back(lexeme.text);
			}
			else if (prevState == Const)
			{
				lexeme.type = vl;
			}
			else if (prevState == Comparison || prevState == ComplexComparison)
			{
				lexeme.type = co;
			}
			else if (prevState == Assignment)
			{
				lexeme.type = eq;
			}
			else if (prevState == Arithmetic)
			{
				lexeme.type = ao;
			}
			else if (prevState == Exception)
			{
				lexeme.type = wl;
			}

			result.push_back(lexeme); // Запись лексемы в список

			if(state != Separator)
				start_lex_pos = position;
			
		}
		else if(prevState == Separator && state != Separator)
		{		
			start_lex_pos = position;
		}
		// Переход к следующему символу
		position++;

	} while (position <= size);

}

bool fileOutput(vector<Lexeme> result, vector<char*> identificators) // вывод в файл
{
	ofstream ofs("output.txt");

	if (!ofs.is_open()) // проверка открылся ли файл для записи
	{
		cout << "\nThe file is not open! Something went wrong!\n";
		return false;
	}

	long unsigned int i = 0;
	while (i < result.size() - 1) // выводим обработанный текст в консоль и в выходной файл без пробела в конце
	{
		cout << result[i] << " ";
		ofs << result[i] << " ";
		i++;
	}
	cout << result[i] << endl;
	ofs << result[i] << endl;

	long unsigned int j = 0;
	while (j < identificators.size()) // выводим идентификаторы в консоль и в выходной файл без пробела в конце
	{

		{
			cout << identificators[j] << " ";
			ofs << identificators[j] << " ";
		}
		j++;
	}
	/*if (result[j].type == id)
	{
		cout << result[j].text;
		ofs << result[j].text;
	}*/
	cout << endl;
	ofs << endl;

	long unsigned int k = 0;
	while (k < result.size()) // выводим константы в консоль и в выходной файл без пробела в конце
	{
		if (result[k].type == vl)
		{
			cout << result[k].text << " ";
			ofs << result[k].text << " ";
		}
		k++;
	}
	/*if (result[k].type == vl)
	{
		cout << result[k].text;
		ofs << result[k].text;
	}*/

	ofs.close();

	return true;
}


int main()
{
	try
	{
		ifstream ifs("input.txt", ios::binary | ios::ate); // открываем файл для чтения и перемещаем указатель в конец файла

		if (!ifs.is_open()) // проверка открылся ли файл для чтения
		{
			cout << "\nThe file is not open! Something went wrong!\n";
			return -1;
		}

		size_t num_of_elem = ifs.tellg(); // считаем количество элементов в файле
		ifs.seekg(0, ios::beg); // перемещаем указатель в начало файла

		char* lines = new char[num_of_elem + 1]; // создаем массив
		lines[num_of_elem] = '\0';

		ifs.read(&lines[0], num_of_elem); // читаем текст из файла в массив

		ifs.close();

		if (lines[0] == '\0')
			return 0;

		States table[7][9];

		createTable(table); // создаем таблицу состояний автомата

		vector<Lexeme> result; // вектор, в который будем записывать лексемы
		vector<char*> identificators;

		lexemeAnalysis(lines, result, table, identificators);

		delete[] lines;

		//if (!fileOutput(result, identificators)) // вывод в файл и проверка открылся ли файл для вывода
		//{
		//	cout << "\nThe file is not open! Something went wrong!\n";
		//	return -1;
		//}

		ofstream ofs("output.txt");

		if (!ofs.is_open()) // проверка открылся ли файл для записи
		{
			cout << "\nThe file is not open! Something went wrong!\n";
			return false;
		}

		long unsigned int i = 0;
		while (i < result.size() - 1) // выводим обработанный текст в консоль и в выходной файл без пробела в конце
		{
			cout << result[i] << " ";
			ofs << result[i] << " ";
			i++;
		}
		cout << result[i] << endl;
		ofs << result[i] << endl;

		long unsigned int l = 0;
		while (l < identificators.size()) // выводим идентификаторы в консоль и в выходной файл без пробела в конце
		{

			{
				cout << identificators[l] << " ";
				ofs << identificators[l] << " ";
			}
			l++;
		}
		/*if (result[j].type == id)
		{
			cout << result[j].text;
			ofs << result[j].text;
		}*/
		cout << endl;
		ofs << endl;

		long unsigned int k = 0;
		while (k < result.size()) // выводим константы в консоль и в выходной файл без пробела в конце
		{
			if (result[k].type == vl)
			{
				cout << result[k].text << " ";
				ofs << result[k].text << " ";
			}
			k++;
		}
		/*if (result[k].type == vl)
		{
			cout << result[k].text;
			ofs << result[k].text;
		}*/

		ofs.close();

		for (long unsigned int i = 0; i < result.size(); i++) // очищаем выделенную для текста лексем память
		{
			delete[] result[i].text;
		}
	}
	catch (runtime_error)
	{
		cout << "oib,rf";
	}
}