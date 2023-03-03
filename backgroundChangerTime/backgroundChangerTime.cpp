#include <iostream>
#include <chrono>
#include <ctime>
#include <windows.h>
#include <experimental/filesystem>
#include <string>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <fstream>
#include <json/value.h>
#include <vector>
#include <nlohmann/json.hpp>
//#include <nlohmann/json.hpp>

std::experimental::filesystem::path programPath;
std::experimental::filesystem::path path;
class OneTime
{
	public:
		std::string filename;
		int from;
		int until;

		OneTime(std::string filename, int from, int until) {
			this->filename = filename;
			this->from = from;
			this->until = until;
		}
		friend void to_json(nlohmann::json& j, const OneTime& o) {
			j = nlohmann::json{
				{"filename", o.filename},
				{"from", o.from},
				{"until", o.until}
			};
		}
};


void changeBackground(std::experimental::filesystem::path path) {
	const wchar_t* tempPath = path.c_str();

	try {
		SystemParametersInfoW(SPI_SETDESKWALLPAPER, 0, (PVOID)tempPath, SPIF_UPDATEINIFILE);
	}
	catch (std::exception& e) {

	}
}

void iterateThroughFiles(std::string imageName) {
	for (const auto& entry : std::experimental::filesystem::directory_iterator(path)) {
		if (entry.path().filename() == imageName + ".png" || entry.path().filename() == imageName + ".jpg" || entry.path().filename() == imageName + ".bmp" || entry.path().filename() == imageName + ".gif" || entry.path().filename() == imageName + ".tif") {
			changeBackground(entry.path());
		}
	}
}

void SerializeTimes(std::vector<OneTime> allTimes) {
	nlohmann::json jsonObjects = nlohmann::json::array();

	for (const auto& obj : allTimes) {
		jsonObjects.push_back(obj);
	}

	std::ofstream jsonFile("backgroundChangerTimeImages/times.json");
	jsonFile << jsonObjects;
	jsonFile.close();
}

std::vector<OneTime> DeserializeTimes() {
	std::vector<OneTime> allTimes;

	try {
		std::ifstream people_file("backgroundChangerTimeImages/times.json", std::ifstream::binary);
		nlohmann::json j = nlohmann::json::parse(people_file);

		for (auto& element : j) {
			OneTime oneTime(element["filename"], element["from"], element["until"]);
			allTimes.push_back(oneTime);
		}

	}
	catch (std::exception ex) {
	}

	return allTimes;
}

int main()
{
	ShowWindow(GetConsoleWindow(), SW_HIDE);

	std::vector<OneTime> allTimes;
	programPath = std::experimental::filesystem::current_path();
	std::experimental::filesystem::create_directory(programPath / "backgroundChangerTimeImages");
	path = programPath;
	path /= "backgroundChangerTimeImages";

	std::experimental::filesystem::path file{ "backgroundChangerTimeImages/times.json" };
	if (!std::experimental::filesystem::exists(file) || std::experimental::filesystem::is_empty(file)) {
		OneTime timeNight("night",22,7);
		OneTime timeMorning("morning",7,10);
		OneTime timeDay("day",10,18);
		OneTime timeEvening("evening",18,22);

		allTimes.push_back(timeNight);
		allTimes.push_back(timeMorning);
		allTimes.push_back(timeDay);
		allTimes.push_back(timeEvening);

		SerializeTimes(allTimes);
	}

	while (true) {
		auto now = std::chrono::system_clock::now();
		std::time_t nowT = std::chrono::system_clock::to_time_t(now);
		std::tm* nowTm = std::localtime(&nowT);

		allTimes = DeserializeTimes();
		for (OneTime oneTime : allTimes) {
			if (oneTime.until < oneTime.from) {
				if (nowTm->tm_hour >= oneTime.from || nowTm->tm_hour < oneTime.until) {
					iterateThroughFiles(oneTime.filename);
					break;
				}
			}
			else {
				if (nowTm->tm_hour >= oneTime.from && nowTm->tm_hour < oneTime.until) {
					iterateThroughFiles(oneTime.filename);
					break;
				}
			}
		}

		Sleep(900000);
	}
}
