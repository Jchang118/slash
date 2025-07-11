#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <ctime>
#include <algorithm>

using namespace std;

enum class AccountType {
    ASSET,
    LIABILITY,
    EQUITY,
    REVENUE,
    EXPENSE
};

string accountTypeToString(AccountType type) {
    switch(type) {
        case AccountType::ASSET: return "资产";
        case AccountType::LIABILITY: return "负债";
        case AccountType::EQUITY: return "权益";
        case AccountType::REVENUE: return "收入";
        case AccountType::EXPENSE: return "支出";
        default: return "未知";
    }
}

class Transaction {
private:
    static int nextId;
    int id;
    string date;
    string description;
    string debitAccount;
    string creditAccount;
    double amount;

public:
    Transaction(const string& desc, const string& debit, const string& credit, double amt)
        : id(nextId++), description(desc), debitAccount(debit), creditAccount(credit), amount(amt) {
        time_t now = time(0);
        char* timeStr = ctime(&now);
        date = string(timeStr).substr(0, 24);
    }

    int getId() const { return id; }
    string getDate() const { return date; }
    string getDescription() const { return description; }
    string getDebitAccount() const { return debitAccount; }
    string getCreditAccount() const { return creditAccount; }
    double getAmount() const { return amount; }

    void display() const {
        cout << "交易ID: " << id << " | 日期: " << date << endl;
        cout << "描述: " << description << endl;
        cout << "借方: " << debitAccount << " | 贷方: " << creditAccount << endl;
        cout << "金额: ¥" << fixed << setprecision(2) << amount << endl;
        cout << "----------------------------------------" << endl;
    }

    string toFileString() const {
        return to_string(id) + "," + date + "," + description + "," + 
               debitAccount + "," + creditAccount + "," + to_string(amount);
    }

    static Transaction fromFileString(const string& line) {
        stringstream ss(line);
        string item;
        vector<string> parts;
        
        while (getline(ss, item, ',')) {
            parts.push_back(item);
        }
        
        Transaction t(parts[2], parts[3], parts[4], stod(parts[5]));
        t.id = stoi(parts[0]);
        t.date = parts[1];
        nextId = max(nextId, t.id + 1);
        return t;
    }
};

int Transaction::nextId = 1;

class Account {
private:
    string name;
    string code;
    AccountType type;
    double balance;

public:
    Account() : name(""), code(""), type(AccountType::ASSET), balance(0.0) {}
    
    Account(const string& n, const string& c, AccountType t) 
        : name(n), code(c), type(t), balance(0.0) {}

    string getName() const { return name; }
    string getCode() const { return code; }
    AccountType getType() const { return type; }
    double getBalance() const { return balance; }

    void debit(double amount) {
        if (type == AccountType::ASSET || type == AccountType::EXPENSE) {
            balance += amount;
        } else {
            balance -= amount;
        }
    }

    void credit(double amount) {
        if (type == AccountType::LIABILITY || type == AccountType::EQUITY || type == AccountType::REVENUE) {
            balance += amount;
        } else {
            balance -= amount;
        }
    }

    void display() const {
        cout << "账户代码: " << code << " | 账户名称: " << name << endl;
        cout << "账户类型: " << accountTypeToString(type) << endl;
        cout << "账户余额: ¥" << fixed << setprecision(2) << balance << endl;
        cout << "----------------------------------------" << endl;
    }

    string toFileString() const {
        return code + "," + name + "," + to_string(static_cast<int>(type)) + "," + to_string(balance);
    }

    static Account fromFileString(const string& line) {
        stringstream ss(line);
        string item;
        vector<string> parts;
        
        while (getline(ss, item, ',')) {
            parts.push_back(item);
        }
        
        Account acc(parts[1], parts[0], static_cast<AccountType>(stoi(parts[2])));
        acc.balance = stod(parts[3]);
        return acc;
    }
};

class AccountingSystem {
private:
    map<string, Account> accounts;
    vector<Transaction> transactions;
    string dataFile = "accounting_data.txt";

public:
    void addAccount(const string& code, const string& name, AccountType type) {
        if (accounts.find(code) != accounts.end()) {
            cout << "错误：账户代码 " << code << " 已存在！" << endl;
            return;
        }
        accounts[code] = Account(name, code, type);
        cout << "账户添加成功！" << endl;
    }

    void removeAccount(const string& code) {
        auto it = accounts.find(code);
        if (it == accounts.end()) {
            cout << "错误：未找到账户代码 " << code << endl;
            return;
        }
        
        if (it->second.getBalance() != 0) {
            cout << "错误：账户余额不为零，无法删除！" << endl;
            return;
        }
        
        accounts.erase(it);
        cout << "账户删除成功！" << endl;
    }

    void addTransaction(const string& description, const string& debitCode, 
                       const string& creditCode, double amount) {
        if (accounts.find(debitCode) == accounts.end()) {
            cout << "错误：借方账户 " << debitCode << " 不存在！" << endl;
            return;
        }
        if (accounts.find(creditCode) == accounts.end()) {
            cout << "错误：贷方账户 " << creditCode << " 不存在！" << endl;
            return;
        }
        if (amount <= 0) {
            cout << "错误：交易金额必须大于零！" << endl;
            return;
        }

        accounts[debitCode].debit(amount);
        accounts[creditCode].credit(amount);
        
        transactions.push_back(Transaction(description, accounts[debitCode].getName(), 
                                         accounts[creditCode].getName(), amount));
        
        cout << "交易记录成功！" << endl;
    }

    void displayAllAccounts() const {
        if (accounts.empty()) {
            cout << "暂无账户信息。" << endl;
            return;
        }
        
        cout << "\n========== 所有账户 ==========" << endl;
        for (const auto& pair : accounts) {
            pair.second.display();
        }
    }

    void displayAccount(const string& code) const {
        auto it = accounts.find(code);
        if (it == accounts.end()) {
            cout << "错误：未找到账户代码 " << code << endl;
            return;
        }
        
        cout << "\n========== 账户详情 ==========" << endl;
        it->second.display();
    }

    void displayAllTransactions() const {
        if (transactions.empty()) {
            cout << "暂无交易记录。" << endl;
            return;
        }
        
        cout << "\n========== 所有交易记录 ==========" << endl;
        for (const auto& transaction : transactions) {
            transaction.display();
        }
    }

    void generateBalanceSheet() const {
        cout << "\n========== 资产负债表 ==========" << endl;
        
        double totalAssets = 0, totalLiabilities = 0, totalEquity = 0;
        
        cout << "\n资产：" << endl;
        for (const auto& pair : accounts) {
            if (pair.second.getType() == AccountType::ASSET) {
                cout << pair.second.getName() << ": ¥" 
                     << fixed << setprecision(2) << pair.second.getBalance() << endl;
                totalAssets += pair.second.getBalance();
            }
        }
        cout << "资产总计: ¥" << fixed << setprecision(2) << totalAssets << endl;
        
        cout << "\n负债：" << endl;
        for (const auto& pair : accounts) {
            if (pair.second.getType() == AccountType::LIABILITY) {
                cout << pair.second.getName() << ": ¥" 
                     << fixed << setprecision(2) << pair.second.getBalance() << endl;
                totalLiabilities += pair.second.getBalance();
            }
        }
        cout << "负债总计: ¥" << fixed << setprecision(2) << totalLiabilities << endl;
        
        cout << "\n所有者权益：" << endl;
        for (const auto& pair : accounts) {
            if (pair.second.getType() == AccountType::EQUITY) {
                cout << pair.second.getName() << ": ¥" 
                     << fixed << setprecision(2) << pair.second.getBalance() << endl;
                totalEquity += pair.second.getBalance();
            }
        }
        cout << "权益总计: ¥" << fixed << setprecision(2) << totalEquity << endl;
        
        cout << "\n负债和权益总计: ¥" << fixed << setprecision(2) 
             << (totalLiabilities + totalEquity) << endl;
    }

    void generateIncomeStatement() const {
        cout << "\n========== 损益表 ==========" << endl;
        
        double totalRevenue = 0, totalExpense = 0;
        
        cout << "\n收入：" << endl;
        for (const auto& pair : accounts) {
            if (pair.second.getType() == AccountType::REVENUE) {
                cout << pair.second.getName() << ": ¥" 
                     << fixed << setprecision(2) << pair.second.getBalance() << endl;
                totalRevenue += pair.second.getBalance();
            }
        }
        cout << "收入总计: ¥" << fixed << setprecision(2) << totalRevenue << endl;
        
        cout << "\n支出：" << endl;
        for (const auto& pair : accounts) {
            if (pair.second.getType() == AccountType::EXPENSE) {
                cout << pair.second.getName() << ": ¥" 
                     << fixed << setprecision(2) << pair.second.getBalance() << endl;
                totalExpense += pair.second.getBalance();
            }
        }
        cout << "支出总计: ¥" << fixed << setprecision(2) << totalExpense << endl;
        
        double netIncome = totalRevenue - totalExpense;
        cout << "\n净收益: ¥" << fixed << setprecision(2) << netIncome << endl;
    }

    void saveData() const {
        ofstream file(dataFile);
        if (!file.is_open()) {
            cout << "错误：无法保存数据到文件！" << endl;
            return;
        }
        
        file << "ACCOUNTS" << endl;
        for (const auto& pair : accounts) {
            file << pair.second.toFileString() << endl;
        }
        
        file << "TRANSACTIONS" << endl;
        for (const auto& transaction : transactions) {
            file << transaction.toFileString() << endl;
        }
        
        file.close();
        cout << "数据保存成功！" << endl;
    }

    void loadData() {
        ifstream file(dataFile);
        if (!file.is_open()) {
            cout << "数据文件不存在，将创建新的账本。" << endl;
            return;
        }
        
        string line, section;
        bool inAccounts = false, inTransactions = false;
        
        while (getline(file, line)) {
            if (line == "ACCOUNTS") {
                inAccounts = true;
                inTransactions = false;
                continue;
            } else if (line == "TRANSACTIONS") {
                inAccounts = false;
                inTransactions = true;
                continue;
            }
            
            if (inAccounts && !line.empty()) {
                Account acc = Account::fromFileString(line);
                accounts[acc.getCode()] = acc;
            } else if (inTransactions && !line.empty()) {
                transactions.push_back(Transaction::fromFileString(line));
            }
        }
        
        file.close();
        cout << "数据加载成功！" << endl;
    }
};

class MenuSystem {
private:
    AccountingSystem system;

public:
    MenuSystem() {
        system.loadData();
    }

    ~MenuSystem() {
        system.saveData();
    }

    void displayMainMenu() {
        cout << "\n========== 会计系统主菜单 ==========" << endl;
        cout << "1. 账户管理" << endl;
        cout << "2. 交易管理" << endl;
        cout << "3. 报表生成" << endl;
        cout << "4. 数据管理" << endl;
        cout << "0. 退出系统" << endl;
        cout << "请选择操作：";
    }

    void accountMenu() {
        int choice;
        do {
            cout << "\n========== 账户管理 ==========" << endl;
            cout << "1. 添加账户" << endl;
            cout << "2. 删除账户" << endl;
            cout << "3. 查看所有账户" << endl;
            cout << "4. 查看特定账户" << endl;
            cout << "0. 返回主菜单" << endl;
            cout << "请选择操作：";
            cin >> choice;

            switch (choice) {
                case 1: addAccountInterface(); break;
                case 2: removeAccountInterface(); break;
                case 3: system.displayAllAccounts(); break;
                case 4: viewAccountInterface(); break;
                case 0: break;
                default: cout << "无效选择！" << endl;
            }
        } while (choice != 0);
    }

    void transactionMenu() {
        int choice;
        do {
            cout << "\n========== 交易管理 ==========" << endl;
            cout << "1. 添加交易" << endl;
            cout << "2. 查看所有交易" << endl;
            cout << "0. 返回主菜单" << endl;
            cout << "请选择操作：";
            cin >> choice;

            switch (choice) {
                case 1: addTransactionInterface(); break;
                case 2: system.displayAllTransactions(); break;
                case 0: break;
                default: cout << "无效选择！" << endl;
            }
        } while (choice != 0);
    }

    void reportMenu() {
        int choice;
        do {
            cout << "\n========== 报表生成 ==========" << endl;
            cout << "1. 生成资产负债表" << endl;
            cout << "2. 生成损益表" << endl;
            cout << "0. 返回主菜单" << endl;
            cout << "请选择操作：";
            cin >> choice;

            switch (choice) {
                case 1: system.generateBalanceSheet(); break;
                case 2: system.generateIncomeStatement(); break;
                case 0: break;
                default: cout << "无效选择！" << endl;
            }
        } while (choice != 0);
    }

    void dataMenu() {
        int choice;
        do {
            cout << "\n========== 数据管理 ==========" << endl;
            cout << "1. 保存数据" << endl;
            cout << "2. 重新加载数据" << endl;
            cout << "0. 返回主菜单" << endl;
            cout << "请选择操作：";
            cin >> choice;

            switch (choice) {
                case 1: system.saveData(); break;
                case 2: system.loadData(); break;
                case 0: break;
                default: cout << "无效选择！" << endl;
            }
        } while (choice != 0);
    }

    void addAccountInterface() {
        string code, name;
        int typeChoice;
        
        cout << "请输入账户代码：";
        cin >> code;
        cin.ignore();
        cout << "请输入账户名称：";
        getline(cin, name);
        
        cout << "请选择账户类型：" << endl;
        cout << "1. 资产 2. 负债 3. 权益 4. 收入 5. 支出" << endl;
        cout << "选择：";
        cin >> typeChoice;
        
        AccountType type;
        switch (typeChoice) {
            case 1: type = AccountType::ASSET; break;
            case 2: type = AccountType::LIABILITY; break;
            case 3: type = AccountType::EQUITY; break;
            case 4: type = AccountType::REVENUE; break;
            case 5: type = AccountType::EXPENSE; break;
            default:
                cout << "无效的账户类型！" << endl;
                return;
        }
        
        system.addAccount(code, name, type);
    }

    void removeAccountInterface() {
        string code;
        cout << "请输入要删除的账户代码：";
        cin >> code;
        system.removeAccount(code);
    }

    void viewAccountInterface() {
        string code;
        cout << "请输入账户代码：";
        cin >> code;
        system.displayAccount(code);
    }

    void addTransactionInterface() {
        string description, debitCode, creditCode;
        double amount;
        
        cin.ignore();
        cout << "请输入交易描述：";
        getline(cin, description);
        
        cout << "请输入借方账户代码：";
        cin >> debitCode;
        
        cout << "请输入贷方账户代码：";
        cin >> creditCode;
        
        cout << "请输入交易金额：";
        cin >> amount;
        
        system.addTransaction(description, debitCode, creditCode, amount);
    }

    void run() {
        int choice;
        cout << "欢迎使用会计管理系统！" << endl;
        
        do {
            displayMainMenu();
            cin >> choice;

            switch (choice) {
                case 1: accountMenu(); break;
                case 2: transactionMenu(); break;
                case 3: reportMenu(); break;
                case 4: dataMenu(); break;
                case 0: 
                    cout << "感谢使用会计系统，再见！" << endl;
                    break;
                default: 
                    cout << "无效选择，请重新输入！" << endl;
            }
        } while (choice != 0);
    }
};

int main() {
    MenuSystem menu;
    menu.run();
    return 0;
}