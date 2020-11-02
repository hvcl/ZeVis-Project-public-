#include "Utils.h"



std::vector<std::string> Utils::Split(const char *phrase, std::string delimiter) {
	std::vector<std::string> list;
	std::string s = std::string(phrase);
	size_t pos = 0;
	std::string token;
	while ((pos = s.find(delimiter)) != std::string::npos) {
		token = s.substr(0, pos);
		list.push_back(token);
		s.erase(0, pos + delimiter.length());
	}
	list.push_back(s);
	return list;
}

std::string Utils::BoolToString(bool b) {
	std::string t = "true";
	std::string f = "false";
	return b ? t : f;
}


void Utils::CalcXYZCount(int *x, int *y, int *z, int level, int x_size, int y_size, int z_size, int blocksize) {
	//level -= 1;
	int size_x = x_size / pow(2,level);
	int size_y = y_size / pow(2, level);
	int size_z = z_size / pow(2, level);

	if (size_x % blocksize == 0)
		*x = size_x / blocksize;
	else
		*x = size_x / blocksize + 1;

	if (size_y % blocksize == 0)
		*y = size_y / blocksize;
	else
		*y = size_y / blocksize + 1;

	if (size_z % blocksize == 0)
		*z = size_z / blocksize;
	else
		*z = size_z / blocksize + 1;


	if (size_z < blocksize)
		*z = 1;
	if (size_y < blocksize)
		*y = 1;
	if (size_x < blocksize)
		*x = 1;

}

QColor Utils::interpolate_color(QColor start, QColor end, double ratio)
{
	int r = (int)(ratio*start.red() + (1 - ratio)*end.red());
	int g = (int)(ratio*start.green() + (1 - ratio)*end.green());
	int b = (int)(ratio*start.blue() + (1 - ratio)*end.blue());
	return QColor::fromRgb(r, g, b);
}

bool Utils::fexists(const char *filename) {
	std::ifstream ifile(filename);
	return (bool)ifile;
}


Utils::Utils()
{
}


Utils::~Utils()
{
}
