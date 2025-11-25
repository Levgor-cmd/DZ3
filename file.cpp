#include <iostream>
#include <string>
#include <regex>
#include <vector>
#include <stack>
#include <cmath>
#include <map>

std::regex NUM_REGEX(R"(\d+\.?\d*|\.\d+)");
std::regex OPER_REGEX(R"(\+|\-|\*|\/)");
std::regex FUNC_REGEX(R"(sin|cos|tg|ctg|exp)");
std::regex CONST_REGEX(R"(E|PI)");
std::regex STAPLES_REGEX(R"(\(|\))");
std::regex VAR_REGEX(R"(X)");

struct Token
{
    std::string type;
    std::string value;
    int position;
};

std::vector<std::string> findNumbers(std::string exp)
{
    std::vector<std::string> result;
    auto begin = std::sregex_iterator(exp.begin(), exp.end(), NUM_REGEX);
    auto end = std::sregex_iterator();

    for (auto it = begin; it != end; ++it)
    {
        result.push_back(it->str());
    }
    return result;
}

std::vector<std::string> findOperators(std::string exp)
{
    std::vector<std::string> result;
    auto begin = std::sregex_iterator(exp.begin(), exp.end(), OPER_REGEX);
    auto end = std::sregex_iterator();

    for (auto it = begin; it != end; ++it)
    {
        result.push_back(it->str());
    }
    return result;
}

std::vector<std::string> findFunctions(std::string exp)
{
    std::vector<std::string> result;
    auto begin = std::sregex_iterator(exp.begin(), exp.end(), FUNC_REGEX);
    auto end = std::sregex_iterator();

    for (auto it = begin; it != end; ++it)
    {
        result.push_back(it->str());
    }
    return result;
}

std::vector<std::string> findConstants(std::string exp)
{
    std::vector<std::string> result;
    auto begin = std::sregex_iterator(exp.begin(), exp.end(), CONST_REGEX);
    auto end = std::sregex_iterator();

    for (auto it = begin; it != end; ++it)
    {
        result.push_back(it->str());
    }
    return result;
}

std::vector<std::string> findStaples(std::string exp)
{
    std::vector<std::string> result;
    auto begin = std::sregex_iterator(exp.begin(), exp.end(), STAPLES_REGEX);
    auto end = std::sregex_iterator();

    for (auto it = begin; it != end; ++it)
    {
        result.push_back(it->str());
    }
    return result;
}

std::vector<Token> findTokensWithPositions(const std::string& exp, const std::regex& REGEX, const std::string type)
{
    std::vector<Token> result;

    auto begin = std::sregex_iterator(exp.begin(), exp.end(), REGEX);
    auto end = std::sregex_iterator();

    for (auto it = begin; it != end; ++it)
    {
        result.push_back({ type, it->str(), (int)it->position() });
    }
    return result;
}

int getPriority(const Token& token)
{
    if (token.type == "UNARY_MINUS") return 4;
    if (token.type == "FUNCTION") return 3;
    if (token.value == "*" || token.value == "/") return 2;
    if (token.value == "+" || token.value == "-") return 1;
    return 0;
}

void detectUnaryMinus(std::vector<Token>& tokens)
{
    for (int i = 0; i < tokens.size(); i++)
    {
        if (tokens[i].value == "-")
        {
            bool isUnary = false;

            if (i == 0) isUnary = true;
            else if (tokens[i - 1].value == "(") isUnary = true;
            else if (tokens[i - 1].type == "OPERATOR") isUnary = true;
            else if (tokens[i - 1].type == "FUNCTION") isUnary = true;

            if (isUnary)
            {
                tokens[i].type = "UNARY_MINUS";
                tokens[i].value = "~";
            }
        }
    }
}

std::vector<Token> infixToPostfix(const std::vector<Token>& tokens)
{
    std::vector<Token> output;
    std::stack<Token> stack;

    for (const Token& token : tokens)
    {
        if (token.type == "NUMBER" || token.type == "CONSTANT")
        {
            output.push_back(token);
        }

        else if (token.type == "VARIABLE")
        {
            output.push_back(token);
        }

        else if (token.type == "UNARY_MINUS")
        {
            stack.push(token);
        }

        else if (token.type == "FUNCTION")
        {
            stack.push(token);
        }

        else if (token.value == "(")
        {
            stack.push(token);
        }

        else if (token.value == ")")
        {
            while (!stack.empty() && stack.top().value != "(")
            {
                output.push_back(stack.top());
                stack.pop();
            }
            stack.pop();
        }
        else if (token.type == "OPERATOR")
        {
            while (!stack.empty() && stack.top().value != "(" && getPriority(stack.top()) >= getPriority(token))
            {
                output.push_back(stack.top());
                stack.pop();
            }
            stack.push(token);
        }
    }

    while (!stack.empty())
    {
        output.push_back(stack.top());
        stack.pop();
    }

    return output;
}

double evaluatePostfix(const std::vector<Token>& postfix, double x_value)
{
    std::stack<double> stack;
    std::map<std::string, double> constants = {
        {"PI", 3.141592653589793},
        {"E", 2.718281828459045}
    };

    for (const Token& token : postfix)
    {
        if (token.type == "NUMBER")
        {
            stack.push(std::stod(token.value));
        }
        else if (token.type == "CONSTANT")
        {
            stack.push(constants[token.value]);
        }
        else if (token.type == "UNARY_MINUS")
        {
            double a = stack.top(); stack.pop();
            stack.push(-a);
        }
        else if (token.type == "VARIABLE")
        {
            stack.push(x_value);
        }
        else if (token.type == "OPERATOR")
        {
            double b = stack.top(); stack.pop();
            double a = stack.top(); stack.pop();

            if (token.value == "+") stack.push(a + b);
            else if (token.value == "-") stack.push(a - b);
            else if (token.value == "*") stack.push(a * b);
            else if (token.value == "/") stack.push(a / b);
        }
        else if (token.type == "FUNCTION")
        {
            double a = stack.top(); stack.pop();

            if (token.value == "sin") stack.push(sin(a));
            else if (token.value == "cos") stack.push(cos(a));
            else if (token.value == "tg") stack.push(tan(a));
            else if (token.value == "ctg") stack.push(1.0 / tan(a));
            else if (token.value == "exp") stack.push(exp(a));
        }
    }
    return stack.top();
}

int main()
{
    std::cout << "Enter your expression with allowed operations without spaces:\n+\t-\t*\t/\t( ... )\nsin\tcos\ttg\tctg\texp\nConst values: E\t\tPI\nVariable:\tX\n" << std::endl;
    std::cout << "Warning: please enter your expression in correct case" << std::endl;
    std::string exp;
    std::cin >> exp;

    std::vector<Token> allTokens;
    auto numbers = findTokensWithPositions(exp, NUM_REGEX, "NUMBER");
    auto operators = findTokensWithPositions(exp, OPER_REGEX, "OPERATOR");
    auto functions = findTokensWithPositions(exp, FUNC_REGEX, "FUNCTION");
    auto constants = findTokensWithPositions(exp, CONST_REGEX, "CONSTANT");
    auto staples = findTokensWithPositions(exp, STAPLES_REGEX, "STAPLE");

    for (auto i : numbers) allTokens.push_back(i);
    for (auto i : operators) allTokens.push_back(i);
    for (auto i : functions) allTokens.push_back(i);
    for (auto i : constants) allTokens.push_back(i);
    for (auto i : staples) allTokens.push_back(i);

    auto variables = findTokensWithPositions(exp, VAR_REGEX, "VARIABLE");
    for (auto i : variables) allTokens.push_back(i);

    std::sort(allTokens.begin(), allTokens.end(), [](const Token& a, const Token& b) -> bool { return a.position < b.position; });

    bool has_x = false;
    for (const auto& token : allTokens)
    {
        if (token.type == "VARIABLE")
        {
            has_x = true;
            break;
        }
    }

    double x_value = 0;
    if (has_x)
    {
        std::cout << "Enter X = ";
        std::cin >> x_value;
    }

    detectUnaryMinus(allTokens);
    std::vector<Token> postfix = infixToPostfix(allTokens);

    std::cout << "Result: " << evaluatePostfix(postfix, x_value) << std::endl;

    return 0;
}
