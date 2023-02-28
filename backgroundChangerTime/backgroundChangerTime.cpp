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

std::experimental::filesystem::path programPath;
std::experimental::filesystem::path path;

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

int main()
{
	ShowWindow(GetConsoleWindow(), SW_HIDE);

	programPath = std::experimental::filesystem::current_path();
	std::experimental::filesystem::create_directory(programPath / "backgroundChangerTimeImages");
	path = programPath;
	path /= "backgroundChangerTimeImages";

	while (true) {
		auto now = std::chrono::system_clock::now();
		std::time_t nowT = std::chrono::system_clock::to_time_t(now);
		std::tm* nowTm = std::localtime(&nowT);

		if (nowTm->tm_hour < 7 || nowTm->tm_hour > 22) {
			iterateThroughFiles("night");
		}
		else if (nowTm->tm_hour < 10) {
			iterateThroughFiles("morning");
		}
		else if (nowTm->tm_hour < 18) {
			iterateThroughFiles("day");
		}
		else if (nowTm->tm_hour < 22) {
			iterateThroughFiles("evening");
		}

		Sleep(1800000);
	}
}
