#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <ctype.h>
using namespace std;

enum Signals { Letter, Digit, ComparisonSing, AssignmentSing, ArithmeticSing, SeparatorSing, Other }; // сигналы
enum States { Start, Id_Kw, Const, Comparison, ComplexComparison, Assignment, Arithmetic, Separator, Exception }; // Состояния автомата
enum LexemeType { kw, id, vl, co, eq, ao, wl }; // Типы лексем
static const char* LexemeTypeStrings[7] = { "[kw]", "[id]", "[vl]", "[co]", "[eq]", "[ao]", "[wl]"}; // текст типов лексем
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
	table[Letter][Exception] = Exception;
	table[Letter][Separator] = Id_Kw;

	table[Digit][Start] = Const;
	table[Digit][Id_Kw] = Id_Kw;
	table[Digit][Const] = Const;
	table[Digit][Assignment] = Const;
	table[Digit][Arithmetic] = Const;
	table[Digit][Comparison] = Const;
	table[Digit][ComplexComparison] = Const;
	table[Digit][Exception] = Exception;
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

struct Lexeme // Контейнер лексемы
{
	LexemeType type; // тип лексемы
	char* text; // текст лексемы
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

void lexemeAnalysis(const char* lines, vector<Lexeme>& result, States table [7][9]) // Функция лексического анализа
{
	long unsigned int size = strlen(lines); // вычисление размера строки
	long unsigned int position = 0;	// текущая позиция в строке
	States state = Start, prevState;
	Lexeme lexeme;
	int start_lex_pos = 0; // позиция начала лексемы

	do
	{
		char current_char = lines[position]; // текущий символ
		prevState = state;

		state = table[check(current_char)][state]; // изменяем состояние автомата

		// Запись лексемы если состояние изменилось
		if ((state != prevState) && (state == Id_Kw || state == Const || state == Comparison || state == Assignment || state == Arithmetic || state == Separator) && position && prevState != Separator)
		{			
			int length = position - start_lex_pos;
			lexeme.text = new char[length + 1];
			strncpy(&lexeme.text[0], &lines[0] + start_lex_pos, length); // Вычленение подстроки и запись в лексему			
			lexeme.text[length] = '\0'; // Постановка финализирующего 0

			if (prevState == Id_Kw) // определение типа лексемы
			{
				lexeme.type = id;
								
				for (long unsigned int i = 0; KeyWords[i] != NULL; i++)
				{
					if (!strcmp(lexeme.text, KeyWords[i])) // сравнение для проверки является лексема ключевым словом, или идентификатором
						lexeme.type = kw;
				}

				if (lexeme.type == id && strlen(lexeme.text) > 5) // проверка идентификатора на соответсвие 
				{
					lexeme.type = wl;
				}				
			}
			else if (prevState == Const)
			{
				lexeme.type = vl;

				if (atoi(lexeme.text) > 32768) // проверка константы на соответствие
					lexeme.type = wl;
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

			if (state != Separator) // сохранение начала лексемы если непробельный символ
				start_lex_pos = position;
			
		}
		else if(prevState == Separator && state != Separator) // Сохранение начала лексемы в случае обнаружения непробельного символа
		{		
			start_lex_pos = position;
		}
		
		position++; // Переход к следующему символу

	} while (position <= size);

}

bool fileOutput(vector<Lexeme> result) // вывод в файл
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
	while (j < result.size()) // выводим идентификаторы в консоль и в выходной файл
	{
		if (result[j].type == id)
		{
			cout << result[j].text << " ";
			ofs << result[j].text << " ";
		}
		j++;
	}
	cout << endl;
	ofs << endl;

	long unsigned int k = 0;
	while (k < result.size()) // выводим константы в консоль и в выходной файл
	{
		if (result[k].type == vl)
		{
			cout << result[k].text << " ";
			ofs << result[k].text << " ";
		}
		k++;
	}

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

		if (lines[0] == '\0') // прерывание программы в случае, если массив пустой
			return 0;

		States table[7][9];

		createTable(table); // создаем таблицу состояний автомата

		vector<Lexeme> result; // вектор, в который будем записывать лексемы

		lexemeAnalysis(lines, result, table);

		delete[] lines;

		if (!fileOutput(result)) // вывод в файл и проверка открылся ли файл для вывода
		{
			cout << "\nThe file is not open! Something went wrong!\n";
			return -1;
		}

		for (long unsigned int i = 0; i < result.size(); i++) // очищаем выделенную для текста лексем память
		{
			delete[] result[i].text;
		}
	}
	catch (runtime_error) {};
}