#include <iostream>
#include <cstdlib> 
#include <ctime>
#include <string>
#include <fstream>
#include <vector>
using namespace std; 

const int FILE_MORSE = 0;
const int FILE_TEXT = 1;

const string arrayError [] = {
    "ERROR 01. Unknown command. Type \"morse -h\" for help",
    "ERROR 02. Unrecognized command. Type \"morse -h\" for help",
    "ERROR 03. File not found.",
    "ERROR 04: Unrecognized character ",
    "ERROR 05: Invalid Morse code "
};

enum messError {UnknowCommand, ErrorCommand, FileNotFound, ErrorCharacter, ErrorMorseCode};

const string strHelp = "\n\
    Example command line: ./morse <input file> <output file> [arguments]\n\
    Options:\n\
    \t o -h : print out the help to instruct user of the usage on the screen and save the instruction to a file name readme.txt, override if readme.txt has already existed.\n\
    \t o -t : force to take the input file as a text file and the output file as the Morse code one.\n\
    \t o -m : force to take the input file as Morse code and the output file as the interpreted textfile.\n\
    \t o -c : print the statistics of the successful conversion to the screen";

void showHelp(){
    cout << strHelp << endl;
    fstream file;
    file.open("readme.txt", ios::out);
    file << strHelp;
    file.close();
}

void showErrorCode(string mes){
    cout << mes << endl;
    if(mes.find(arrayError[FileNotFound]) < 0){
        cout<<"cl";
        showHelp();
    }
}

const string arrayCommand [] = {"-h", "-t", "-m", "-c"};

const string arrayMorseCode [60][2] = {
    {"a", ".-"}, {"b", "-..."}, {"c", "-.-."}, {"d", "-.."}, {"e", "."},{"é", "..-.."},{"É", "..-.."}, {"f", "..-."}, {"g", "--."}, {"h", "...."}, {"i", ".."}, {"j", ".---"},\
    {"k", "-.-"}, {"l", ".-.."}, {"m", "--"}, {"n", "-."}, {"o", "---"}, {"p", ".--."}, {"q", "--.-"}, {"r", ".-."}, {"s", "..."}, {"t", "-"},\
    {"u", "..-"}, {"v", "...-"}, {"w", ".--"}, {"x", "-..-"}, {"y", "-.--"}, {"z", "--.."}, {"0", "-----"}, {"1", ".----"}, {"2", "..---"}, {"3", "...--"},\
    {"4", "....-"}, {"5", "....."}, {"6", "-...."}, {"7", "--..."}, {"8", "---.."}, {"9", "----."}, {"!", "-.-.--"}, {"@", ".--.-."},\
    {"$", "...-..-"}, {"&", ".-..."}, {"(", "-.--."}, {"×", "-..-"}, {")", "-.--.-"}, {"-", "-....-"}, {"_", "..--.-"},\
    {"+", ".-.-."}, {"=", "-...-"}, {":", "---..."}, {";", "-.-.-"}, {"\'", ".----."}, {"\"", ".-..-."}, {".", ".-.-.-"}, {",", "--..--"},\
    {"?", "..--.."}, {"/", "-..-."}, {"understood", "...-."}, {"#", "........"}, {"end of work", "...-.-"}
};

bool isInArrayCommand(string cmd){
    for(int i = 0;i<4;i++){
        if(cmd == arrayCommand[i]){
            return true;
        }
    }
    return false;
}

const char separatorCharacterMorse = ' ';
const char separatorWordMorse = '\\';
const char separatorWordText = ' ';
const char errorCharacter = '#';

struct InfoTask{
    string inputFile = "";
    string outputFile = "";
    string fileLog = "";
    string timeFinsh = "";
    long duration = 0;
    long numWordInput = 0;
    long numWordConvert = 0;
    long numWordError = 0;
    long totalCharacterInput = 0;
    long totalCharacterConvert = 0;
    long totalCharacterNotConvert = 0;
    vector<string> listError;
};

typedef struct InfoTask InfoTask;

struct Config{
    string inputFile;
    string outputFile;
    bool isConvertTextToMorse = false;
    bool isConvertMorseToText = false;
    bool isPrintOntoScreen = false;
    InfoTask infoTask;
    bool accept = false;
};

typedef struct Config Config;

Config getConfigFromCommandLine(int args, char * argv[]){
    Config config;
    if(args < 2){
        showErrorCode(arrayError[ErrorCommand]);
    }else if(args == 2){
        string arg = argv[1];
        if(arg == "-h"){
            showHelp();
        }else{
            showErrorCode(arrayError[UnknowCommand]);
        }
    } else if(args > 4){
        showErrorCode(arrayError[UnknowCommand]);
    }else{
        bool checkForceOnlyOne = false;
        config.inputFile = argv[1];
        config.accept = true;
        if(args >= 3){
            config.outputFile = argv[2];
            for(int i = 3;i<args;i++){
                string arg = argv[i];
                if(arg == "-t" || arg == "-m" || arg == "-tc" || arg == "-ct" || arg == "-mc" || arg == "-cm"){
                    if(checkForceOnlyOne == false){
                        checkForceOnlyOne = true;
                        if(arg == "-t" || arg == "-tc" || arg == "-ct"){
                            config.isConvertTextToMorse = true;
                        }else{
                            config.isConvertMorseToText = true;
                        }
                    }else{
                        config.accept = false;
                        showErrorCode(arrayError[UnknowCommand]);
                        break;
                    }
                    string arg = argv[i];
                    if(arg.find('c')>=0){
                        config.isPrintOntoScreen = true;
                    }
                }else if(arg == "-c"){
                    config.isPrintOntoScreen = true;
                }else{
                    config.accept = false;
                    showErrorCode(arrayError[UnknowCommand]);
                }
            }
        }else{
            config.accept = false;
            showErrorCode(arrayError[UnknowCommand]);
        }
    }
    return config;
}

int getTypeFile(string filename){
    fstream file;
    file.open(filename, ios::in);
    int typeFile = FILE_MORSE;
    if(file.good()){
        int count = 0;
        char c = file.get();
        while(!file.eof()){
            if(c != '.' && c != '-' && c != '/' && c != ' ' && c != '\n'){
                typeFile = FILE_TEXT;
                break;
            }
            c = file.get();
        }
    }
    file.close();
    return typeFile;
}

bool checkValidFile(Config *config){
    string inputFile = config->inputFile;
    string outputFile = config->outputFile;
    fstream file;
    file.open(inputFile, ios::in);
    if(!file.good()){
        string mes = arrayError[FileNotFound] + " " + "File input: \"" + inputFile + "\" not found.";
        showErrorCode(mes);
        file.close();
        return false;
    }
    file.close();
    file.open(outputFile, ios::in);
    if(file.good()){
        cout << "File output: \"" << outputFile << "\" is existed. Do you want override? [Y/N]: ";
        char a;
        cin >> a;
        if(a != 'y' && a != 'Y'){
            file.close();
            return false;
        }
    }
    file.close();
    int typeFileInput = getTypeFile(inputFile);
    if(config->isConvertTextToMorse == true && typeFileInput == FILE_MORSE){
        cout << "File input is file Morse code. Require file input is file text." << endl;
        return false;
    }else if (config->isConvertMorseToText == true && typeFileInput == FILE_TEXT){
        cout << "File input is file text. Require file input is file Morse code." << endl;
        return false;
    }else if(config->isConvertMorseToText == false && config->isConvertTextToMorse == false){
        if(typeFileInput == FILE_TEXT){
            config->isConvertTextToMorse = true;
        }else{
            config->isConvertMorseToText = true;
        }
    }
    return true;
}

string getStringMonth(int month){
    switch (month)
    {
    case 1:
        return "JAN";
    case 2:
        return "FEB";
    case 3:
        return "MAR";
    case 4:
        return "APR";
    case 5:
        return "MAY";
    case 6:
        return "JUN";
    case 7:
        return "JUL";
    case 8:
        return "AUG";
    case 9:
        return "SEP";
    case 10:
        return "OCT";
    case 11:
        return "NOV";
    case 12:
        return "DEC";
    
    default:
        return "NONE";
    }
}

string getStringOfNumber(int num){
    if(num < 10){
        return "0"+to_string(num);
    }else{
        return "" + to_string(num);
    }
}

string findMorseCode(char character){
    if(character >= 65 && character <= 90){
        character += 32;
    }
    string fc = "";
    fc.push_back(character);
    for(int i = 0;i<60;i++){
        if(arrayMorseCode[i][0] == fc){
            return arrayMorseCode[i][1];
        }
    }
    return "unknow";
}

/*
    A-Z: 65 - 90
    a-z: 97 - 122
    0-9: 48 - 57
*/
bool isWord(string word){
    for(int i = 0;i < word.size();i++){
        int c = word[i];
        if((c >= 48 && c <= 57) || (c >= 65 && c <= 90) || (c >= 97 && c <= 122)){
            return true;
        }
    }
    return false;
}

string getFileName(string fileName){
    int indexStart = fileName.size();
    int indexEnd = fileName.size();
    for(int i = fileName.size() - 1;i>=0;i--){
        if(indexStart == fileName.size()){
            if(fileName[i] == '/' || fileName[i] == '\\'){
                indexStart = i;
            }
        }else{
            break;
        }
        if(indexEnd == fileName.size()){
            if(fileName[i] == '.'){
                indexEnd = i;
            }
        }
    }
    if(indexStart == fileName.size()){
        indexStart = -1;
    }
    return fileName.substr(indexStart+1, indexEnd);
}

void convertTextToMorse(Config *config){
    time_t start = time(0);
    long line = 1;
    string inputFileName = config->inputFile;
    string outputFileName = config->outputFile;
    fstream reader, writer;
    reader.open(inputFileName, ios::in);
    writer.open(outputFileName, ios::out);
    char charactor;
    string code;
    charactor = reader.get();
    string word = "";
    bool errorWord = false;
    while(!reader.eof()){
        config->infoTask.totalCharacterInput ++;
        if(charactor == '\t' || charactor == ' ' || charactor == '\n'){
            if(word.size() > 0 && isWord(word) == true){
                config->infoTask.numWordInput ++;
                if(errorWord == true){
                    config->infoTask.numWordError ++;
                }else{
                    config->infoTask.numWordConvert ++;
                }
            }
            word = "";
            errorWord = false;
            if(charactor == '\n'){
                line ++;
                writer << charactor;
                charactor = reader.get();
                continue;
            }
            if(charactor == '\t'){
                writer << "    ";
                charactor = reader.get();
                continue;
            }
            if(charactor == ' '){
                writer << "/ ";
                charactor = reader.get();
                continue;
            }
        }
        word.push_back(charactor);
        code = findMorseCode(charactor);
        if(code == "unknow"){
            if(errorWord == false){
                errorWord = true;
            }
            config->infoTask.totalCharacterNotConvert ++;
            code == "........ ";
            string errorConvert = arrayError[ErrorCharacter];
            errorConvert.push_back(charactor);
            errorConvert += " on line " + to_string(line);
            config->infoTask.listError.push_back(errorConvert);
        }else{
            config->infoTask.totalCharacterConvert ++;
        }
        code += " ";
        writer << code;
        charactor = reader.get();

    }
    if(word.size() > 0 && isWord(word) == true){
        config->infoTask.numWordInput ++;
        if(errorWord == true){
            config->infoTask.numWordError ++;
        }else{
            config->infoTask.numWordConvert ++;
        }
    }
    reader.close();
    writer.close();
    time_t end = time(0);
    config->infoTask.duration = (end - start);
    string fdatetime = "";
    tm *infoDate = localtime(&end);
    fdatetime = to_string(infoDate->tm_year + 1900) + "-" + getStringMonth(infoDate->tm_mon + 1) + "-" + getStringOfNumber(infoDate->tm_mday) + " ";
    fdatetime += getStringOfNumber(infoDate->tm_hour) + ":" + getStringOfNumber(infoDate->tm_min) + ":" + getStringOfNumber(infoDate->tm_sec);
    config->infoTask.timeFinsh = fdatetime;
    config->infoTask.inputFile = inputFileName;
    config->infoTask.outputFile = outputFileName;
    config->infoTask.fileLog = getFileName(inputFileName)+"_"+getFileName(outputFileName)+"_"+to_string(infoDate->tm_year + 1900)+getStringOfNumber(infoDate->tm_mon + 1)+getStringOfNumber(infoDate->tm_mday)\
        + "_"+getStringOfNumber(infoDate->tm_hour)+getStringOfNumber(infoDate->tm_min)+getStringOfNumber(infoDate->tm_sec)+".log";
}

string findCharacter(string morseCode){
    for(int i = 0;i < 60; i++){
        if(arrayMorseCode[i][1] == morseCode){
            return arrayMorseCode[i][0];
        }
    }
    return "unknow";
}

void convertMorseToText(Config *config){
    time_t start = time(0);
    string inputFileName = config->inputFile;
    string outputFileName = config->outputFile;
    fstream reader, writer;
    reader.open(inputFileName, ios::in);
    writer.open(outputFileName, ios::out);
    char charactor, cbefore;
    string code = "", result;
    charactor = reader.get();
    string word = "";
    bool errorWord = false;
    long line = 1;
    while(!reader.eof()){
        if(charactor == ' ' || charactor == '/' || charactor == '\n'){
            if(code.size() > 0){
                config->infoTask.totalCharacterInput ++;
                result = findCharacter(code);
                if(result == "unknow"){
                    config->infoTask.totalCharacterNotConvert ++;
                    if(code.size() > 7){
                        writer << "#";
                        word += "#";
                    }else{
                        writer << "*";
                        word += "*";
                    }
                    string log = arrayError[ErrorMorseCode] + code + " on line " + to_string(line);
                    config->infoTask.listError.push_back(log);
                    errorWord = true;
                }else{
                    config->infoTask.totalCharacterConvert ++;
                    writer << result;
                    word += result;
                }
            }
            if((charactor == '/' || charactor == '\n') && word.size() > 0){
                cout << word << endl;
                config->infoTask.numWordInput ++;
                if(errorWord == true){
                    config->infoTask.numWordError ++;
                }else{
                    config->infoTask.numWordConvert ++;
                }
                word = "";
                errorWord = false;
            }
            if(charactor == '\n'){
                line ++;
                writer << charactor;
            }
            if(charactor == '/'){
                writer << " ";
            }
            code = "";
            charactor = reader.get();
            continue;
        }
        code.push_back(charactor);
        charactor = reader.get();
    }
    if(code.size() > 0){
        config->infoTask.totalCharacterInput ++;
        result = findCharacter(code);
        if(result == "unknow"){
            config->infoTask.totalCharacterNotConvert ++;
            if(code.size() > 7){
                writer << "#";
                word += "#";
            }else{
                writer << "*";
                word += "*";
            }
            string log = arrayError[ErrorMorseCode] + code + " on line " + to_string(line);
            config->infoTask.listError.push_back(log);
            errorWord = true;
        }else{
            config->infoTask.totalCharacterConvert ++;
            writer << result;
            word += result;
        }
    }
    if(word.size() > 0){
        config->infoTask.numWordInput ++;
        if(errorWord == true){
            config->infoTask.numWordError ++;
        }else{
            config->infoTask.numWordConvert ++;
        }
        word = "";
        errorWord = false;
    }
    reader.close();
    writer.close();
    time_t end = time(0);
    config->infoTask.duration = (end - start);
    string fdatetime = "";
    tm *infoDate = localtime(&end);
    fdatetime = to_string(infoDate->tm_year + 1900) + "-" + getStringMonth(infoDate->tm_mon + 1) + "-" + getStringOfNumber(infoDate->tm_mday) + " ";
    fdatetime += getStringOfNumber(infoDate->tm_hour) + ":" + getStringOfNumber(infoDate->tm_min) + ":" + getStringOfNumber(infoDate->tm_sec);
    config->infoTask.timeFinsh = fdatetime;
    config->infoTask.inputFile = inputFileName;
    config->infoTask.outputFile = outputFileName;
    config->infoTask.fileLog = getFileName(inputFileName)+"_"+getFileName(outputFileName)+"_"+to_string(infoDate->tm_year + 1900)+getStringOfNumber(infoDate->tm_mon + 1)+getStringOfNumber(infoDate->tm_mday)\
        + "_"+getStringOfNumber(infoDate->tm_hour)+getStringOfNumber(infoDate->tm_min)+getStringOfNumber(infoDate->tm_sec)+".log";
}

void printResultConvertToFile(InfoTask info){
    string fileName = info.fileLog;
    fstream writer;
    writer.open(fileName, ios::out);
    writer << "Input file: " << info.inputFile << "\n";
    writer << "Output file: " << info.outputFile << "\n";
    writer << "Time complete: " << info.timeFinsh << "\n";
    writer << "Duration [seconds]: " << info.duration << "\n";
    writer << "Word count in input file: " << info.numWordInput << "\n";
    writer << "Word converted: " << info.numWordConvert << "\n";
    writer << "Word with errors: " << info.numWordError << "\n";
    writer << "Total number of characters: " << info.totalCharacterInput << "\n";
    writer << "Number of characters have been converted: " << info.totalCharacterConvert << "\n";
    writer << "Number of characters are NOT converted: " << info.totalCharacterNotConvert << "\n";
    for(int i = 0;i < info.listError.size();i++){
        writer << "\t" << info.listError[i] << "\n";
    }
    writer.close();
}

void printResultConvertToScreen(InfoTask info){
    cout << "Input file: " << info.inputFile << endl;
    cout << "Output file: " << info.outputFile << endl;
    cout << "Time complete: " << info.timeFinsh << endl;
    cout << "Duration [seconds]: " << info.duration << endl;
    cout << "Word count in input file: " << info.numWordInput << endl;
    cout << "Word converted: " << info.numWordConvert << endl;
    cout << "Word with errors: " << info.numWordError << endl;
    cout << "Total number of characters: " << info.totalCharacterInput << endl;
    cout << "Number of characters have been converted: " << info.totalCharacterConvert << endl;
    cout << "Number of characters are NOT converted: " << info.totalCharacterNotConvert << endl;
    for(int i = 0;i < info.listError.size();i++){
        cout << "\t" << info.listError[i] << endl;
    }
}

int main(int args, char * argv[]){
    Config config = getConfigFromCommandLine(args, argv);
    if(config.accept == true){
        if(checkValidFile(&config) == true){
            if(config.isConvertTextToMorse == true){
                convertTextToMorse(&config);
            }else{
                convertMorseToText(&config);
            }
            printResultConvertToFile(config.infoTask);
            if(config.isPrintOntoScreen == true){
                printResultConvertToScreen(config.infoTask);
            }
        }
    }
    return 0;
}
