#include <memory>
#include <stack>
#include <string>

class IOperation
{
public:
	virtual ~IOperation() = default;
	virtual double Evaluate() const noexcept = 0;
};

class Literal
	: public virtual IOperation
{
public:
	Literal(double Value)
		: value(Value)
	{
	}
	double Evaluate() const noexcept override
	{
		return value;
	}
private:
	double value;
};

class Expression
	: public virtual IOperation
{
public:
	enum class Operation
	{
		SingleOperand,
		Plus,
		Minus,
		Mul,
		Div
	};
	double Evaluate() const noexcept override
	{
		switch (operation) {
			case Operation::SingleOperand :
				return leftOperand->Evaluate();
				break;
			case Operation::Plus :
				return leftOperand->Evaluate() + rightOperand->Evaluate();
				break;
			case Operation::Minus :
				return leftOperand->Evaluate() - rightOperand->Evaluate();
				break;
			case Operation::Mul :
				return leftOperand->Evaluate() * rightOperand->Evaluate();
				break;
			case Operation::Div :
				return leftOperand->Evaluate() / rightOperand->Evaluate();
				break;
		}
	}
	std::shared_ptr<IOperation> leftOperand;
	std::shared_ptr<IOperation> rightOperand;
	Operation operation;
};

int main(int argc, char** argv)
{
	//const std::string expression = "1";
	//const std::string expression = "(1)";
	//const std::string expression = "(1)+2";
	//const std::string expression = "1+(((2)+3)+4)";
	//const std::string expression = "(((1)+2)+3)";
	const std::string expression = "((1+2)+(3+4))+((5+6)+(7+8))";
	//const std::string expression = "(1+(2+(3+4)))";
	//const std::string expression = "(1+(2+(3+4)))+5";
	//const std::string expression = "(1+2)+(3+4)";
	std::string processingStr;

	std::shared_ptr<Expression> processingExpression(new Expression());
	std::shared_ptr<IOperation> lastExpression;
	std::stack<std::shared_ptr<Expression>> brackets;

	for (const auto c : expression) {
		if (c == '+') {
			if (lastExpression == nullptr && processingStr.empty() == false) {
				lastExpression.reset(new Literal(std::stod(processingStr)));
				processingStr.clear();
			}

			if (processingExpression->leftOperand == nullptr) {
				processingExpression->operation = Expression::Operation::Plus;
				processingExpression->leftOperand = std::move(lastExpression);
			} else {
				processingExpression->rightOperand = std::move(lastExpression);

				auto newProcessingExpression = std::make_shared<Expression>();
				newProcessingExpression->leftOperand = std::move(processingExpression);
				newProcessingExpression->operation = Expression::Operation::Plus;

				processingExpression = std::move(newProcessingExpression);
			}
		} else if (c == '(') {
			brackets.push(processingExpression);
			processingExpression.reset(new Expression());
			lastExpression = nullptr;
		} else if (c == ')') {
			if (lastExpression == nullptr && processingStr.empty() == false) {
				lastExpression.reset(new Literal(std::stod(processingStr)));
				processingStr.clear();
			}

			if (processingExpression->leftOperand == nullptr) {
				processingExpression->leftOperand = std::move(lastExpression);
			} else {
				processingExpression->rightOperand = std::move(lastExpression);
			}

			lastExpression = processingExpression;

			processingExpression = brackets.top();
			brackets.pop();

		} else {
			processingStr += c;
		}
	}

	if (processingExpression->leftOperand == nullptr) {
		if (processingStr.empty() == false) {
			processingExpression->leftOperand = std::make_shared<Literal>(std::stod(processingStr));
		} else {
			processingExpression->leftOperand = std::move(lastExpression);
		}
	} else {
		if (processingStr.empty() == false) {
			processingExpression->rightOperand = std::make_shared<Literal>(std::stod(processingStr));
		} else {
			processingExpression->rightOperand = std::move(lastExpression);
		}
	}

	const auto res = processingExpression->Evaluate();
	return 0;
}
