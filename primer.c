#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include <locale.h>

#define BUFFER 64 // Должно хватить на все утехи

// Так будет намного лечге, чем каждый раз чото высчитывать, с учетом того что мы подсчитываем
// количество предложений на этапе сплита
struct strings_array {
	char** strings;
	int size;
};

char* read_text();
int check_text(char* str_in);
struct strings_array text_split(char*, char delim);
int count_words(char* str);
struct strings_array delete_even_sentences(struct strings_array str_arr);
void sort_words(struct strings_array str_arr);
char* replace_all_less_than_3_on(char* str, char* str_to_replace);
void find_longest_digit_strings(struct strings_array str_arr);
struct strings_array delete_all_same_sentences(struct strings_array str_arr);

char* concat_to_text(struct strings_array str_arr) {
	// calc size
	int size = 0;
	for (int i = 0; i < str_arr.size; i++) {
		size += strlen(str_arr.strings[i]); // длина строки
		size += 1; // + не забываем про точку между предложениями
	}

	char* str_out = (char*)malloc(size + 1);
	str_out[0] = '\0'; // чтобы strcat не падал
	
	for (int i = 0; i < str_arr.size; i++) {
		strcat(str_out, str_arr.strings[i]);
		strcat(str_out, ".");
	}

	return str_out;
}


int chooser() {
	printf("Выберите опцию:\n");
	printf("1. Удалить все четные по счету предложения в которых четное количество слов.\n");
	printf("2. Отсортировать все слова в предложениях по возрастанию количества букв в верхнем регистре.\n");
	printf("3. Заменить все слова в тексте длина которых не более 3 символов на подстроку \"Less Than 3\".\n");
	printf("4. Найти в каждом преложении строку максимальной длины, котоаря начинается и заканчивается цифрой. Вывести найденные подстроки по убыванию длины подстроки.\n");

	printf(">> ");
	
	// read digit from stdin
	char* str = malloc(sizeof(char) * 2); // sizof(char) это 1, но это не очевидно. Умножаем на 2, чтобы не было проблем с переполнением
	fgets(str, 2, stdin); // читаем ввод
	int choice = atoi(str); //Конвертируем строку в число
	free(str); // освобождаем память
	
	if (!(1 <= choice && choice <= 4)) { // проверяем что число в диапазоне
		printf("Некорректный ввод\n"); // если нет, то выводим ошибку
		return 0;
	}

	return choice; // возвращаем число
}

int main() {
	setlocale(LC_ALL, ""); // для кириллицы

	int amount; // количество предложений
	char* str_in = read_text(); // читаем текст
	if (check_text(str_in)) { // проверяем текст на ошибки
		printf("Текст подходит под требования.\n\n"); // если все ок, то выводим сообщение
	}
	else {
		goto finish; // если нет, то просто выходим
	}

	struct strings_array str_arr = text_split(str_in, '.'); // разбиваем текст на предложения
	str_arr = delete_all_same_sentences(str_arr); // удаляем повторяющиеся предложения
	printf("Текст после очистки от повторов:\n");
	for (int i = 0; i < str_arr.size; i++) {
		printf("%s.", str_arr.strings[i]);
	}
	printf("\n\n");

	int choice = chooser(); // выбираем опцию
	if (choice) {
		switch (choice) {
		case 1: // Удалить все четные по счету предложения в которых четное количество слов.
			printf("=======================\n");
			printf("Текст после преобразования:\n");

			struct strings_array choice1 = delete_even_sentences(str_arr);
			printf(concat_to_text(choice1));
			printf("\n=======================\n");
			break;
		case 2: // Отсортировать все слова в предложениях по возрастанию количества букв в верхнем регистре.
			printf("\n=======================\n");
			printf("Текст после сортировки слов в предложениях (qsort из стандартной библиотеки, компарация по колву букв в верхнем регистре):\n");
			sort_words(str_arr);
			printf("\n=======================\n");
			break;
		case 3:		// Заменить все слова в тексте длина которых не более 3 символов на подстроку "Less Than 3".
			printf("\n==============\n");
			printf("Текст после преобразования:\n");
			char* replaced_string = replace_all_less_than_3_on(str_in, "Less Then 3");
			printf(replaced_string);
			printf("\n==============\n");
			break;
		case 4: // Найти в каждом преложении строку максимальной длины, котоаря начинается и заканчивается цифрой. Вывести найденные подстроки по убыванию длины подстроки.
			find_longest_digit_strings(str_arr);
			break;
		}
	}

		
finish:
	printf("Введите что хотите чтобы закончить работу с программой: ");
	char ignored = getchar();
	free(str_in);
	return 0;
}

// Прочитать текст в обычную строку. Конец текста - \n
char* read_text() {
	printf("Введите текст, в котором предложения разделены точками. Предложения - набор слов,\nразделенные пробелом или запятой, слова - набор латинских букв и цифр\n\n");
	printf("Текст : ");

	int c; // текущий символ
	int nch = 0; // количество символов в строке
	int size = BUFFER; // размер буфера
	char* buf = malloc(size); // выделяем память под буфер
	if (buf == NULL) //  Бывает не выделяется
	{
		fprintf(stderr, "out of memory\n");
		exit(1);
	}

	while ((c = getchar()) != 10) // символ '\n' == 10
	{
		if (nch >= size - 1)
		{
			size += 1; // дибильное увеличение, но пусть будет
			buf = realloc(buf, size);
			if (buf == NULL)
			{
				fprintf(stderr, "out of memory\n");
				exit(1);
			}
		}

		buf[nch++] = c; // добавляем символ в буфер
	}

	buf[nch++] = '\0';	// добавляем символ конца строки
	return buf;
}

// Проверяем на соответствие требованиям
int check_text(char* s) {
	int len = strlen(s);
	for (int i = 0; i < len - 1; i++) { // len - 1, чтобы не выйти за границы массива
		if (!(s[i] == 32 || s[i] == 44 || s[i] == 46 || (s[i] >= 48 && s[i] <= 57) || (s[i] >= 65 && s[i] <= 90) || (s[i] >= 97 && s[i] <= 122))) {
			printf("\nВ текст могут включаться только латинские буквы, цифры, точки и запятые.\n");
			return 0;
		}
	}
	if (s[len - 1] != '.') { 
		printf("\nПоследний символ должен быть точкой.\n");
		return 0;
	}
	return 1;
}

// Разделяем на предложения
struct strings_array text_split(char* str_in, char delim) {
	int amount = 0; // количество предложений
	int len = strlen(str_in); // длина строки
	for (int i = 0; i < len; i++) { // считаем количество предложений
		if (str_in[i] == '.') { // если встретили точку
			amount++;
		}
	}
	char** str = (char**)malloc(amount * sizeof(char*)); // выделяем память под массив указателей на строки

	int j = 0; // номер текущего предложения
	int prev_sentence_end = 0; // индекс предыдущего конца предложения
	for (int i = 0; i < len; i++) { // проходим по строке
		if (str_in[i] == delim) { // если встретили разделитель
			str[j] = (char*)malloc((i - prev_sentence_end + 1) * sizeof(char)); // выделяем память под строку
			for (int k = 0; k < i - prev_sentence_end; k++) { // копируем строку
				str[j][k] = str_in[prev_sentence_end + k]; // копируем символ
			}
			str[j][i - prev_sentence_end] = '\0'; // добавляем символ конца строки
			j++; // переходим к следующему предложению
			prev_sentence_end = i + 1; // запоминаем индекс конца предложения
		}
	}

	struct strings_array str_arr; // создаем структуру
	str_arr.strings = str;
	str_arr.size = amount;
	return str_arr;
}

// Удалить все повторяющиеся предложения (без учета регистра)
struct strings_array delete_all_same_sentences(struct strings_array str_arr) {
	// удалить повторяющиеся предложения повторяющие одно из предыдущих
	for (int i = 0; i < str_arr.size; i++) {
		for (int j = i + 1; j < str_arr.size; j++) {
			// Создаём копии предложений, чтобы не изменять исходные
			char* str1 = (char*)malloc((strlen(str_arr.strings[i]) + 1) * sizeof(char));
			char* str2 = (char*)malloc((strlen(str_arr.strings[j]) + 1) * sizeof(char));
			strcpy(str1, str_arr.strings[i]);
			strcpy(str2, str_arr.strings[j]);
			
			// К нижнему регистру
			for (int k = 0; k < strlen(str1); k++) {
				str1[k] = tolower(str1[k]);
			}
			for (int k = 0; k < strlen(str2); k++) {
				str2[k] = tolower(str2[k]);
			}

			// сравниваем
			if (strcmp(str1, str2) == 0) {
				free(str_arr.strings[j]);
				for (int k = j; k < str_arr.size - 1; k++) {
					str_arr.strings[k] = str_arr.strings[k + 1];
				}
				str_arr.size--;
				j--;
			}

			// Не забудем очиститься
			free(str1);
			free(str2);
		}
	}

	return str_arr;
}

// Считаем слова. Важно помнить что нигде не сказано что все соответсвует правилам грамматики,
// так что могут появляться несколько запятых или пробелов подряд
int count_words(char* str) {
	int amount = 0; // количество слов
	int len = strlen(str); // длина строки
	int prev_delim = 1; // флаг предыдущего символа разделителя
	for (int i = 0; i < len; i++) { // проходим по строке
		if (str[i] == ' ' || str[i] == ',') { // если встретили разделитель
			if (prev_delim == 0) { // если предыдущий символ не разделитель
				amount++; 
			}
			prev_delim = 1; // запоминаем что встретили разделитель
		}
		else {
			prev_delim = 0; // запоминаем что встретили не разделитель
		}
	}
	if (prev_delim == 0) { // если последний символ не разделитель
		amount++;
	}
	return amount;
}

// Удаление четных предложений с четным колвом слов.
struct strings_array delete_even_sentences(struct strings_array str_arr) {
	// Подсчитаем количество предложений подходящих под требования
	int amount = 0; // количество предложений
	for (int i = 0; i < str_arr.size; i++) { // проходим по массиву предложений
		if (i % 2 != 0) { // Люди считают с 1, а в С счет с 0
			if (count_words(str_arr.strings[i]) % 2 == 0) { // если количество слов четное
				amount++;
			}
		}
	}
	char** str = (char**)malloc((str_arr.size - amount) * sizeof(char*)); // выделяем память под массив строк

	int j = 0; // индекс строки в новом массиве
	for (int i = 0; i < str_arr.size; i++) { // проходим по массиву предложений
		if (i % 2 != 0) { 
			if (count_words(str_arr.strings[i]) % 2 == 0) {
				continue;
			}
		}
		str[j] = str_arr.strings[i]; // копируем строку, а конкретно присваеваем указатель на нее
		j++;
	}

	struct strings_array str_arr_out;
	str_arr_out.strings = str;
	str_arr_out.size = str_arr.size - amount;

	return str_arr_out;
}

// count upper case letters in word
int count_upper(char* str) {
	int amount = 0; // количество заглавных букв
	int len = strlen(str);
	for (int i = 0; i < len; i++) {
		if (str[i] >= 65 && str[i] <= 90) { // если заглавная буква
			amount++;
		}
	}
	return amount;
}

struct strings_array extract_words(char* str) {
	struct strings_array words; // массив слов
	int amount = count_words(str);
	words.strings = (char**)malloc(amount * sizeof(char*)); // выделяем память под массив слов
	words.size = amount;

	int j = 0; // индекс слова в массиве
	int prev_delim = 1; // флаг предыдущего символа разделителя
	int prev_delim_pos = 0; // позиция предыдущего разделителя
	int len = strlen(str); // длина строки
	for (int k = 0; k < len; k++) { // проходим по строке
		if (str[k] == ' ' || str[k] == ',') { // если встретили разделитель
			if (prev_delim == 0) { // если предыдущий символ не разделитель
				words.strings[j] = (char*)malloc((k - prev_delim_pos + 1) * sizeof(char)); // выделяем память под слово
				for (int l = 0; l < k - prev_delim_pos; l++) { // копируем слово
					words.strings[j][l] = str[prev_delim_pos + l]; // копируем символ
				}
				words.strings[j][k - prev_delim_pos] = '\0'; // добавляем нуль-терминатор
				j++; // переходим к следующему слову
			}
			prev_delim = 1; // флаг предыдущего символа разделителя
			prev_delim_pos = k + 1; // позиция предыдущего разделителя
		}
		else {
			prev_delim = 0; // флаг предыдущего символа не разделитель
		}
	}

	if (prev_delim == 0) { // если последний символ не разделитель
		words.strings[j] = (char*)malloc((len - prev_delim_pos + 1) * sizeof(char)); // выделяем память под слово
		for (int l = 0; l < len - prev_delim_pos; l++) { // копируем слово
			words.strings[j][l] = str[prev_delim_pos + l]; // копируем символ
		}
		words.strings[j][len - prev_delim_pos] = '\0'; // добавляем нуль-терминатор
	}

	return words;
}

int count_delimeters(char* str) {
	int amount = 0; // количество разделителей
	int len = strlen(str);
	for (int i = 0; i < len; i++) {
		if (str[i] == ' ' || str[i] == ',' || str[i] == '.') { // если встретили разделитель
			amount++;
		}
	}
	return amount;
}

//compare function for qsort
int compare(const void* a, const void* b) {
	char* str_a = *(char**)a; // приводим к типу char*
	char* str_b = *(char**)b; // приводим к типу char*
	int count_a = count_upper(str_a); // количество заглавных букв в первом слове
	int count_b = count_upper(str_b); // количество заглавных букв во втором слове
	if (count_a > count_b) {
		return 1;
	}
	else if (count_a < count_b) {
		return -1;
	}
	else {
		return 0;
	}
}

void sort_words(struct strings_array str_arr) {
	for (int i = 0; i < str_arr.size; i++) {
		struct strings_array words = extract_words(str_arr.strings[i]); // массив слов
		
		qsort(words.strings, words.size, sizeof(char*), compare); // сортируем слова

		char* new_sentence = (char*)malloc(sizeof(char) * (strlen(str_arr.strings[i]) + 1)); // новое предложение	
		
		int i_old = 0; // индекс в старой строке
		int i_new = 0; // индекс в новой строке
		int i_word = 0; // индекс в массиве слов

		while (str_arr.strings[i][i_old] != '\0') { // проходимся, пока не дойдем до конца начальной строки
			if (str_arr.strings[i][i_old] == ' ' || str_arr.strings[i][i_old] == ',' || str_arr.strings[i][i_old] == '.') { // если делиметр
				new_sentence[i_new] = str_arr.strings[i][i_old]; // копируем в новую строку
				i_old++;
				i_new++;
			}
			else {
				while (str_arr.strings[i][i_old] != ' ' && str_arr.strings[i][i_old] != ',' && str_arr.strings[i][i_old] != '.' && str_arr.strings[i][i_old] != '\0') {
					i_old++; // пропускаем старое слово
				}
				for (int k = 0; k < strlen(words.strings[i_word]); k++) {
					new_sentence[i_new] = words.strings[i_word][k]; // вбиваем новое слово
					i_new++;
				}
				i_word++; // переходим к следующему слову из списка на замену
			}
		}

		new_sentence[i_new] = '\0';
		
		
		printf("%s.", new_sentence); // выводим предложение
	}
}

char* replace_all_less_than_3_on(char* str, char* str_to_replace) {
	// replace all words wich length less than 3 on str_to_replace
	// delimiters are spaces, commas and dots
	
	int len = strlen(str); // длина строки
	int len_to_replace = strlen(str_to_replace); // длина строки, на которую заменяем
	int amount = 0; // количество слов, которые заменяем
	int prev_delim = 1; // флаг предыдущего символа разделитель
	int prev_delim_pos = 0; // позиция предыдущего разделителя
	for (int i = 0; i < len; i++) { // проходим по строке
		if (str[i] == ' ' || str[i] == ',' || str[i] == '.') { // если текущий символ разделитель
			if (prev_delim == 0) { // если предыдущий символ не разделитель
				if (i - prev_delim_pos < 3) { // если длина слова меньше 3
					amount++; // увеличиваем количество слов, которые заменяем
				}
			}
			prev_delim = 1; // флаг предыдущего символа разделитель
			prev_delim_pos = i + 1; // позиция предыдущего разделителя
		}
		else {
			prev_delim = 0; // флаг предыдущего символа не разделитель
		}
	}
	if (prev_delim == 0) { // если последний символ не разделитель
		if (len - prev_delim_pos < 3) { // если длина слова меньше 3
			amount++; // увеличиваем количество слов, которые заменяем
		}
	}
	
	char* new_str = (char*)malloc((len + amount * (len_to_replace - 3) + 1) * sizeof(char)); // выделяем память под новую строку
	int j = 0; // позиция в новой строке
	prev_delim = 1; // флаг предыдущего символа разделитель
	prev_delim_pos = 0; // позиция предыдущего разделителя
	for (int i = 0; i < len; i++) { // проходим по строке
		if (str[i] == ' ' || str[i] == ',' || str[i] == '.') { // если текущий символ разделитель
			if (prev_delim == 0) { // если предыдущий символ не разделитель
				if (i - prev_delim_pos < 3) { // если длина слова меньше 3
					for (int k = 0; k < len_to_replace; k++) { // проходим по строке, на которую заменяем
						new_str[j] = str_to_replace[k]; // добавляем символ в новую строку
						j++; // увеличиваем позицию в новой строке
					}
				}
				else { // если длина слова больше или равна 3
					for (int k = prev_delim_pos; k < i; k++) { // проходим по слову
						new_str[j] = str[k]; // добавляем символ в новую строку
						j++; // увеличиваем позицию в новой строке
					}
				}
			}
			new_str[j] = str[i]; // добавляем символ в новую строку
			j++; // увеличиваем позицию в новой строке
			prev_delim = 1; // флаг предыдущего символа разделитель
			prev_delim_pos = i + 1; // позиция предыдущего разделителя
		}
		else {
			prev_delim = 0; // флаг предыдущего символа не разделитель
		}
	}
	
	if (prev_delim == 0) { // если последний символ не разделитель
		if (len - prev_delim_pos < 3) { // если длина слова меньше 3
			for (int k = 0; k < len_to_replace; k++) { // проходим по строке, на которую заменяем
				new_str[j] = str_to_replace[k]; // добавляем символ в новую строку
				j++; // увеличиваем позицию в новой строке
			}
		}
		else { // если длина слова больше или равна 3
			for (int k = prev_delim_pos; k < len; k++) { // проходим по слову
				new_str[j] = str[k]; // добавляем символ в новую строку
				j++; // увеличиваем позицию в новой строке
			}
		}
	}
	
	new_str[j] = '\0'; // добавляем символ конца строки в новую строку
	
	return new_str;
}

// Тут все тоже самое что и в предыдущей функции, только вместо посчеста заглавных, считаем количество символов
int compare_by_length(const void* a, const void* b) {
	char* str_a = *(char**)a;
	char* str_b = *(char**)b;
	int len_a = strlen(str_a);
	int len_b = strlen(str_b);
	if (len_a > len_b) {
		return 1;
	}
	else if (len_a < len_b) {
		return -1;
	}
	else {
		return 0;
	}
}

void add_string(struct strings_array* str_arr, char* str) {
	if (str_arr->size == 0) { // если массив пустой
		str_arr->strings = (char**)malloc(sizeof(char*)); // выделяем память под один указатель
		str_arr->strings[0] = str; // добавляем строку в массив
		str_arr->size = 1;
	}
	else { // если массив не пустой
		str_arr->strings = (char**)realloc(str_arr->strings, (str_arr->size + 1) * sizeof(char*)); // выделяем памяти на один указатель больше
		str_arr->strings[str_arr->size] = str; // добавляем строку в массив, а конкретно в последнюю позицию
		str_arr->size++; // увеличиваем размер массива
	}
}

// Судя по заданию, подразумевались подстроки предложений. Отталкиваемся от этого.
void find_longest_digit_strings(struct strings_array str_arr) {
	struct strings_array digit_strings; // массив строк, которые начинаются и заканчиваются цифрой
	digit_strings.size = 0;
	
	for (int i = 0; i < str_arr.size; i++) {
		// посчитать длину строки начиная с первой цифры и заканчивая последней
		
		int len = strlen(str_arr.strings[i]); // длина строки
		int first_digit = -1; // позиция первой цифры
		int last_digit = -1; // позиция последней цифры
		for (int j = 0; j < len; j++) { // проходим по строке
			if (str_arr.strings[i][j] >= '0' && str_arr.strings[i][j] <= '9') { // если символ цифра
				if (first_digit == -1) { // если первая цифра еще не найдена
					first_digit = j; // запоминаем позицию первой цифры
				}
				last_digit = j; // запоминаем позицию последней цифры
			}
		}
		
		if (first_digit == -1) { // если первая цифра не найдена вообще
			printf("В предложении %d нет строк, начинающихся и заканчивающихся цифрой\n", i);
			continue; // переходим к следующему предложению
		}
		
		// print that string
		printf("Самая длинная строка, начинающаяся и заканчивающаяся цифрой, в предложении %d: \"", i);
		for (int j = first_digit; j <= last_digit; j++) {
			printf("%c", str_arr.strings[i][j]);
		}
		printf("\"\n");

		char* str = (char*)malloc((last_digit - first_digit + 2) * sizeof(char)); // выделяем память под строку
		int j = 0;
		for (int k = first_digit; k <= last_digit; k++) {
			str[j] = str_arr.strings[i][k]; // копируем строку
			j++;
		}
		str[j] = '\0';
		
		add_string(&digit_strings, str); // добавляем строку в массив
	}

	// sort array of strings by length
	qsort(digit_strings.strings, digit_strings.size, sizeof(char*), compare_by_length); // сортируем массив строк по длине
	
	// print sorted array of strings
	printf("====================================================================\n");
	printf("Отсортированный по длине строки массив строк, начинающихся и заканчивающихся цифрой:\n");
	for (int i = 0; i < digit_strings.size; i++) {
		printf("%d: \"%s\"\n", i, digit_strings.strings[i]); // красивенько выводим
	}
	printf("====================================================================\n");
}
