#include <iostream>
#include <vector>
#include <filesystem>
#include <string>
#include <fstream>

namespace fs = std::filesystem;
using std::string;
using std::wstring;
using std::cout;
using std::endl;

//std::vector<string> known_music_paths = { "F:\\iTunes_Music" };
struct Library
{
    std::vector<wstring> music_files;
    std::vector<wstring> missing_files;
    std::vector<wstring> ext_library;
    std::vector<wstring> found_files;
};
string data_path = "";
int process_args(int arg_count, char* arg_array[])
{
    if(arg_count == 1)
    {
        cout << "Usage: \n";
        cout << "\t " << arg_array[0] << " <path>" << endl;
        return 1;
    }
    if(arg_count > 1)
    {
        data_path = arg_array[1];
        if (!fs::is_directory(fs::status(data_path)))
        {
            cout << "Path [ " << data_path << " ] is not a directory" << endl;
            cout << "Please supply a path to the music files." << endl;
            return 1; 
        }
    }
    return 0;
}
void buildDictionaryOfExistingMusic(std::vector<wstring>& library)
{
    std::wifstream ext_lib_file("sumo_mp3.txt");
    if(ext_lib_file.is_open())
    {

        cout << "Opened library file" << endl;
        wstring line;
        while(ext_lib_file.good())
        {
            std::getline(ext_lib_file, line);
            library.push_back(line);
        }
        cout << "Read " << library.size() << " files from external library list" << endl;
    }
    else
    {
        cout << "Couldn't read library file" << endl;
    }

        
}
void findFilenameInExtLibrary(std::vector<wstring>& ext_library, std::vector<wstring>& missing, std::vector<wstring>& found)
{
    for(auto look_for_str : missing)
    {   
        std::filesystem::path look_for_path(look_for_str);

        std::wcout << "Looking for [" << look_for_path.stem() << "]";
        bool found_file = false;
        for(auto line : ext_library)
        {
            std::filesystem::path path(line);
            if(path.filename().wstring().find(look_for_path.stem().wstring().substr(0,10)) != wstring::npos)
            {
                if(found_file == true)
                {
                    cout << "found file AGAIN!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
                    std::cin.get();
                }
                found.push_back(line);
                found_file = true;
                std::wcout << "\n\t.......... Found: " << line << '\n';
                string accept;
                cout << "Do you want to accept this match?";
                getline (std::cin, accept);
                if(accept == "Y" || accept == "y")
                {
                    cout << "accepted." << endl;
                }
            }
        }
        cout << '\n';
    }
    cout << "Found " << found.size() << " files" << endl;

}
int main(int argc, char* argv[])
{
    int args_result = process_args(argc, argv);
    if(args_result > 0)
    {
        return args_result;
    }
    cout << "OK" << '\n';

    auto myLibrary = std::make_unique<Library>();
    buildDictionaryOfExistingMusic(myLibrary->ext_library);
    int file_count = 0;
    int missing_file_count = 0;
    int music_file_count = 0;
    cout << "Looking in: " << data_path << '\n';
    for(const auto& entry : fs::directory_iterator(data_path))
    {
        file_count++;
        if(entry.path().extension() == ".csv")
        {
            myLibrary->missing_files.push_back(entry.path().wstring());
        }
        else if(entry.path().extension() == ".mp3")
        {
            myLibrary->music_files.push_back(entry.path().wstring());
        }
    }
    missing_file_count = myLibrary->missing_files.size();
    music_file_count = myLibrary->music_files.size();

    cout << "Found: " << file_count << " files.\n";
    cout << "Found: " << music_file_count << " music files.\n";
    cout << "Found: " << missing_file_count << " metadata files.\n";
    cout << "Found: " << music_file_count + missing_file_count << " total files.\n";
    if(file_count == (missing_file_count + music_file_count))
    {
        cout << "Read ALL files." << endl;
    }
    else
    {
        cout << "Some files not read!!!!!!!!!" << endl;
    }
    findFilenameInExtLibrary(myLibrary->ext_library, myLibrary->missing_files, myLibrary->found_files);
}
