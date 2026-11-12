#include "ulist.h"

UList::Node::Node(string val, Node* t) : data(val), value(""), next(t) {}

UList::Node::Node(string key, string val, Node* t) : data(key), value(val), next(t) {}

UList::UList(Node* n) : head(n), sz(n ? 1 : 0) {}

UList::Node* UList::ul_find(string key) {
    Node* cur = head;
    while (cur != nullptr && cur->data != key) {
        cur = cur->next;
    }
    return cur;
}

void UList::ul_insert_head(string val) {
    Node* n = new Node{val, head};
    head = n;
    sz++;
}

void UList::ul_insert_head(string key, string value) {
    Node* n = new Node{key, value, head};
    head = n;
    sz++;
}

void UList::ul_insert_tail(string val) {
    Node* n = new Node{val, nullptr};
    if (head == nullptr) {
        head = n;
        sz++;
        return;
    }
    Node* cur = head;
    while (cur->next != nullptr) {
        cur = cur->next;
    }
    cur->next = n;
    sz++;
}

void UList::ul_insert_before(string key, string val) {
    if (head == nullptr) {
        cout << "Ошибка: Не найдено\n";
        return;
    }
    if (head->data == key) {
        ul_insert_head(val);
        return;
    }
    Node* cur = head;
    while (cur->next != nullptr && cur->next->data != key) {
        cur = cur->next;
    }
    if (cur->next == nullptr) {
        cout << "Ошибка: Не найдено\n";
        return;
    }
    Node* n = new Node{val, cur->next};
    cur->next = n;
    sz++;
}

void UList::ul_insert_after(string key, string val) {
    Node* cur = ul_find(key);
    if (cur == nullptr) {
        cout << "Ошибка: Не найдено\n";
        return;
    }
    Node* n = new Node{val, cur->next};
    cur->next = n;
    sz++;
}

void UList::ul_delete_head() {
    if (head == nullptr) {
        cout << "Ошибка: Список пуст\n";
        return;
    }
    if (head->next == nullptr) {
        cout << "Последний узел списка удален.\n";
    }
    Node* v = head;
    head = v->next;
    delete v;
    sz--;
}

void UList::ul_delete_tail() {
    if (head == nullptr) {
        cout << "Ошибка: Список пуст\n";
        return;
    }
    if (head->next == nullptr) {
        delete head;
        head = nullptr;
        sz--;
        cout << "Последний узел списка удален.\n";
        return;
    }
    Node* cur = head;
    while (cur->next->next != nullptr) {
        cur = cur->next;
    }
    delete cur->next;
    cur->next = nullptr;
    sz--;
}

void UList::ul_delete_before(string key) {
    if (head == nullptr || head->next == nullptr) {
        cout << "Ошибка: Не найдено\n";
        return;
    }
    if (head->data == key) {
        cout << "Ошибка: Перед элементом ничего нет\n";
        return;
    }
    if (head->next->data == key) {
        Node* v = head;
        head = v->next;
        delete v;
        sz--;
        return;
    }
    Node* cur = head;
    while (cur->next->next != nullptr && cur->next->next->data != key) {
        cur = cur->next;
    }
    if (cur->next->next == nullptr) {
        cout << "Ошибка: Не найдено\n";
        return;
    }
    Node* v = cur->next;
    cur->next = v->next;
    delete v;
    sz--;
}

void UList::ul_delete_after(string key) {
    Node* cur = ul_find(key);
    if (cur == nullptr) {
        cout << "Ошибка: Не найдено\n";
        return;
    }
    if (cur->next == nullptr) {
        cout << "Ошибка: После элемента ничего нет\n";
        return;
    }
    Node* v = cur->next;
    cur->next = v->next;
    delete v;
    sz--;
}

void UList::ul_delete_key(string key) {
    if (head == nullptr) {
        cout << "Ошибка: Не найдено\n";
        return;
    }
    if (head->data == key) {
        if (head->next == nullptr) {
            cout << "Последний узел списка удален.\n";
        }
        Node* v = head;
        head = v->next;
        delete v;
        sz--;
        return;
    }
    Node* cur = head;
    while (cur->next != nullptr && cur->next->data != key) {
        cur = cur->next;
    }
    if (cur->next == nullptr) {
        cout << "Ошибка: Не найдено\n";
        return;
    }
    Node* v = cur->next;
    cur->next = v->next;
    delete v;
    sz--;
}

string UList::ul_search(string key) {
    Node* cur = ul_find(key);
    if (cur == nullptr) {
        cout << "Ошибка: Не найдено\n";
        return "";
    }
    cout << cur->data << endl;
    return cur->data;
}

void UList::ul_print_iter() {
    if (head == nullptr) {
        cout << "Список пуст\n";
        return;
    }
    Node* cur = head;
    while (cur != nullptr) {
        cout << cur->data << " -> ";
        cur = cur->next;
    }
    cout << "nullptr\n";
}

void UList::printrecurs(Node* c) {
    if (c == nullptr) {
        cout << "nullptr\n";
        return;
    }
    cout << c->data << " -> ";
    printrecurs(c->next);
}

void UList::ul_print_rec() {
    if (head == nullptr) {
        cout << "Список пуст\n";
        return;
    }
    printrecurs(head);
}

bool UList::ul_get_value(string key, string& value) const {
    Node* cur = head;
    while (cur != nullptr) {
        if (cur->data == key) {
            value = cur->value;
            return true;
        }
        cur = cur->next;
    }
    return false;
}

bool UList::ul_update_value(string key, string value) {
    Node* cur = head;
    while (cur != nullptr) {
        if (cur->data == key) {
            cur->value = value;
            return true;
        }
        cur = cur->next;
    }
    return false;
}

bool UList::ul_remove_key(string key) {
    if (head == nullptr) return false;
    if (head->data == key) {
        Node* v = head;
        head = v->next;
        delete v;
        sz--;
        return true;
    }
    Node* cur = head;
    while (cur->next != nullptr && cur->next->data != key) {
        cur = cur->next;
    }
    if (cur->next == nullptr) return false;
    Node* v = cur->next;
    cur->next = v->next;
    delete v;
    sz--;
    return true;
}

vector<pair<string, string>> UList::ul_get_pairs() const {
    vector<pair<string, string>> result;
    Node* cur = head;
    while (cur != nullptr) {
        result.push_back({cur->data, cur->value});
        cur = cur->next;
    }
    return result;
}

void UList::ul_print_pairs() const {
    Node* cur = head;
    while (cur != nullptr) {
        cout << cur->data << "=" << cur->value << " -> ";
        cur = cur->next;
    }
    cout << "nullptr\n";
}

int UList::ul_getsz() const {
    return sz;
}

UList::~UList() {
    while (head != nullptr) {
        Node* v = head;
        head = head->next;
        delete v;
    }
}
