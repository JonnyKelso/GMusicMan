#include <iostream>
#include <vector>
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

std::vector<std::string> music_files;
std::vector<std::string> missing_files;

std::string data_path = "";
int process_args(int arg_count, char* arg_array[])
{
    if(arg_count == 1)
    {
        std::cout << "Usage: \n";
        std::cout << "\t " << arg_array[0] << " <path>" << std::endl;
        return 1;
    }
    if(arg_count > 1)
    {
        data_path = arg_array[1];
        if (!fs::is_directory(fs::status(data_path)))
        {
            std::cout << "Path [ " << data_path << " ] is not a directory" << std::endl;
            std::cout << "Please supply a path to the music files." << std::endl;
            return 1; 
        }
    }
    return 0;
}
int main(int argc, char* argv[])
{
    int args_result = process_args(argc, argv);
    if(args_result > 0)
    {
        return args_result;
    }
    for(const auto& entry : fs::directory_iterator(data_path))
    {
        std::cout << entry.path() << std::endl;
        if(entry.path().extension() == ".csv")
        {
   //         std::cout << "Data file: " << entry.path() << std::endl;
            missing_files.push_back(entry.path().string());
        }
        else if(entry.path().extension() == ".mp3")
        {
            music_files.push_back(entry.path().string());
        }
    }

    std::cout << "Found " << music_files.size() << " music files and " << missing_files.size() << " metadata files for missing files." << std::endl;
}
