
#include "taskmanager.h"
#include <iostream>
#include <string>
#include <stdexcept>

using namespace std;

enum class Command {
    Create, Get, List, StartNext, Complete,
    Fail, Cancel, Stats, Help, Unknown
};

Command parseCommand(const string& command) {
    if (command == "create") return Command::Create;
    if (command == "get") return Command::Get;
    if (command == "list") return Command::List;
    if (command == "start-next") return Command::StartNext;
    if (command == "complete") return Command::Complete;
    if (command == "fail") return Command::Fail;
    if (command == "cancel") return Command::Cancel;
    if (command == "stats") return Command::Stats;
    if (command == "help") return Command::Help;
    return Command::Unknown;
}

int main(int argc, char* argv[]) {
    if (argc < 4 || string(argv[1]) != "--data") {
        cout << "Использование: taskctl --data <path> <command> <args>" << endl;
        return 1;
    }

    string path = argv[2];
    Command command = parseCommand(argv[3]);

    if (command == Command::Unknown) {
        cout << "Неизвестная команда" << endl;
        return 1;
    }

    try {
        TaskManager manager;
        manager.loadFromFile(path);

        switch (command) {
            case Command::Help:
                cout << "Доступные команды:\n   create, get, list, start-next, complete, fail, cancel, stats" << endl;
                break;

            case Command::Create: {
                string type, priorityText, payload;
                bool typ = false, prior = false, load = false;

                for (int i = 4; i < argc; i += 2) {
                    if (i + 1 >= argc) {
                        throw invalid_argument("Не указано значение параметра");
                    }

                    string flag = argv[i];
                    string value = argv[i + 1];

                    if (flag == "--type") {
                        type = value;
                        typ = true;
                    }
                    else if (flag == "--priority") {
                        priorityText = value;
                        prior = true;
                    }
                    else if (flag == "--payload") {
                        payload = value;
                        load = true;
                    }
                    else {
                        throw invalid_argument("Неизвестный параметр: " + flag);
                    }
                }

                if (!typ || !prior || !load) {
                    throw invalid_argument("Не указаны все параметры");
                }

                size_t pos = 0;
                int priority = stoi(priorityText, &pos);

                if (pos != priorityText.size() || priority < 0 || priority > 100) {
                    throw invalid_argument("Неверное значение приоритета (от 0 до 100)");
                }

                uint64_t id = manager.createTask(type, payload, static_cast<unsigned int>(priority));
                manager.saveToFile(path);
                cout << "Задача создана. ID: " << id << endl;
                break;
            }

            case Command::Get: {
                if (argc != 5) {
                    throw invalid_argument("Использование: taskctl --data <path> get <id>");
                }

                string idText = argv[4];
                size_t pos = 0;

                if (idText.empty() || idText[0] == '-') {
                    throw invalid_argument("Некорректный ID");
                }

                uint64_t id = stoull(idText, &pos);

                if (pos != idText.size() || id == 0) {
                    throw invalid_argument("Некорректный ID");
                }

                Task* task = manager.getTask(id);
                cout << "ID: " << task->id << "\nType: " << task->type << "\nPayload: " << task->payload
                     << "\nPriority: " << task->priority << "\nStatus: " << task->status
                     << "\nCreated At: " << task->created_at << "\nUpdated At: " << task->updated_at
                     << "\nError: " << task->error << endl;
                break;
            }

            case Command::List: {
                string status = "";
                if (argc != 4 && argc != 6) {
                    throw invalid_argument("Команда: taskctl --data <path> list [--status <status]");
                }

                if (argc == 6) {
                    if (string(argv[4]) != "--status") {
                        throw invalid_argument("Неизвестный параметр");
                    }
                    status = argv[5];
                }
                Arr result;
                manager.listTasks(result, status);

                if (result.arr_getsz() == 0) {
                    cout << "Задачи не найдены" << endl;
                }
                
                for (int i = 0; i < result.arr_getsz(); i++) {
                    Task* task = result.arr_search(i);
                    cout << "[ID: " << task->id << "] " << "[Type: " << task->type << "] "
                    << "[Priority: " << task->priority << "] " << "[Status: " << task->status << "]" << endl;
                }
                break;
            }

            case Command::StartNext: {
                if (argc != 4) {
                    throw invalid_argument("Команда: taskctl --data <path> start-next");
                }

                Task* task = manager.startNextTask();
                manager.saveToFile(path);
                cout << "Задача запущена: [ID: " << task->id << "] " << "[Type: " << task->type << "] "
                    << "[Priority: " << task->priority << "] " << "[Status: " << task->status << "]" << endl;
                break;
            }

            case Command::Complete: {
                if (argc != 5) {
                    throw invalid_argument("Команда: taskctl --data <path> complete <id>");
                }
                string idText = argv[4];
                size_t pos = 0;

                if (idText.empty() || idText[0] == '-') {
                    throw invalid_argument("Некорректный ID");
                }

                uint64_t id = stoull(idText, &pos);

                if (pos != idText.size() || id == 0) {
                    throw invalid_argument("Некорректный ID");
                }

                manager.completeTask(id);
                manager.saveToFile(path);
                cout << "Задача " << id << " успешно завершена!" << endl;
                break;
            }

            case Command::Fail: {
                if (argc != 7) {
                    throw invalid_argument("Команда: taskctl --data <path> fail <id> --error <text>");
                }
                if (string(argv[5]) != "--error") {
                    throw invalid_argument("Ожидался параметр --error");
                }

                string idText = argv[4];
                string error = argv[6];
                size_t pos = 0;

                if (idText.empty() || idText[0] == '-') {
                    throw invalid_argument("Некорректный ID");
                }

                uint64_t id = stoull(idText, &pos);

                if (pos != idText.size() || id == 0) {
                    throw invalid_argument("Некорректный ID");
                }

                manager.failTask(id, error);
                manager.saveToFile(path);
                cout << "Задача " << id << " завершена с ошибкой: " << error << endl;
                break;
            }

            case Command::Cancel: {
                if (argc != 5) {
                    throw invalid_argument("Команда: taskctl --data <path> cancel <id>");
                }
                string idText = argv[4];
                size_t pos = 0;

                if (idText.empty() || idText[0] == '-') {
                    throw invalid_argument("Некорректный ID");
                }

                uint64_t id = stoull(idText, &pos);

                if (pos != idText.size() || id == 0) {
                    throw invalid_argument("Некорректный ID");
                }

                manager.cancelTask(id);
                manager.saveToFile(path);
                cout << "Задача " << id << " отменена" << endl;
                break;
            }

            case Command::Stats: {
                TaskStats statistics = manager.getStats();
                cout << "[Статистика]\n    Total: " << statistics.total << "\n    Running: " << statistics.running
                     << "\n    Queued: " << statistics.queued << "\n    Queue size: " << statistics.queue_size
                     << "\n    Completed: " << statistics.completed << "\n    Cancelled: " << statistics.cancelled
                     << "\n    Failed: " << statistics.failed << "\n    Max priority: " << statistics.max_priority << endl;
                break;
            }
            
            default:
                break;
        }
    }
    catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
        return 1;
    }

    return 0;
}