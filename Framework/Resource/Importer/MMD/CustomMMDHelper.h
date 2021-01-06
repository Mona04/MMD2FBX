#pragma once
#include <string>
#include <istream>

std::wstring ReadPmxWString(std::istream* _stream, int size);
std::string ReadPmxString(std::istream* _stream, int size);
void PreProcessing_MMD_Vector3(Vector3& in, bool isPosition = false);