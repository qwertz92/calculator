#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define MAX_INPUT 50
#define IS_NUM 1
#define IS_CHAR 2
#define IS_OPER 3
#define QUIT_STRING "quit"
#define HELP_STRING "help"
#define QUIT 0
#define HELP 10
#define MEMORY_SIZE 26
#define MEMORY_ALLOC_ERROR -1


int f_check_input(char *token);
void f_move_doublearray(double *array, size_t n);
int f_memory(double **memory, int *memory_count, double number);

int main(int argc, char *argv[]) {
	char input_string[MAX_INPUT] = {};
	char delimiter[] = {' ', '\n', '\0'};
	char *token = NULL;
	char operator[25] = {};
	int input = 0;
	int memory_count = 0;
	int i = 0, count = 0;
	double *memory = NULL;
	double number[25] = {};
	double result = 0;
	char help[] = "\nThis is help!\nGeben Sie eine Rechnung im Format \"Zahl/Variable Rechenoperator Zahl/Variable ...\" z.B. 1 + 2 oder a - b ein.\n"
								"Es koennen auch laengere Rechnung eingegeben werden z.B. 5 + 4 / 3 * 4.2\n"
								"Gueltige Rechenoperationen: +, -, *, / (Punkt vor Strich wird beachtet)\n"
								"Gueltige Variablen: a ... z (nur wenn diese definiert wurden)\n"
								"Variablen koennen direkt beim Aufruf als Parameter uebergeben werden.\n";

	printf("==============================================\
				\nGeben Sie \"help\" ein um die Hilfe aufzurufen\n\n");
	// Parameter abfragen und in Speicher schreiben wenn bestimmte Bedingungen erfuellt sind
	if(argc < 2 || argc > 26) {
		if (argc > 26) {
			printf("Es wurden zu viele Werte eingegeben! MAX-INPUT = %d\n", MEMORY_SIZE - 1);
			return -1;
		} else {
			printf("Keine Werte in den Speicher eingelesen.\n");
		}
	} else {
		for (count = 1; count < argc; count++) {
			input = f_check_input(argv[count]);
			switch (input) {
				case IS_NUM:
					number[count] = atof(argv[count]);
					break;
				case QUIT:
					free(memory);
					return 0;
				case HELP:
					printf("%s\n", help);
					count = 0;
					argc = 0;
					break;
				default:
					free(memory);
					printf("Es wurden falsche Eingaben getaetigt!\n");
					return -1;
			}
		}
		// Werte in Memory speichern
		for (i = 1; i < count; i++) {
			if(f_memory(&memory, &memory_count, number[i]) != 0) {
				printf("Fehler beim allocieren von Speicher!\n");
				free(memory);
				return -1;
			}
		}
	}
	// Lauft so lange bis der Speicher voll ist
	while (memory_count < MEMORY_SIZE) {
		printf("Eingabe> ");
		if (fgets(input_string, MAX_INPUT, stdin) == NULL) {
			printf("Error Reading!\n");
			return -1;
		}
		token = strtok(input_string, delimiter);
		if(!token) {
			printf("ERROR token is NULL\n");
		} else {
			for (count = 0; token != NULL; count++) {
				input = f_check_input(token);
				switch (input) {
					case IS_NUM:
						if ((count % 2) == 0) {
							number[(count / 2)] = atof(token);
						} else {
							input = -1;
						}
						break;
					case IS_CHAR:
						if ((count % 2) == 0 && ((*token) - 'a') < memory_count) {
							number[(count / 2)] = memory[((*token) - 'a')];
						} else {
							input = -1;
						}
						break;
					case IS_OPER:
						if ((count % 2) == 1) {
							operator[((count - 1) / 2)] = *token;
						} else {
							input = -1;
						}
						break;
					case QUIT:
						free(memory);
						return 0;
					case HELP:
						printf("%s\n", help);
						token = NULL;
						break;
				}
				if (input < 0) {
					printf("Es wurden falsche Eingaben getaetigt!\n");
					free(memory);
					return -1;
				}
				// Naechsten token einlesen wenn nicht help eingegeben wurde
				if (input != HELP) {
					token = strtok(NULL, delimiter);
				}
			}
			// Berechnung
			// Soll nur durchgefuehrt werden wenn der kein Input fehlerhaft war und mindestens 3(immer ungerade(5,7,9)) Zeichen eingegeben wurde (2 + 1)
			if (input != HELP) {
				if ((input != -1) && (count % 2 == 1) && count > 2) {
					for (i = 0; i < ((count - 1) / 2); i++) {
						if (operator[i] == '*') {
							number[i + 1] = number[i] * number[(i + 1)];
							strncpy((operator + i), (operator + (i + 1)), ((count - 1) / 2) - (i + 1));			// delete Operator from array and shift all values 1 left
							f_move_doublearray((number + i), (((count - 1) / 2) - i));
							count -= 2;
							i--;
						}
						if (operator[i] == '/') {
							if (number[i + 1] == 0) {
								printf("Division durch 0 nicht moeglich!\n");
								free(memory);
								return -1;
							}
							number[i + 1] = number[i] / number[(i + 1)];
							strncpy((operator + i), (operator + (i + 1)), ((count - 1) / 2) - (i + 1));			// delete Operator from array and shift all values 1 left
							f_move_doublearray((number + i), (((count - 1) / 2) - i));
							count -= 2;				// weil sich die Rechnung um einen Operator und eine Zahl verringert
							i--;							// durch linksverschieben der Operatoren und Zahlen soll der Index der selbe bleiben im naechsten Durchlauf
						}
					}
					result = number[0];													// Falls keine Addition oder Subtarktion mehr folgt. Anderfalls wird result ueberschrieben
					for (i = 0; i < ((count - 1) / 2); i++) {
						if (operator[i] == '+') {
							if (i == 0) {
								result = number[i] + number[(i + 1)];
							} else {
								result += number[i + 1];
							}
						}
						if (operator[i] == '-') {
							if (i == 0) {
								result = number[i] - number[(i + 1)];
							} else {
								result -= number[i + 1];
							}
						}
					}
				} else {
					printf("Es wurden falsche Eingaben getaetigt!\n");
					free(memory);
					return -1;
				}
				if(f_memory(&memory, &memory_count, result) != 0) {
					printf("Fehler beim allocieren von Speicher!\n");
					free(memory);
					return -1;
				}
			}
		}
	}

	printf("Memory full!\n");
	free(memory);
	return 0;
}

// Ueberpruefen ob eine korrekte Zahl, Buchstabe oder Rechenoperator eingegeben wurde
int f_check_input(char *token){
	int length = strlen(token);
	int i = 0;
	int decimal_point = 0;
	if (length == 1) {
		switch (*token) {
			case 48 ... 57:
				return IS_NUM;
			case 97 ... 122:
				return IS_CHAR;
			case 42:		// *
			case 43:		// +
			case 45:		// -
			case 47:		// /
				return IS_OPER;
			default:
				return -1;
		}
	} else {
		for (i = 0; i < length; i++) {
			if ((*(token + i) < 48 || *(token + i) > 57) && *(token + i) != '.') {					// Check ob ungleich 0 - 9 und '.'
				if (strcmp(token, QUIT_STRING) == 0) {																				// check ob token = quit entspricht
					return QUIT;
				}
				if (strcmp(token, HELP_STRING) == 0) {																				// check ob token = help entspricht
					return HELP;
				}
				if (i == 0) {																																	// check ob Zahl mit Vorzeichen eingegeben wurde (Vorzeichen kann nur an erster Stelle sein)
					switch (token[i]) {
						case 43:		// +
						case 45:		// -
							break;
						default:
							return -1;
					}
				} else {
					return -1;
				}
			}
			if (token[i] == '.') {
				decimal_point++;
				if (decimal_point > 1) {						// Check if mehr als ein komma pro zahl zb 4.3.44
					return -1;
				}
			}
		}
		return IS_NUM;
	}
	return -1;
}

void f_move_doublearray(double *array, size_t n) {
	int i = 0;
	for (i = 0; i < n; i++) {
		array[i] = array[i + 1];
	}
}

int f_memory(double **memory, int *memory_count, double number) {
	// Speicher anlegen, oder wenn Speicher schon angelegt, Speicher um eine double Variable vergroessern
	if (*memory == NULL) {
		*memory = (double *) malloc((0 * sizeof(double)));
	} else {
		*memory = (double *) realloc(*memory, ((*memory_count + 1) * sizeof(double)));
	}
	if (*memory != NULL) {
		(*memory)[*memory_count] = number;																									// Zahl in memory speichern
		printf("%c = %lf\n", (*memory_count + 97), (*memory)[*memory_count]);
		(*memory_count)++;
	} else {
		return MEMORY_ALLOC_ERROR;																													// Rueckagabe -1 falls Memory allocation fehlschlaegt
	}
	return 0;
}
