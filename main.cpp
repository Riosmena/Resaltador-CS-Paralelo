// =====================================================
// File: main.cpp
// Authors: Iván Ricardo Paredes Avilez → A01705083
//          José Emiliano Riosmena Castañón → A01704245
// Date: Wednesday, may 24th, 2023
// To compile: g++ main.cpp -lpthread -o app -std=c++17
// =====================================================

#include <iostream>
#include <fstream>
#include <string>
#include <pthread.h>
#include <regex>
#include <ctime>
#include <filesystem>

using namespace std;
namespace fs = filesystem;

string createLexer(const string &inputText) {
    string tokenCode;

    const string Identifiers = "[a-zA-Z_][a-zA-Z0-9_]*";
    const string Comments = "//.*|/\\*.*\\*/";
    const string Literals = "[0-9]+(\\.[0-9]+)?|\".*\"|'.*'";
    const string Keywords = "abstract|as|base|bool|break|byte|case|catch|char|checked|class|const|continue|decimal|default|delegate|double|do|else|enum|event|explicit|extern|false|finally|fixed|float|foreach|for|foreach|goto|if|implicit|int|in|interface|internal|is|lock|long|namespace|new|null|object|operator|out|override|params|private|protected|public|readonly|ref|return|sbyte|sealed|short|sizeof|stackalloc|static|string|struct|switch|this|throw|true|try|typeof|uint|ulong|unchecked|unsafe|ushort|using|virtual|void|volatile|while";
    const string Operators = "\\+|-|\\|/|%|\\^|&|\\||~|!|=|<|>|\\?|:|;|,|\\.|\\+\\+|--|&&|\\|\\||==|!=|<=|>=|\\+=|-=|\\=|/=|%\\=|\\^=|&\\=|\\|=|<<=|>>=|=>|\\?\\?";
    const string Functions = "System|Console|Program|WriteLine|ReadLine";
    const string Separators = "[\\(\\)\\{\\}\\[\\];,.]";
    const string BreakLine = "\n";
    const string Spaces = "\\s+";
    const regex tokens(Identifiers + "|" + Comments + "|" + Literals + "|" + Operators + "|" + Keywords + "|" + Functions + "|" + Separators + "|" + BreakLine + "|" + Spaces);

    auto actual = sregex_iterator(inputText.begin(), inputText.end(), tokens);
    const auto finish = sregex_iterator();

    while (actual != finish) {
        string type;
        const string token = (*actual).str();
        if (token == "\n") {
            tokenCode += "<br>";
        }
        else {

            if (regex_match(token, regex(BreakLine))) {
                tokenCode += "</pre><pre>";
            }

            else if (regex_match(token, regex(Spaces))) {
                tokenCode += token;
            }

            else if (regex_match(token, regex(Operators))) {
                type = "operator";
            }

            else if (regex_match(token, regex(Comments))) {
                type = "comment";
            }

            else if (regex_match(token, regex(Keywords))) {
                type = "keyword";
            }

            else if (regex_match(token, regex(Literals))) {
                type = "literal";
            }

            else if (regex_match(token, regex(Functions))) {
                type = "function";
            }

            else if (regex_match(token, regex(Separators))) {
                type = "separator";
            }

            else if (regex_match(token, regex(Identifiers))) {
                type = "identifier";
            }
            
            else {
                type = "error";
            }

            tokenCode += "<span class=\"" + type + "\">" + token + "</span>";
        }

        actual++;
    }
    return tokenCode;
}

void createHTML(const string &tokenCode, const string &file) {
    string name = fs::path(file).stem().string() + ".html";
    string path = "./outputFiles/" + name;

    if (!fs::exists("./outputFiles")) {
        fs::create_directory("./outputFiles/");
    }

    string html = R"(
        <!DOCTYPE html>
        <html lang="en">
        <head>
            <meta charset="UTF-8">
            <meta http-equiv="X-UA-Compatible" content="IE=edge">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <link rel="stylesheet" type="text/css" href="../estilos.css">
            <title>Document</title>
        </head>
        <body>
        <pre>
    )";

    html += tokenCode;

    html += R"(
        </pre>
        </body>
        </html>
    )";

    ofstream outputFile(path, ios::app);
    outputFile << html;
    outputFile.close();
}

double seqExecution(const string &directory) {
    cout << "\nSequential App" << endl;
    clock_t start = clock();
    for (auto &files : fs::directory_iterator(directory)) {
        if (files.path().extension() == ".cs") {
            string Path = files.path().string();

            ifstream inputFile(Path);
            if (!inputFile.is_open()) {
                cout << "File: \"" << Path << "\" not found." << endl;
                continue;
            }
            
            string inputLines((istreambuf_iterator<char>(inputFile)), istreambuf_iterator<char>());
            string tokenCode = createLexer(inputLines);
            createHTML(tokenCode, Path);
        }
    }

    clock_t finish = clock();
    double timeElapsed = double (finish - start) / CLOCKS_PER_SEC;
    return timeElapsed;
}

struct ThreadArgs {
    string file;
    string directory;
};

void *threadCreateFile(void* args) {
    ThreadArgs *threadArgs = (ThreadArgs *)args;
    string file = threadArgs -> file;
    string directory = threadArgs -> directory;

    ifstream inputFile(directory + "/" + file);
    string inputLines((istreambuf_iterator<char>(inputFile)), istreambuf_iterator<char>());

    string tokenCode = createLexer(inputLines);
    createHTML(tokenCode, file);

    pthread_exit(NULL);

    return NULL;
}

double parExecution(const string &directory) {
    cout << "\nParallel App" << endl;
    clock_t start = clock();

    pthread_t MAX_THREADS[100];
    int index = 0;

    for (auto &files : fs::directory_iterator(directory)) {
        string file = files.path().filename().string();

        ThreadArgs *threadArgs = new ThreadArgs;
        threadArgs -> file = file;
        threadArgs -> directory = directory;

        pthread_create(&MAX_THREADS[index], NULL, threadCreateFile, (void *)threadArgs);
        index++;
    }

    for (int i = 0; i < index; i++) {
        pthread_join(MAX_THREADS[i], NULL);
    }

    clock_t finish = clock();
    double timeElapsed = double (finish - start) / CLOCKS_PER_SEC;
    return timeElapsed;
}

int main(int argc, char* argv[]) {
    double seqTime, parTime;
    string directory = argv[1];

    if (argc != 2) {
        cout << "usage: " << argv[0] << " ./input_Folder" << endl;
        return -1;
    }

    if (!fs::is_directory(directory)) {
        cout << argv[0] << ": Directory: \"" << argv[1] << "\" not found." << endl;
        return -1;
    }

    cout << "App is running..." << endl;

    seqTime = seqExecution(directory);
    cout << "Total time is: " << seqTime << " seconds" << endl;
    parTime = parExecution(directory);
    cout << "Total time is: " << parTime << " seconds" << endl;
    
    cout << "\nSpeedup is: " << seqTime / parTime << endl;

    return 0;
}

