#include <iostream>
//выражение->терм[+терм][-терм]
//терм->фактор[*фактор][/ фактор]
//фактор->переменная, число или выражение

enum types { DELIMITER = 1, VARIABLE, NUMBER };

class Parser {
	char* expPtr;
	char token[80];
	char tokType;

	void getToken() {
		char* temp;
		tokType = 0;
		temp = token;
		*temp = '\0';
		if (!*expPtr)return;
		while (isspace(*expPtr))++expPtr;
		if (strchr("+-*/%^=()", *expPtr)) {
			tokType = DELIMITER;
			*temp++ = *expPtr++;
		}
		else if (isalpha(*expPtr)) {
			while (!isDelim(*expPtr)) *temp++ = *expPtr++;
			tokType = VARIABLE;
		}
		else if (isdigit(*expPtr)) {
			while (!isDelim(*expPtr)) *temp++ = *expPtr++;
			tokType = NUMBER;
		}
		*temp = '\0';
	}

	bool isDelim(char c) {
		if (strchr(" +-/*%^=()", c))
			return 1;
		return 0;
	}

	void sError(int error) {
		static const char* e[] = {
			"syntax error",
			"out of balance between brackets",
			"expression is empty"
		};
		std::cout << e[error];
	}
	void evalExp2(double& result)// + - термы
	{
		double temp;
		char op;
		evalExp3(result);
		while ((op = *token) == '+' || op == '-')
		{
			getToken();
			evalExp3(temp);
			switch (op) {
			case '-':result -= temp; break;
			case '+':result += temp; break;
			}
		}
	}

	void evalExp3(double& result)// * / факторы
	{
		char op;
		double temp;
		evalExp4(result);
		while ((op = *token) == '*' || op == '/' || op == '%') {
			getToken();
			evalExp4(temp);
			switch (op) {
			case '*':result *= temp; break;
			case '/':result /= temp; break;
			case '%':result = (int)result % (int)temp; break;
			}
		}
	}

	void evalExp4(double& result)// ^ возведение в степень
	{
		double temp, ex;
		evalExp5(result);
		if (*token == '^') {
			getToken();
			evalExp4(temp);
			ex = result;
			if (temp == 0) {
				result = 1;
				return;
			}
			for (int i = (int)temp - 1; i > 0; --i)
				result *= (double)ex;
		}
	}

	void evalExp5(double& result)// + - унарно
	{
		char op = 0;
		if ((tokType == DELIMITER) && *token == '+' || *token == '-') {
			op = *token;
			getToken();
		}
		evalExp6(result);
		if (op == '-')result = -result;
	}

	void evalExp6(double& result)// скобки
	{
		if (*token == '(') {
			getToken();
			evalExp2(result);
			if (*token != ')')
				sError(1);
			getToken();
		}
		else atom(result);
	}

	void atom(double& result) // number
	{
		switch (tokType) {
		case NUMBER:
			result = atof(token);
			getToken();
			return;
		default:sError(0);
		}
	}

public:
	Parser() :expPtr(nullptr) {}

	double evalExp(char* exp) {
		double result;
		expPtr = exp;
		getToken();
		if (!*token) {
			sError(2);//+-терм
			return 0;
		}
		evalExp2(result);
		if (*token)sError(0);//last lexem must be 0
		return result;
	}
};

int main() {
	char expstr[80];
	Parser pars;
	while (true) {
		std::cout << "EXPRESSION:";
		std::cin.getline(expstr, 79);
		if (*expstr == '.')break;
		std::cout << std::endl << "ANSWER: " << pars.evalExp(expstr) << std::endl;
	}
	return 0;
}