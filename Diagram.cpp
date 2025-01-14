#include "Diagram.h"

Diagram::Diagram(Scaner* scaner, Tree* tree)
{
	this->scaner = scaner;
	this->tree = tree;
}

int Diagram::LookForward(int pos) 
{
	type_lex lex;
	int saved_pointer = scaner->GetUK();
	int next_type;
	for (int i = 0; i < pos; i++)
		next_type = scaner->UseScaner(lex);
	scaner->PutUK(saved_pointer);
	return next_type;
}

int Diagram::Scan(type_lex lex) 
{
	return scaner->UseScaner(lex);
}

void Diagram::Program() 
{
	type_lex lex;
	int type;
	type = LookForward(1);
	while (type == typeInt || type == typeShort || type == typeLong || type == typeBool) 
	{
		Description();
		type = LookForward(1);
	}
	if (type != typeEnd) 
	{
		type = Scan(lex);
		scaner->PrintError("ожидался конец, ", lex);
	}
}

void Diagram::Description() 
{
	type_lex lex;
	int type = LookForward(1);
	int simbol = LookForward(3);
	if ((type == typeInt || type == typeShort || type == typeLong || type == typeBool) && simbol != typeLeftBracket) 
	{
		Data();
		return;
	}
	if ((type == typeInt || type == typeShort || type == typeLong || type == typeBool) && simbol == typeLeftBracket) 
	{
		Function();
		return;
	}
	type = Scan(lex);
	scaner->PrintError("ожидался тип (int, short, long, bool), ", lex);
}

void Diagram::List()
{
	type_lex lex;
	type_lex* lexData;
	int type, pointer;
	int typeData = LookForward(1);
	Scan(lex);
	Variable(typeData);
	type = LookForward(1);
	while (type == typeComma)
	{
		type = Scan(lex);
		Variable(typeData);
		type = LookForward(1);
	}
}

void Diagram::Data()
{
	type_lex lex;
	int type;
	Type();
	List();
	type = Scan(lex);
	if (type != typeSemicolon)
	{
		scaner->PrintError("ожидалась ;, ", lex);
	}
}

void Diagram::Function() {
	type_lex lex;
	int type = Scan(lex);
	if (type != typeInt && type != typeShort && type != typeLong && type != typeBool) {
		scaner->PrintError("ожидался тип (int, short, long, bool), ", lex);
	}
	int type1 = type;
	type = Scan(lex);
	if (type != typeId && type != typeMain) {
		scaner->PrintError("ожидался идентификатор, ", lex);
	}

	if (tree->IsDoublicateId(tree, lex)) {
		scaner->PrintError("Переопределение", lex);
	}

	// Создаём новый узел для функции
	Node* newNode = new Node();
	newNode->id = lex;  // Назначаем идентификатор
	newNode->objectType = OBJ_FUNC;
	newNode->dataType = tree->GetDataType(type1);

	// Вставляем узел в дерево
	tree->SetLeft(newNode);
	tree = tree->GetLeft(); // Переход к новому узлу
	tree->SetRight(NULL);

	Tree* tmpTree = tree; // Сохраняем текущий указатель дерева
	tree = tree->GetRight(); // Переход к правому поддереву

	type = Scan(lex);
	if (type != typeLeftBracket) {
		scaner->PrintError("ожидалась (, ", lex);
	}

	// Обработка параметров функции
	type = LookForward(1);
	if (type != typeRightBracket) { // Если есть параметры
		ParameterList();
	}

	type = Scan(lex);
	if (type != typeRightBracket) {
		scaner->PrintError("ожидалась ), ", lex);
	}

	type = Scan(lex);
	if (type != typeLeftBrace) {
		scaner->PrintError("ожидалась {, ", lex);
	}

	/*bool fReturn = false;
	type1 = type;
	int lvl = 1;
	type = LookForward(1);

	while (type != typeRightBrace)
	{
		if (type != typeReturn)
		{
			type = LookForward(lvl);
			lvl++;
		}
		else {
			fReturn = true;
			break;
		}
	}

	if (!fReturn) { scaner->PrintError("нет return, ", ":)"); }
	type = type1;*/

	OperatorsAndDescriptions();

	// Возвращаемся к предыдущему узлу
	tree = tmpTree;

	type = Scan(lex);
	if (type != typeRightBrace) {
		scaner->PrintError("ожидалась }, ", lex);
	}
}

void Diagram::ParameterList() {
	type_lex lex;
	int type;

	// Обработка первого параметра
	Parameter();
	type = LookForward(1);

	// Обработка остальных параметров через запятую
	while (type == typeComma) {
		type = Scan(lex);
		Parameter();
		type = LookForward(1);
	}
}

void Diagram::Parameter() {
	type_lex lex;
	int type = LookForward(1);

	// Проверка типа параметра
	if (type != typeInt && type != typeShort && type != typeLong && type != typeBool) {
		type = Scan(lex);
		scaner->PrintError("ожидался тип параметра (int, short, long, bool), ", lex);
	}
	type_data typeData = tree->GetDataType(type);
	
	type = Scan(lex); // Считываем идентификатор
	type = LookForward(1);
	if (type != typeId) {
		scaner->PrintError("ожидался идентификатор параметра, ", lex);
	}
	type = Scan(lex);

	// Добавляем параметр в дерево
	Node* paramNode = new Node();
	paramNode->id = lex;
	paramNode->objectType = OBJ_VAR; // Параметры обрабатываются как переменные
	paramNode->dataType = typeData;
	tree->SetLeft(paramNode);
	tree = tree->GetLeft();
	tree->SetRight(NULL);
}

void Diagram::Type()
{
	type_lex lex;
	int type;
	type = LookForward(1);
	if (type != typeInt && type != typeShort && type != typeLong && type != typeBool)
	{
		scaner->PrintError("ожидался тип (int, short, long, float), ", lex);
	}
}

void Diagram::Variable(int typeData1)
{
	type_lex lex;
	Node* newNode = new Node();
	int type;
	if (typeData1 != typeInt && typeData1 != typeShort && typeData1 != typeLong && typeData1 != typeBool)
	{
		scaner->PrintError("ожидался тип (int, short, long, float), ", lex);
	}
	if (typeData1 == typeInt)
		newNode->dataType = TYPE_INTEGER;
	else if (typeData1 == typeShort)
		newNode->dataType = TYPE_SHORT;
	else if (typeData1 == typeLong)
		newNode->dataType = TYPE_LONG;
	else if (typeData1 == typeBool)
		newNode->dataType = TYPE_BOOL;
	else
		newNode->dataType = TYPE_UNKNOWN;
	type = LookForward(1);
	type_data typeData = tree->GetDataType(type);

	if (type != typeId)
	{
		type = Scan(lex);
		scaner->PrintError("ожидался идентификатор, ", lex);
	}

	int pointer = scaner->GetUK();
	type = Scan(lex);

	if (tree->IsDoublicateId(tree, lex)) {
		tree->PrintError("Переопределение", lex);
	}

	newNode->id = lex;  // Устанавливаем идентификатор
	newNode->objectType = OBJ_VAR;

	type = LookForward(1);
	if (type == typeEval) {
		newNode->flagInit = 1;
	}
	else
	{
		newNode->flagInit = 0;
	}

	// Добавляем узел в левое поддерево
	tree->SetLeft(newNode);

	// Переход к левому дочернему узлу для дальнейших операций
	tree = tree->GetLeft();

	scaner->PutUK(pointer);

	type = LookForward(2);
	if (type == typeEval)
	{
		Assignment();
		return;
	}
	type = Scan(lex);
}

void Diagram::Assignment()
{
	type_lex lex;
	int type = Scan(lex);

	if (type != typeId)
	{
		scaner->PrintError("ожидался идентификатор, ", lex);
	}

	Tree* node = tree->FindUp(lex);

	if (node == NULL) {
		scaner->PrintError("Семантическая ошибка. ID не найден", lex);
	}

	if (node->GetSelfObjectType() != OBJ_VAR) {
		scaner->PrintError("Семантическая ошибка. Попытка использования не переменной в присваивании", lex);
	}

	node->SetInit();

	// Получаем тип переменной узла
	type_data varType = node->GetSelfDataType();
	type = Scan(lex);
	if (type != typeEval)
	{
		scaner->PrintError("ожидалось =, ", lex);
	}

	Expression();
}

void Diagram::Expression()
{
	type_lex lex;
	int type;

	Comparison();
	type = LookForward(1);
	while (type == typeEq || type == typeUnEq)
	{
		type = Scan(lex);
		Comparison();
		type = LookForward(1);

	}
}

void Diagram::CompositeOperator() 
{
	type_lex lex;
	int type = Scan(lex);
	Tree* varNode = NULL;
	type_data typeData = tree->GetDataType(type);

	if (type != typeLeftBrace)
	{
		scaner->PrintError("ожидалась {, ", lex);
	}

	Node* newNode = new Node();
	newNode->id = lex;
	newNode->objectType = OBJ_FUNC;
	newNode->dataType = typeData;
	if (varNode != NULL) newNode->pointer = varNode;
	tree->SetLeft(newNode);
	tree = tree->GetLeft();
	tree->SetRight(NULL);
	Tree* tmpTree = tree;
	tree = tree->GetRight();

	OperatorsAndDescriptions();

	type = Scan(lex);
	if (type != typeRightBrace)
	{
		scaner->PrintError("ожидалась }, ", lex);
	}
	tree = tmpTree;
}

void Diagram::OperatorsAndDescriptions() 
{
	type_lex lex;
	int type = LookForward(1);

	while (type != typeRightBrace) 
	{
		if (type == typeInt || type == typeShort || type == typeLong || type == typeBool) 
		{
			Data();
		}
		else Operator();
		type = LookForward(1);
	}
}

void Diagram::Operator()
{
	type_lex lex;
	int type = LookForward(1);

	if (type == typeReturn)
	{
		ReturnStatement();
		return;
	}

	if (type == typeSemicolon)
	{
		type = Scan(lex);
		return;
	}

	if (type == typeLeftBrace)
	{
		CompositeOperator();
		return;
	}

	if (type == typeSwitch)
	{
		Cycle();
		return;
	}

	int type2 = LookForward(2);
	if (type == typeId && type2 == typeLeftBracket)
	{
		FunctionCall();
		return;
	}

	if (type == typeId && type2 == typeEval)
	{
		Assignment();
		type = Scan(lex);
		if (type != typeSemicolon)
			scaner->PrintError("ожидалась ;, ", lex);
		return;
	}

	if (type == typeId)
	{
		Expression();
		return;
	}

	type = Scan(lex);
	scaner->PrintError("ожидался оператор, ", lex);
}

void Diagram::ReturnStatement() 
{
	type_lex lex;
	int type = Scan(lex);

	if (type != typeReturn) 
	{
		scaner->PrintError("ожидалось return, ", lex);
	}

	Expression();

	type = Scan(lex);
	if (type != typeSemicolon) 
	{
		scaner->PrintError("ожидалась ; после return <выражение>, ", lex);
	}
}

void Diagram::Cycle() {
	
	type_lex lex;
	int type = Scan(lex);

	if (type != typeSwitch) {
		scaner->PrintError("ожидалось switch, ", lex);
	}

	type = Scan(lex);
	if (type != typeLeftBracket) {
		scaner->PrintError("ожидалась (, ", lex);
	}

	Expression();

	type = Scan(lex);
	if (type != typeRightBracket) {
		scaner->PrintError("ожидалась ), ", lex);
	}

	type = Scan(lex);
	if (type != typeLeftBrace) {
		scaner->PrintError("ожидалась {, ", lex);
	}

	type = LookForward(1);
	while (type == typeCase) {
		HandleCase();
		type = LookForward(1);
	}

	if (type == typeDefault) {
		HandleDefault();
		type = LookForward(1);
	}

	if (type != typeRightBrace) {
		scaner->PrintError("ожидалась }, ", lex);
	}

	type = Scan(lex);
}

void Diagram::HandleCase() {
	type_lex lex;
	int type = Scan(lex);

	if (type != typeCase) {
		scaner->PrintError("ожидался case, ", lex);
	}

	type = Scan(lex);
	if (type != constInt && type != constBool) {
		scaner->PrintError("ожидалась константа для case, ", lex);
	}

	type = Scan(lex);
	if (type != typeColon) {
		scaner->PrintError("ожидался :, ", lex);
	}

	type = LookForward(1);
	while (type != typeCase && type != typeDefault && type != typeRightBrace) {
		if (type == typeBreak) {
			type = Scan(lex);
			type = LookForward(1);
			if (type != typeSemicolon) {
				scaner->PrintError("ожидалась ;, ", lex);
			}
			//type = Scan(lex);
		}
		Operator();
		type = LookForward(1);
	}
}

void Diagram::HandleDefault() {
	type_lex lex;
	int type = Scan(lex);

	if (type != typeDefault) {
		scaner->PrintError("ожидался default, ", lex);
	}

	type = Scan(lex);
	if (type != typeColon) {
		scaner->PrintError("ожидался :, ", lex);
	}

	type = LookForward(1);
	while (type != typeRightBrace) {
		Operator();
		type = LookForward(1);
	}
}

void Diagram::FunctionCall() {
	type_lex lex;
	int type = Scan(lex);

	if (type != typeId) {
		scaner->PrintError("ожидался идентификатор функции, ", lex);
	}

	Tree* methodNode = tree->FindUp(lex); // Находим узел функции
	if (methodNode == NULL || methodNode->GetSelfObjectType() != OBJ_FUNC) {
		scaner->PrintError("Функция не найдена или не является функцией, ", lex);
	}

	type = Scan(lex);
	if (type != typeLeftBracket) {
		scaner->PrintError("ожидалась (, ", lex);
	}

	// Обработка списка аргументов
	type = LookForward(1);

	if (type != typeRightBracket) { // Если есть аргументы
		ArgumentList();
	}

	type = Scan(lex);
	if (type != typeRightBracket) {
		scaner->PrintError("ожидалась ), ", lex);
	}

	type = Scan(lex);
	if (type != typeSemicolon) {
		scaner->PrintError("ожидалась ; после вызова функции, ", lex);
	}
}

void Diagram::FunctionCallInExp() {
	type_lex lex;
	int type = Scan(lex);

	if (type != typeId) {
		scaner->PrintError("ожидался идентификатор функции, ", lex);
	}

	Tree* methodNode = tree->FindUp(lex); // Находим узел функции
	if (methodNode == NULL || methodNode->GetSelfObjectType() != OBJ_FUNC) {
		scaner->PrintError("Функция не найдена или не является функцией, ", lex);
	}

	type = Scan(lex);
	if (type != typeLeftBracket) {
		scaner->PrintError("ожидалась (, ", lex);
	}

	// Обработка списка аргументов
	type = LookForward(1);

	if (type != typeRightBracket) { // Если есть аргументы
		ArgumentList();
	}

	type = Scan(lex);
	if (type != typeRightBracket) {
		scaner->PrintError("ожидалась ), ", lex);
	}
}

void Diagram::ArgumentList() {
	type_lex lex;
	int type;

	// Обработка первого аргумента
	Expression(); // Аргумент передается как выражение
	type = LookForward(1);

	// Обработка остальных аргументов через запятую
	while (type == typeComma) {
		type = Scan(lex);
		Expression();
		type = LookForward(1);
	}
}

void Diagram::Comparison() 
{
	type_lex lex;
	Summand();
	int type = LookForward(1);
	while (type == typeLess || type == typeLessOrEq || type == typeMore || type == typeMoreOrEq) 
	{
		type = Scan(lex);
		Summand();
		type = LookForward(1);
	}
}

void Diagram::LogicalOr() {
	type_lex lex;
	LogicalAnd();
	int type = LookForward(1);
	while (type == typeOr) { // Логическое "или"
		type = Scan(lex);
		LogicalAnd();
		type = LookForward(1);
	}
}

void Diagram::LogicalAnd() {
	type_lex lex;
	Comparison();
	int type = LookForward(1);
	while (type == typeAnd) { // Логическое "и"
		type = Scan(lex);
		Comparison();
		type = LookForward(1);
	}
}

void Diagram::Summand() 
{
	type_lex lex;
	int type;
	Multiplier();
	type = LookForward(1);
	while (type == typePlus || type == typeMinus) 
	{
		type = Scan(lex);
		Multiplier();
		type = LookForward(1);
	}
}

void Diagram::Multiplier()
{
	type_lex lex;
	int type;
	UnaryOperation();
	type = LookForward(1);
	while (type == typeMul || type == typeDiv || type == typeMod)
	{
		type = Scan(lex);
		Tree* node = tree->FindUp(lex);
		UnaryOperation();
		type = LookForward(1);
	}
}

void Diagram::UnaryOperation()
{
	type_lex lex;
	int type = LookForward(1);

	if (type == typePlus || type == typeMinus)
	{
		type = Scan(lex);
		ElementaryExpression();
	}
	else
	{
		ElementaryExpression();
	}
}

void Diagram::ElementaryExpression() 
{
	type_lex lex;
	int type = LookForward(1);
	if (type == typeId) 
	{
		if (LookForward(2) == typeLeftBracket) {
			FunctionCallInExp();
			return;
		}
		type = Scan(lex);
		Tree* node = tree->FindUp(lex);
		if (node == nullptr)
		{
			scaner->PrintError("Semant Error. Variable not found", lex);
		}
		if(node != NULL)

		if (node->IsSelfInit() == 0)
		{
			scaner->PrintError("Semant Error. Variable is not initialized", lex);
		}
		return;
	}
	if (type == typeShort || type == typeBool || type == typeInt || type == typeLong || type == constBool || type == constInt)
	{
		type = Scan(lex);
		return;
	}
	if (type == typeLeftBracket) 
	{
		type = Scan(lex);
		Expression();
		type = Scan(lex);
		if (type != typeRightBracket)
		{
			scaner->PrintError("ожидалась ), ", lex);
		}
		return;
	}
	type = Scan(lex);
	scaner->PrintError("ожидалось выражение, ", lex);
}