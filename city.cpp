#include <iostream>
#include <unordered_map>
#include <string>

class City{
    private:
    std::unordered_map<int, std::string> Map;
    public:
    // adds new citizen, requirements: id(int) comment(std::stringi)
    void Add(int &id, std::string &extra){

        if(Map.find(id) != Map.end()){
            std::cout << "This id already exists\n";
        }else{
            Map[id] = extra;
            std::cout << "ID: " << id << " COMMENT: " << extra << " has been added\n";
        }
    }
    // deletes citizen, requirements: id(int)
    void Delete(int &id){

        if(Map.find(id) != Map.end()){
            std::cout << "ID: " << id << " COMMENT: " << Map.find(id)->second << " has been deleted\n";
            Map.erase(id);
        }else{
            std::cout << "ID: " << id << " does not exists\n";
        }
    }

    //prints all items in the Map
    void Print_all(){
        int total = 0;
        for (auto [key, value] : Map)
        {
            std::cout << "ID: " << key << " COMMENT: " << value << std::endl;
            total ++;
        }   
        std::cout << "Total number of citizens: " <<  total << std::endl;
    }

    //Looking for specific citizen, requirements: id(int)
    void Look(int id){
        if(Map.find(id) != Map.end()){
            std::cout << "ID: " << id << " COMMENT: " << Map.find(id)->second << std::endl;
        }else{
            std::cout << "ID: " << id << " does not exists\n";
        }
    }
};

int main(){
    City ct; 
    std::string menu= "Commands:\n  add: adds citizen\n  delete: deletes citizen\n  print all: prints all the citizen\n  Look: finds for specific user\n exit: exit\n";
    std::cout << menu;
    while (1)
    {
        std::string input;

        std::cout << "User@code~#";
        std::getline(std::cin, input);

        if(input == "add"){
            std::string comment;
            int id;

            std::cout << "COMMENT(name, inf, etc)~#";
            std::getline(std::cin, comment);

            std::cout << "ID(INT)~#";
            std::cin >> id;

            std::cin.ignore();
            
            ct.Add(id, comment);
        }else if (input == "delete"){
            int id;

            std::cout << "ID(INT)~#";
            std::cin >> id;

            std::cin.ignore();

            ct.Delete(id);
        }else if(input == "print all"){
            ct.Print_all();
        }else if(input == "Look"){
            int id;

            std::cout << "ID(INT)~#";
            std::cin >> id;

            std::cin.ignore();

            ct.Look(id);
        }else if(input == "exit"){
            return 0;
        }else{
            std::cout << "Invalid command\n";
        }
    }
    
    return 0;
}
