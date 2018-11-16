#ifndef TYPES_MANAGER_
#define TYPES_MANAGER_

#include <string.h>

 /*
 * validate file types sent to the server and generate http header
 * for the specific data type
 */
class types_manager{
private:
	std::string get_file_type (const std::string& file_name);
public:
	bool is_file_supported (const std::string& file_name);
	std::string generate_file_header (const std::string& file_name);
};

#endif //TYPES_MANAGER_
