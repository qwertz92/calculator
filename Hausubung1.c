#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define MAX_INPUT 50
#define IS_NUM 1
#define IS_CHAR 2
#define IS_OPER 3
#define QUIT_STRING "quit"
#define QUIT 0
#define MEMORY_SIZE 5
#define MEMORY_FULL 1


int f_check_input(char *token);
void f_delete_char(char *string, char zeichen);
void f_move_doublearray(double *array, size_t n);
int f_memory(double *memory, int *memory_count, double number);

int main() {
	char input_string[MAX_INPUT] = {};
	char delimiter[] = {' ', '\n', '\0'};
	char *token = NULL;
	char operator[15] = {};
	int input[50] = {};
	int memory_count = 0;
	int i = 0, count = 0;
	double *memory = NULL;
	double number[15] = {};
	double result = 0;

	memory = (double *) malloc((0 * sizeof(double)));

	printf("Geben Sie maximal 26 Nummern ein (Beenden mit quit): \n");
	if (fgets(input_string, MAX_INPUT, stdin) == NULL) {
		printf("Error Reading!\n");
		return -1;
	}

	token = strtok(input_string, delimiter);
	if(!token) {
		printf("Keine Werte in den Speicher eingelesen.\n");
	} else {
		for (count = 0; token != NULL && count < MAX_INPUT; count++) {
			input[count] = f_check_input(token);
			switch (input[count]) {
				case IS_NUM:
					input[count] = atof(token);
					token = strtok(NULL, delimiter);
					break;
				case QUIT:
					free(memory);
					return 0;
				default:
					free(memory);
					printf("Es wurden falsche Eingaben getaetigt!\n");
					return -1;
			}
		}
		if (count < MEMORY_SIZE) {
			for (i = 0; i < count; i++) {
				if(f_memory(memory, &memory_count, input[i]) == MEMORY_FULL) {
					printf("Memory full!\n");
					free(memory);
					return 0;
				}
			}
		}
	}

	while (1) {
		printf("> ");
		if (fgets(input_string, MAX_INPUT, stdin) == NULL) {
			printf("Error Reading!\n");
			return -1;
		}
		token = strtok(input_string, delimiter);
		if(!token) {
			printf("ERROR token is NULL\n");
		} else {
			for (count = 0, i = input[count]; i != -1 && token != NULL; count++) {
				input[count] = f_check_input(token);
				switch (input[count]) {
					case IS_NUM:
						if ((count % 2) == 0) {
							number[(count / 2)] = atof(token);
						} else {
							input[count] = -1;
						}
						break;
					case IS_CHAR:
						if ((count % 2) == 0 && ((*token) - 'a') < memory_count) {
							number[(count / 2)] = memory[((*token) - 'a')];
						} else {
							printf("Fuck\n");
							input[count] = -1;
						}
						break;
					case IS_OPER:
						if ((count % 2) == 1) {
							operator[((count - 1) / 2)] = *token;
						} else {
							input[count] = -1;
						}
						break;
					case QUIT:
						free(memory);
						return 0;
					}
				// Naechsten token einlesen
				token = strtok(NULL, delimiter);
				i = input[count];
			}
			// Berechnung
			// Soll nur durchgefuehrt werden wenn der kein Input fehlerhaft war und mindestens 3(immer ungerade(5,7,9)) Zeichen eingegeben wurde (2 + 1)
			if ((input[count - 1] != -1) && (count % 2 == 1) && count > 2) {
				for (i = 0; i < ((count - 1) / 2); i++) {
					if (operator[i] == '*') {
						number[i + 1] = number[i] * number[(i + 1)];
						f_delete_char(operator, '*');
						f_move_doublearray((number + i), (((count - 1) / 2) - i));
						count -= 2;
						i--;
					}
					if (operator[i] == '/') {
						number[i + 1] = number[i] / number[(i + 1)];
						f_delete_char(operator, '/');
						f_move_doublearray((number + i), (((count - 1) / 2) - i));
						count -= 2;
						i--;
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
			f_memory(memory, &memory_count, result);
		}
	}

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
		}
	} else {
		for (i = 0; i < length; i++) {
			if ((*(token + i) < 48 || *(token + i) > 57) && *(token + i) != '.') {					// Check ob auch wirklich nur Zahlen eingegeben wurden
				if(strcmp(token, QUIT_STRING) == 0) {
					return QUIT;
				}
				return -1;
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


void f_delete_char(char *string, char zeichen) {
  int i = 0, j = 0;
  char *position = string;
  int length = strlen(string);
  for (i = 0; i < length; i++) {
    position = strchr(position, zeichen);                 // strchr(string, char) Sucht ein Zeichen (char) in einem String und gibt die Adresse des ersten ubereinstimmenden Elemetes zurueck, bei keiner uebereinstimmung wird NULL zurueckgegeben
    if (position != NULL) {                               // Wenn ein Wert gefunden wurde werden alle nachfolgenden Werte des Arrays um 1 Element nach vorne geschoben und das gesuchte ersetzt
      i = (position - string);                            // i auf den Index Wert des Elementes setzen
      for (j = 0; j < (length - i); j++) {                // length - i: Anzahl nachfolgender Elemente im Array (strlen(position))
        *(position + j) = *(position + (j + 1));          // Werte um 1 Element nach links verschieben (inklusive '\0')
      }
      i = length;                                         // i auf Laenge des Strings setzen um Schleife zu beenden
    }
  }
}

void f_move_doublearray(double *array, size_t n) {
	int i = 0;
	for (i = 0; i < n; i++) {
		array[i] = array[i + 1];
	}
}

int f_memory(double *memory, int *memory_count, double number) {
	memory = (double *) realloc(memory, ((*memory_count) + 1));				// Speicher um eine double Variable vergroessern
	if (memory != NULL) {
		memory[*memory_count] = number;																	// Zahl in memory speichern
		printf("%c = %lf\n", (*memory_count + 97), memory[*memory_count]);
		(*memory_count)++;
		if (*memory_count >= (MEMORY_SIZE)) {														// Ueberpruefen ob memory voll ist
			return MEMORY_FULL;
		}
	} else {
		return MEMORY_FULL;																							// Rueckagabe -1 falls Memory allocation fehlschlaegt
	}
	return 0;
}
