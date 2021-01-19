#pragma once
#include <string>
#include <istream>

std::wstring ReadPmxWString(std::istream* _stream, int size);
std::string ReadPmxString(std::istream* _stream, int size);
void PreProcess_MMD_Vector3(Vector3& in, bool not_Vector = false);
Vector3 PreProcess_MMD_Vector3(float x, float y, float z, bool not_Vector = false);