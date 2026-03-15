// Todo-app.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//
// プログラムの実行: Ctrl + F5 または [デバッグ] > [デバッグなしで開始] メニュー
// プログラムのデバッグ: F5 または [デバッグ] > [デバッグの開始] メニュー

// 作業を開始するためのヒント: 
//    1. ソリューション エクスプローラー ウィンドウを使用してファイルを追加/管理します 
//   2. チーム エクスプローラー ウィンドウを使用してソース管理に接続します
//   3. 出力ウィンドウを使用して、ビルド出力とその他のメッセージを表示します
//   4. エラー一覧ウィンドウを使用してエラーを表示します
//   5. [プロジェクト] > [新しい項目の追加] と移動して新しいコード ファイルを作成するか、[プロジェクト] > [既存の項目の追加] と移動して既存のコード ファイルをプロジェクトに追加します
//   6. 後ほどこのプロジェクトを再び開く場合、[ファイル] > [開く] > [プロジェクト] と移動して .sln ファイルを選択します
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>

namespace fs = std::filesystem;

struct Task {
    std::string title;
};

static std::string trim(const std::string& s) {
    const auto first = s.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    const auto last = s.find_last_not_of(" \t\r\n");
    return s.substr(first, last - first + 1);
}

static fs::path get_storage_file_path() {
    // Windows: %LOCALAPPDATA%\TodoApp\tasks.txt
    char* local = nullptr;
    size_t len = 0;

    if (_dupenv_s(&local, &len, "LOCALAPPDATA") != 0 || local == nullptr) {
        return fs::current_path() / "TodoApp" / "tasks.txt";
    }

    fs::path base(local);
    std::free(local);

    return base / "TodoApp" / "tasks.txt";
}

static std::vector<Task> load_tasks(const fs::path& file) {
    std::vector<Task> tasks;
    std::ifstream ifs(file);
    if (!ifs) return tasks;

    std::string line;
    while (std::getline(ifs, line)) {
        line = trim(line);
        if (line.empty()) continue;
        tasks.push_back(Task{ line });
    }
    return tasks;
}

static bool save_tasks(const fs::path& file, const std::vector<Task>& tasks) {
    try {
        fs::create_directories(file.parent_path());
        std::ofstream ofs(file, std::ios::trunc);
        if (!ofs) return false;

        for (const auto& t : tasks) {
            ofs << t.title << "\n";
        }
        return true;
    }
    catch (...) {
        return false;
    }
}

static void print_menu() {
    std::cout << "\n=== TODO APP (minimal) ===\n"
        << "1) Add task\n"
        << "2) List tasks\n"
        << "3) Delete task\n"
        << "0) Exit\n"
        << "Select: ";
}

static void list_tasks(const std::vector<Task>& tasks) {
    if (tasks.empty()) {
        std::cout << "(no tasks)\n";
        return;
    }
    for (size_t i = 0; i < tasks.size(); ++i) {
        std::cout << (i + 1) << ") " << tasks[i].title << "\n";
    }
}

int main() {
    const fs::path file = get_storage_file_path();
    std::vector<Task> tasks = load_tasks(file);

    std::cout << "Storage: " << file.string() << "\n";
    std::cout << "Loaded tasks: " << tasks.size() << "\n";

    while (true) {
        print_menu();

        std::string input;
        if (!std::getline(std::cin, input)) break;
        input = trim(input);

        if (input == "0") break;

        if (input == "1") {
            std::cout << "Task title: ";
            std::string title;
            std::getline(std::cin, title);
            title = trim(title);

            if (title.empty()) {
                std::cout << "Ignored (empty)\n";
                continue;
            }
            tasks.push_back(Task{ title });

            std::cout << (save_tasks(file, tasks) ? "Saved.\n" : "Save failed.\n");
        }
        else if (input == "2") {
            list_tasks(tasks);
        }
        else if (input == "3") {
            list_tasks(tasks);
            if (tasks.empty()) continue;

            std::cout << "Delete index (1.." << tasks.size() << "): ";
            std::string idxs;
            std::getline(std::cin, idxs);
            idxs = trim(idxs);

            try {
                const size_t idx = static_cast<size_t>(std::stoul(idxs));
                if (idx == 0 || idx > tasks.size()) {
                    std::cout << "Invalid index.\n";
                    continue;
                }
                tasks.erase(tasks.begin() + (idx - 1));
                std::cout << (save_tasks(file, tasks) ? "Saved.\n" : "Save failed.\n");
            }
            catch (...) {
                std::cout << "Invalid input.\n";
            }
        }
        else {
            std::cout << "Unknown command.\n";
        }
    }
    return 0;
}