#include <string>
#include <vector>
#include <optional>
#include <filesystem>
#include <algorithm>
#include <iostream>

class file_scanner {
private:
    std::string directory_;
    std::vector<std::string> masks_;
    
    std::string to_lowercase(const std::string& str) const;
    bool matches_mask(const std::string& filename) const;
    
public:
    file_scanner(const std::string& directory, const std::vector<std::string>& masks);

    std::string get_directory() const {return directory_;}
    std::vector<std::string> get_masks() const {return masks_;}

    std::vector<std::filesystem::path> scan_files() const;
};