/*****************************
 * Project of:
 * Ori Kopel 205533151
 * Shlomo Rabinovich 308432517
 * December 18
 ******************************/

#include "ReadData.h"
#include "Command.h"
#include <iterator>
#include <map>
#include <vector>

#include "OpenServerCommand.h"

/**
 * Create map command
 */
ReadData::ReadData() {
    this->mapSymb = new map<string, string>;
    commandMap.insert(pair<string, Command *>("openDataServer", new OpenServerCommand(this->mapSymb)));
    commandMap.insert(pair<string, Command *>("connect", new ConnectCommand(this->mapSymb)));
    commandMap.insert(pair<string, Command *>("var", new DefineVarCommand(this->mapSymb)));
    commandMap.insert(pair<string, Command *>("while", new LoopCommand(this->mapSymb)));
    commandMap.insert(pair<string, Command *>("if", new IfCommand(this->mapSymb)));
    commandMap.insert(pair<string, Command *>("print", new PrintCommand(this->mapSymb)));
    commandMap.insert(pair<string, Command *>("sleep", new SleepCommand(this->mapSymb)));
}


/**
 * make array of words
 * @param file of instructions
 */
void ReadData::lexer(string file) {
    string s = "1";
    ifstream ifs;
    ifs.open(file);
    if (!ifs.is_open()) { return; }

    // read lines from data file
    while (!s.empty()) {
        s = "";
        getline(ifs, s);
        //parse line to words
        std::istringstream iss(s);
        std::vector<std::string> results(std::istream_iterator<std::string>{iss},
                                         std::istream_iterator<std::string>());
        //each line in a node
        this->vec.push_back(results);
    }

//    this->vec = results;
    ifs.close();
}

/**
 *
 */
void ReadData::parser() {
    ConditionParser *dad = nullptr;
    vector<Command *> commandList;
    for (vector<string> tmp:this->vec) {
        if (tmp.size() <= 1) {
            if (tmp.size() == 1 && tmp[0] == "}") {
                dad = dad->getDad();
            }
            continue;
        }
        Command *c;
        if (tmp[0] == "var") {
            c = new DefineVarCommand(this->mapSymb);
        } else {
            c = this->commandMap[tmp[0]];
        }
        if (c == nullptr) {
            c = new AssingmentCommand(this->mapSymb);
        } else {
            tmp.erase(tmp.begin());//delete the funcName
        }
        c->setParam(tmp);//send parameters
        if (dad != nullptr) {
            dad->addCommand(c);
        } else {
            commandList.push_back(c);
        }
        if (c->isDad) {
            c->setDad(dad);
            dad = (ConditionParser *) c;
        }
        string check = tmp[tmp.size() - 1];
        if (check == "}" || check[check.size() - 1] == '}') {
            dad = dad->getDad();
        }
//        c->execute();
    }

    for (Command *tmp:commandList) {
        tmp->execute();
    }


}//end of parser

/**
 * DTOR
 */
ReadData::~ReadData() {
    for (auto &c : this->commandMap) {
        delete c.second;
    }
}

const std::vector<vector<string>> &

ReadData::getVector() const {
    return this->vec;
}

bool ReadData::isVarInMap(string s) {
    return true;//todo checking before assigment
}
