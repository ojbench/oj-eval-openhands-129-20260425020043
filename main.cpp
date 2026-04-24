

#include <iostream>
#include <vector>
#include <string>
#include <stack>
#include <map>
#include <cctype>

using namespace std;

// Structure to represent a variable with its type and value
struct Variable {
    string type;
    string value;
    
    Variable(string t, string v) : type(t), value(v) {}
};

int main() {
    // Speed up I/O
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    
    // Stack of scopes - each scope is a map from variable name to Variable
    stack<map<string, Variable>> scopes;
    
    // Start with global scope
    scopes.push(map<string, Variable>());
    
    int n;
    cin >> n;
    cin.ignore(); // Ignore the newline after n
    
    for (int i = 0; i < n; i++) {
        string line;
        getline(cin, line);
        
        // Parse the command
        vector<string> tokens;
        string token;
        for (char c : line) {
            if (c == ' ') {
                if (!token.empty()) {
                    tokens.push_back(token);
                    token = "";
                }
            } else {
                token += c;
            }
        }
        if (!token.empty()) {
            tokens.push_back(token);
        }
        
        if (tokens.empty()) continue;
        
        string command = tokens[0];
        
        if (command == "Indent") {
            // Create a new scope
            scopes.push(map<string, Variable>());
        }
        else if (command == "Dedent") {
            // Exit the current scope
            if (scopes.size() > 1) { // Can't exit global scope
                scopes.pop();
            } else {
                cout << "Invalid operation" << endl;
            }
        }
        else if (command == "Declare" && tokens.size() == 4) {
            string type = tokens[1];
            string name = tokens[2];
            string value = tokens[3];
            
            // Check if type is valid
            if (type != "int" && type != "string") {
                cout << "Invalid operation" << endl;
                continue;
            }
            
            // Check if value is valid for the type
            if (type == "int") {
                // Check if value is a valid integer
                bool valid = true;
                if (value.empty()) valid = false;
                int start = 0;
                if (value[0] == '-' || value[0] == '+') {
                    if (value.length() == 1) valid = false;
                    start = 1;
                }
                for (int j = start; j < value.length(); j++) {
                    if (!isdigit(value[j])) {
                        valid = false;
                        break;
                    }
                }
                if (!valid) {
                    cout << "Invalid operation" << endl;
                    continue;
                }
            } else if (type == "string") {
                // Check if value is a valid string (starts and ends with ")
                if (value.length() < 2 || value[0] != '"' || value[value.length()-1] != '"') {
                    cout << "Invalid operation" << endl;
                    continue;
                }
                // Remove the quotes for storage
                value = value.substr(1, value.length() - 2);
            }
            
            // Check if variable already exists in current scope
            if (scopes.top().find(name) != scopes.top().end()) {
                cout << "Invalid operation" << endl;
                continue;
            }
            
            // Add variable to current scope
            // First check if variable already exists in current scope
            if (scopes.top().find(name) != scopes.top().end()) {
                cout << "Invalid operation" << endl;
                continue;
            }
            // If not, insert it
            scopes.top().insert(pair<string, Variable>(name, Variable(type, value)));
        }
        else if (command == "Add" && tokens.size() == 4) {
            string result = tokens[1];
            string val1 = tokens[2];
            string val2 = tokens[3];
            
            // Find variables in scope (search from current scope outwards)
            auto findVariable = [&](const string& name) -> pair<bool, Variable> {
                // Check if it's a literal string (starts with ")
                if (name.length() >= 2 && name[0] == '"' && name[name.length()-1] == '"') {
                    string strVal = name.substr(1, name.length() - 2);
                    return {true, Variable("string", strVal)};
                }
                
                // Check if it's a literal integer
                bool isInt = true;
                if (name.empty()) isInt = false;
                int start = 0;
                if (name[0] == '-' || name[0] == '+') {
                    if (name.length() == 1) isInt = false;
                    start = 1;
                }
                for (int j = start; j < name.length(); j++) {
                    if (!isdigit(name[j])) {
                        isInt = false;
                        break;
                    }
                }
                if (isInt) {
                    return {true, Variable("int", name)};
                }
                
                // Look up in scopes
                stack<map<string, Variable>> tempScopes = scopes;
                while (!tempScopes.empty()) {
                    auto& scope = tempScopes.top();
                    if (scope.find(name) != scope.end()) {
                        return {true, scope[name]};
                    }
                    tempScopes.pop();
                }
                return {false, Variable("", "")};
            };
            
            auto result1 = findVariable(val1);
            auto result2 = findVariable(val2);
            bool found1 = result1.first;
            Variable var1 = result1.second;
            bool found2 = result2.first;
            Variable var2 = result2.second;
            
            if (!found1 || !found2) {
                cout << "Invalid operation" << endl;
                continue;
            }
            
            // Check if types match
            if (var1.type != var2.type) {
                cout << "Invalid operation" << endl;
                continue;
            }
            
            // Find result variable in current scope
            if (scopes.top().find(result) == scopes.top().end()) {
                cout << "Invalid operation" << endl;
                continue;
            }
            
            Variable& resultVar = scopes.top()[result];
            
            // Check if result type matches
            if (resultVar.type != var1.type) {
                cout << "Invalid operation" << endl;
                continue;
            }
            
            // Perform addition
            if (var1.type == "int") {
                int v1 = stoi(var1.value);
                int v2 = stoi(var2.value);
                resultVar.value = to_string(v1 + v2);
            } else { // string
                resultVar.value = var1.value + var2.value;
            }
        }
        else if (command == "SelfAdd" && tokens.size() == 3) {
            string name = tokens[1];
            string value = tokens[2];
            
            // Find variable in scope
            if (scopes.top().find(name) == scopes.top().end()) {
                cout << "Invalid operation" << endl;
                continue;
            }
            
            Variable& var = scopes.top()[name];
            
            // Check if value is valid for the type
            if (var.type == "int") {
                // Check if value is a valid integer
                bool valid = true;
                if (value.empty()) valid = false;
                int start = 0;
                if (value[0] == '-' || value[0] == '+') {
                    if (value.length() == 1) valid = false;
                    start = 1;
                }
                for (int j = start; j < value.length(); j++) {
                    if (!isdigit(value[j])) {
                        valid = false;
                        break;
                    }
                }
                if (!valid) {
                    cout << "Invalid operation" << endl;
                    continue;
                }
                
                // Perform addition
                int v1 = stoi(var.value);
                int v2 = stoi(value);
                var.value = to_string(v1 + v2);
            } else { // string
                // Check if value is a valid string (starts and ends with ")
                if (value.length() < 2 || value[0] != '"' || value[value.length()-1] != '"') {
                    cout << "Invalid operation" << endl;
                    continue;
                }
                
                // Perform concatenation
                string strVal = value.substr(1, value.length() - 2);
                var.value = var.value + strVal;
            }
        }
        else if (command == "Print" && tokens.size() == 2) {
            string name = tokens[1];
            
            // Find variable in scope (search from current scope outwards)
            bool found = false;
            Variable var("", "");
            stack<map<string, Variable>> tempScopes = scopes;
            while (!tempScopes.empty()) {
                auto& scope = tempScopes.top();
                if (scope.find(name) != scope.end()) {
                    var = scope[name];
                    found = true;
                    break;
                }
                tempScopes.pop();
            }
            
            if (!found) {
                cout << "Invalid operation" << endl;
            } else {
                cout << name << ":" << var.value << endl;
            }
        }
        else {
            cout << "Invalid operation" << endl;
        }
    }
    
    return 0;
}
